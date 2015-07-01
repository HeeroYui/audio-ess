/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#include <etk/types.h>

#include <audio/ess/music.h>
#include <audio/ess/debug.h>
#include <audio/ess/LoadedFile.h>
#include <math.h>
#include <mutex>

static std::mutex localMutex;

static bool    musicMute = false;
static float   musicVolume = 0;
static int32_t musicVolumeApply = 1<<16;
static int32_t musicFadingTime = 0;
static int32_t musicPositionReading = 0;
static std::vector<audio::ess::LoadedFile*> musicListRead;
static int32_t musicCurrentRead = -1;
static int32_t musicNextRead = -1;

audio::ess::Music::Music(const std::shared_ptr<audio::river::Manager>& _manager) :
  m_manager(_manager),
  m_position(0) {
	std::unique_lock<std::mutex> lock(m_mutex);
	//Set stereo output:
	std::vector<audio::channel> channelMap;
	channelMap.push_back(audio::channel_frontLeft);
	channelMap.push_back(audio::channel_frontRight);
	m_interface = m_manager->createOutput(48000,
	                                      channelMap,
	                                      audio::format_int16,
	                                      "speaker");
	if (m_interface == nullptr) {
		EWOLSA_ERROR("can not allocate output interface ... ");
		return;
	}
	m_interface->setName("audio::ess::music");
	m_interface->addVolumeGroup("MUSIC");
	// set callback mode ...
	m_interface->setOutputCallback(std::bind(&audio::ess::Music::onDataNeeded,
	                                         this,
	                                         std::placeholders::_1,
	                                         std::placeholders::_2,
	                                         std::placeholders::_3,
	                                         std::placeholders::_4,
	                                         std::placeholders::_5,
	                                         std::placeholders::_6));
}

audio::ess::Music::~Music() {
	if (m_interface != nullptr) {
		m_interface.stop();
	}
	m_interface.reset();
	m_manager.reset();
	m_list.clear();
	m_current.reset();
	m_next.reset();
}


void audio::ess::music::fading(int32_t _timeMs) {
	musicFadingTime = _timeMs;
	musicFadingTime = std::avg(-100, musicFadingTime, 20);
	EWOLSA_INFO("Set music fading time at " << _timeMs << "ms  == > " << musicFadingTime << "ms");
}


void audio::ess::music::preLoad(const std::string& _file) {
	// check if music already existed ...
	for (size_t iii=0; iii<musicListRead.size(); ++iii) {
		if (musicListRead[iii] == nullptr) {
			continue;
		}
		if (musicListRead[iii]->getName() == _file) {
			return;
		}
	}
	audio::ess::LoadedFile* tmp = new audio::ess::LoadedFile(_file, 2);
	if (tmp != nullptr) {
		/*
		if (tmp->m_data == nullptr) {
			EWOLSA_ERROR("Music has no data ... : " << _file);
			delete(tmp);
			return;
		}*/
		std::unique_lock<std::mutex> lck(localMutex);
		musicListRead.push_back(tmp);
	} else {
		EWOLSA_ERROR("can not preload audio Music");
	}
}


bool audio::ess::music::play(const std::string& _file) {
	preLoad(_file);
	// in all case we stop the current playing music ...
	stop();
	int32_t idMusic = -1;
	// check if music already existed ...
	for (size_t iii=0; iii<musicListRead.size(); ++iii) {
		if (musicListRead[iii] == nullptr) {
			continue;
		}
		if (musicListRead[iii]->getName() == _file) {
			idMusic = iii;
			break;
		}
	}
	if (idMusic == -1) {
		return false;
	}
	std::unique_lock<std::mutex> lck(localMutex);
	musicNextRead = idMusic;
	EWOLSA_INFO("Playing track " << musicCurrentRead << " request next : " << idMusic);
	return true;
}


bool audio::ess::music::stop() {
	if (musicCurrentRead == -1) {
		EWOLSA_INFO("No current audio is playing");
		return false;
	}
	std::unique_lock<std::mutex> lck(localMutex);
	musicNextRead = -1;
	return true;
}



float audio::ess::music::volumeGet() {
	return musicVolume;
}


static void uptateMusicVolume() {
	if (musicMute == true) {
		musicVolumeApply = 0;
	} else {
		// convert in an fixpoint value
		// V2 = V1*10^(db/20)
		double coef = pow(10, (musicVolume/20) );
		std::unique_lock<std::mutex> lck(localMutex);
		musicVolumeApply = (int32_t)(coef * (double)(1<<16));
	}
}

void audio::ess::music::volumeSet(float _newVolume) {
	musicVolume = _newVolume;
	musicVolume = std::avg(-1000.0f, musicVolume, 40.0f);
	EWOLSA_INFO("Set music Volume at " << _newVolume << "dB  == > " << musicVolume << "dB");
	uptateMusicVolume();
}


bool audio::ess::music::muteGet() {
	return musicMute;
}


void audio::ess::music::muteSet(bool _newMute) {
	musicMute = _newMute;
	EWOLSA_INFO("Set music Mute at " << _newMute);
	uptateMusicVolume();
}


void audio::ess::music::getData(int16_t * _bufferInterlace, int32_t _nbSample, int32_t _nbChannels) {
	EWOLSA_VERBOSE("Music !!! " << musicCurrentRead << " ... " << musicNextRead << " pos: " << musicPositionReading);
	std::unique_lock<std::mutex> lck(localMutex);
	if (musicCurrentRead != musicNextRead) {
		EWOLSA_DEBUG("change track " << musicCurrentRead << " ==> " << musicNextRead);
		// TODO : create fading ....
		musicCurrentRead = musicNextRead;
		musicPositionReading = 0;
	}
	if (musicCurrentRead < 0) {
		// nothing to play ...
		return;
	}
	if (   musicCurrentRead >= musicListRead.size()
	    || musicListRead[musicCurrentRead] == nullptr) {
		musicCurrentRead = -1;
		musicPositionReading = 0;
		EWOLSA_ERROR("request read an unexisting audio track ... : " << musicCurrentRead << "/" << musicListRead.size());
		return;
	}
	if (musicListRead[musicCurrentRead]->m_data.size() == 0) {
		return;
	}
	int32_t processTimeMax = std::min(_nbSample*_nbChannels, musicListRead[musicCurrentRead]->m_nbSamples - musicPositionReading);
	processTimeMax = std::max(0, processTimeMax);
	int16_t * pointer = _bufferInterlace;
	int16_t * newData = &musicListRead[musicCurrentRead]->m_data[musicPositionReading];
	EWOLSA_DEBUG("AUDIO : Play slot... nb sample : " << processTimeMax << " nbCannels=" << _nbChannels << " chunkRequest=" << _nbSample);
	for (int32_t iii=0; iii<processTimeMax; iii++) {
		/*
		int32_t tmppp = ((int32_t)*pointer) + ((((int32_t)*newData)*musicVolumeApply)>>16);
		*pointer = std::avg(-32767, tmppp, 32766);
		*/
		*pointer = *newData;
		//EWOLSA_DEBUG("AUDIO : element : " << *pointer);
		pointer++;
		newData++;
	}
	musicPositionReading += processTimeMax;
	// check end of playing ...
	if (musicListRead[musicCurrentRead]->m_nbSamples <= musicPositionReading) {
		musicPositionReading = 0;
		musicCurrentRead = -1;
	}
	
}
