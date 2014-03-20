/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD 3 clauses (see license file)
 */


#include <etk/types.h>

#include <ewolsa/music.h>
#include <ewolsa/debug.h>
#include <ewolsa/LoadedFile.h>
#include <math.h>
#include <mutex>

static std::mutex localMutex;

static bool    musicMute = false;
static float   musicVolume = 0;
static int32_t musicVolumeApply = 1<<16;
static int32_t musicFadingTime = 0;
static int32_t musicPositionReading = 0;
static std::vector<ewolsa::LoadedFile*> musicListRead;
static int32_t musicCurrentRead = -1;
static int32_t musicNextRead = -1;

void ewolsa::music::init(void) {
	ewolsa::music::volumeSet(0);
	ewolsa::music::muteSet(false);
	std::unique_lock<std::mutex> lck(localMutex);
	musicCurrentRead = -1;
	musicNextRead = -1;
	musicPositionReading = 0;
	for (size_t iii=0; iii<musicListRead.size(); ++iii) {
		if (musicListRead[iii] == NULL) {
			continue;
		}
		delete musicListRead[iii];
		musicListRead[iii] = NULL;
	}
	musicListRead.clear();
}

void ewolsa::music::unInit(void) {
	ewolsa::music::volumeSet(-1000);
	ewolsa::music::muteSet(true);
	std::unique_lock<std::mutex> lck(localMutex);
	musicCurrentRead = -1;
	musicNextRead = -1;
	musicPositionReading = 0;
	for (size_t iii=0; iii<musicListRead.size(); ++iii) {
		if (musicListRead[iii] == NULL) {
			continue;
		}
		delete musicListRead[iii];
		musicListRead[iii] = NULL;
	}
	musicListRead.clear();
}


void ewolsa::music::fading(int32_t _timeMs) {
	musicFadingTime = _timeMs;
	musicFadingTime = etk_avg(-100, musicFadingTime, 20);
	EWOLSA_INFO("Set music fading time at " << _timeMs << "ms  == > " << musicFadingTime << "ms");
}


void ewolsa::music::preLoad(const std::string& _file) {
	// check if music already existed ...
	for (size_t iii=0; iii<musicListRead.size(); ++iii) {
		if (musicListRead[iii] == NULL) {
			continue;
		}
		if (musicListRead[iii]->getName() == _file) {
			return;
		}
	}
	ewolsa::LoadedFile* tmp = new ewolsa::LoadedFile(_file, 2);
	if (tmp != NULL) {
		if (tmp->m_data == NULL) {
			EWOLSA_ERROR("Music has no data ... : " << _file);
			delete(tmp);
			return;
		}
		std::unique_lock<std::mutex> lck(localMutex);
		musicListRead.push_back(tmp);
	} else {
		EWOLSA_ERROR("can not preload audio Music");
	}
}


bool ewolsa::music::play(const std::string& _file) {
	preLoad(_file);
	// in all case we stop the current playing music ...
	stop();
	int32_t idMusic = -1;
	// check if music already existed ...
	for (size_t iii=0; iii<musicListRead.size(); ++iii) {
		if (musicListRead[iii] == NULL) {
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


bool ewolsa::music::stop(void) {
	if (musicCurrentRead == -1) {
		EWOLSA_INFO("No current audio is playing");
		return false;
	}
	std::unique_lock<std::mutex> lck(localMutex);
	musicNextRead = -1;
	return true;
}



float ewolsa::music::volumeGet(void) {
	return musicVolume;
}


static void uptateMusicVolume(void) {
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

void ewolsa::music::volumeSet(float _newVolume) {
	musicVolume = _newVolume;
	musicVolume = etk_avg(-1000, musicVolume, 40);
	EWOLSA_INFO("Set music Volume at " << _newVolume << "dB  == > " << musicVolume << "dB");
	uptateMusicVolume();
}


bool ewolsa::music::muteGet(void) {
	return musicMute;
}


void ewolsa::music::muteSet(bool _newMute) {
	musicMute = _newMute;
	EWOLSA_INFO("Set music Mute at " << _newMute);
	uptateMusicVolume();
}


void ewolsa::music::getData(int16_t * _bufferInterlace, int32_t _nbSample, int32_t _nbChannels) {
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
	    || musicListRead[musicCurrentRead] == NULL) {
		musicCurrentRead = -1;
		musicPositionReading = 0;
		EWOLSA_ERROR("request read an unexisting audio track ... : " << musicCurrentRead << "/" << musicListRead.size());
		return;
	}
	int32_t processTimeMax = etk_min(_nbSample*_nbChannels, musicListRead[musicCurrentRead]->m_nbSamples - musicPositionReading);
	processTimeMax = etk_max(0, processTimeMax);
	int16_t * pointer = _bufferInterlace;
	int16_t * newData = &musicListRead[musicCurrentRead]->m_data[musicPositionReading];
	//EWOLSA_DEBUG("AUDIO : Play slot... nb sample : " << processTimeMax << " playTime=" <<m_playTime << " nbCannels=" << nbChannels);
	for (int32_t iii=0; iii<processTimeMax; iii++) {
		int32_t tmppp = *pointer + ((((int32_t)*newData)*musicVolumeApply)>>16);
		*pointer = etk_avg(-32767, tmppp, 32766);
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
