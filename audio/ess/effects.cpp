/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#include <etk/types.h>

#include <audio/ess/debug.h>
#include <audio/ess/effects.h>
#include <audio/ess/decWav.h>
#include <math.h>



audio::ess::Effect::Effect(const std::shared_ptr<audio::river::Manager>& _manager) :
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
	m_interface->setName("audio::ess::Effect");
	m_interface->addVolumeGroup("EFFECT");
	// set callback mode ...
	m_interface->setOutputCallback(std::bind(&audio::ess::Effect::onDataNeeded,
	                                         this,
	                                         std::placeholders::_1,
	                                         std::placeholders::_2,
	                                         std::placeholders::_3,
	                                         std::placeholders::_4,
	                                         std::placeholders::_5,
	                                         std::placeholders::_6));
}

audio::ess::Effect::~Effect() {
	if (m_interface != nullptr) {
		m_interface.stop();
	}
	m_interface.reset();
	m_manager.reset();
	m_list.clear();
	m_playing.clear();
}


void audio::ess::Effect::onDataNeeded(void* _data,
                                     const audio::Time& _playTime,
                                     const size_t& _nbChunk,
                                     enum audio::format _format,
                                     uint32_t _sampleRate,
                                     const std::vector<audio::channel>& _map){
	if (_format != audio::format_int16) {
		EWOLSA_ERROR("call wrong type ... (need int16_t)");
	}
	std::unique_lock<std::mutex> lck(localMutex);
	if (m_current != m_next) {
		EWOLSA_INFO("change track " << (m_curent!=nullptr?-1:m_current->getUId()) << " ==> " << (m_next!=nullptr?-1:m_next->getUId()));
		m_curent = m_next;
		m_position = 0;
	}
	if (m_curent == nullptr) {
		// nothing to play ...
		return;
	}
	if (m_current->m_data.size() == 0) {
		return;
	}
	int32_t processTimeMax = std::min(_nbChunk*_map.size(), m_current->m_nbSamples - m_position);
	processTimeMax = std::max(0, processTimeMax);
	int16_t * pointer = static_cast<int16_t*>(_data);
	int16_t * newData = &m_current->m_data[m_posiion];
	EWOLSA_DEBUG("AUDIO : Play slot... nb sample : " << processTimeMax << " map=" << _map << " _nbChunk=" << _nbChunk);
	for (int32_t iii=0; iii<processTimeMax; iii++) {
		*pointer++ = *newData++;
	}
	m_position += processTimeMax;
	// check end of playing ...
	if (m_current->m_nbSamples <= m_position) {
		m_position = 0;
		m_current.reset();
	}
}

void audio::ess::Effect::load(const std::string& _file, const std::string& _name) {
	auto it = m_list.find(_name);
	if (it != m_list.end()) {
		return;
	}
	std::shared_ptr<audio::ess::LoadedFile> tmp = std::make_shared<audio::ess::LoadedFile>(_file, 2);
	if (tmp == nullptr) {
		EWOLSA_ERROR("can not load audio Effect = " << _file);
		return;
	}
	std::unique_lock<std::mutex> lock(m_mutex);
	m_list.insert(std::pair<std::string,std::shared_ptr<audio::ess::LoadedFile>>(_name,tmp));
}

void audio::ess::Effect::play(const std::string& _name) {
	auto it = m_list.find(_name);
	if (it != m_list.end()) {
		EWOLSA_ERROR("Can not Play Effect : " << _name);
		return;
	}
	// in all case we stop the current playing Effect ...
	stop();
	std::unique_lock<std::mutex> lock(m_mutex);
	m_next = *it;
	EWOLSA_INFO("Playing track " << (m_curent!=nullptr?-1:m_current->getUId()) << " request next : " << m_next->getUId());
}

void audio::ess::Effect::stop() {
	if (m_curent == nullptr) {
		EWOLSA_INFO("No current audio is playing");
	}
	std::unique_lock<std::mutex> lock(m_mutex);
	m_curent.reset();
}

void audio::ess::Effect::clear() {
	stop();
	m_current.reset();
	m_next.reset();
	m_position = 0;
	m_list.clear();
}




static std::mutex localMutex;
static bool    effectsMute = false;
static float   effectsVolume = 0;
static int32_t effectsVolumeApply = 1<<16;


//----------------------------------------------------------------------------------------------------------
//                               Effects ...
//----------------------------------------------------------------------------------------------------------

class RequestPlay {
	private:
		bool m_freeSlot;
		audio::ess::LoadedFile* m_effect; // reference to the effects
		int32_t m_playTime; // position in sample playing in the audio effects
	public :
		RequestPlay(audio::ess::LoadedFile * _effect) :
		  m_freeSlot(false),
		  m_effect(_effect),
		  m_playTime(0) {
			
		};
		void reset(audio::ess::LoadedFile * _effect) {
			m_effect=_effect;
			m_playTime=0;
			m_freeSlot=false;
		};
		bool isFree() {
			return m_freeSlot;
		};
		void play(int16_t * _bufferInterlace, int32_t _nbSample, int32_t _nbChannels) {
			if (true == m_freeSlot) {
				return;
			}
			if (m_effect->m_data.size() == 0) {
				m_freeSlot = true;
				return;
			}
			int32_t processTimeMax = std::min(_nbSample, m_effect->m_nbSamples - m_playTime);
			processTimeMax = std::max(0, processTimeMax);
			int16_t * pointer = _bufferInterlace;
			int16_t * newData = &m_effect->m_data[m_playTime];
			//EWOLSA_DEBUG("AUDIO : Play slot... nb sample : " << processTimeMax << " playTime=" <<m_playTime << " nbCannels=" << nbChannels);
			for (int32_t iii=0; iii<processTimeMax; iii++) {
				// TODO : set volume and spacialisation ...
				for (int32_t jjj=0; jjj<_nbChannels; jjj++) {
					int32_t tmppp = *pointer + ((((int32_t)*newData)*effectsVolumeApply)>>16);
					*pointer = std::avg(-32767, tmppp, 32766);
					//EWOLSA_DEBUG("AUDIO : element : " << *pointer);
					pointer++;
				}
				newData++;
			}
			m_playTime += processTimeMax;
			// check end of playing ...
			if (m_effect->m_nbSamples <= m_playTime) {
				m_freeSlot=true;
			}
		}
};

#include <vector>
std::vector<audio::ess::LoadedFile*> ListEffects;
std::vector<RequestPlay*> ListEffectsPlaying;

void audio::ess::effects::init() {
	audio::ess::effects::volumeSet(0);
	audio::ess::effects::muteSet(false);
}

void audio::ess::effects::unInit() {
	audio::ess::effects::volumeSet(-1000);
	audio::ess::effects::muteSet(true);
}

int32_t audio::ess::effects::add(const std::string& _file) {
	for (size_t iii=0; iii<ListEffects.size(); iii++) {
		if (ListEffects[iii] == nullptr) {
			continue;
		}
		if (ListEffects[iii]->m_file == _file) {
			ListEffects[iii]->m_requestedTime++;
			return iii;
		}
	}
	// effect does not exist ... create a new one ...
	audio::ess::LoadedFile * tmpEffect = new audio::ess::LoadedFile(_file);
	if (nullptr == tmpEffect) {
		EWOLSA_ERROR("Error to load the effects : \"" << _file << "\"");
		return -1;
	}
	ListEffects.push_back(tmpEffect);
	return ListEffects.size()-1;
}

void audio::ess::effects::rm(int32_t _effectId) {
	// find element ...
	if (_effectId <0 || _effectId >= (int64_t)ListEffects.size()) {
		EWOLSA_ERROR("Wrong effect ID : " << _effectId << " != [0.." << ListEffects.size()-1 << "]  == > can not remove it ...");
		return;
	}
	if (ListEffects[_effectId] == nullptr) {
		EWOLSA_ERROR("effect ID : " << _effectId << "  == > has already been removed");
		return;
	}
	// check number of requested
	if (ListEffects[_effectId]->m_requestedTime  <= 0) {
		EWOLSA_ERROR("effect ID : " << _effectId << "  == > request more than predicted a removed of an effects");
		return;
	}
	ListEffects[_effectId]->m_requestedTime--;
	// mark to be removed ... TODO : Really removed it when no other element readed it ...
	// TODO : ...
}


void audio::ess::effects::play(int32_t _effectId, float _xxx, float _yyy) {
	if (_effectId <0 || _effectId >= (int64_t)ListEffects.size()) {
		EWOLSA_ERROR("Wrong effect ID : " << _effectId << " != [0.." << ListEffects.size()-1 << "]  == > can not play it ...");
		return;
	}
	if (ListEffects[_effectId] == nullptr) {
		EWOLSA_ERROR("effect ID : " << _effectId << "  == > has been removed");
		return;
	}
	EWOLSA_VERBOSE("effect play : " << _effectId );
	// try to find an empty slot :
	for (size_t iii=0; iii<ListEffectsPlaying.size(); iii++) {
		if (ListEffectsPlaying[iii]->isFree()) {
			ListEffectsPlaying[iii]->reset(ListEffects[_effectId]);
			return;
		}
	}
	RequestPlay* newPlay = new RequestPlay(ListEffects[_effectId]);
	if (nullptr == newPlay) {
		EWOLSA_CRITICAL("Allocation error of a playing element : " << _effectId);
		return;
	}
	ListEffectsPlaying.push_back(newPlay);
}


float audio::ess::effects::volumeGet() {
	return effectsVolume;
}


static void uptateEffectVolume() {
	if (effectsMute == true) {
		effectsVolumeApply = 0;
	} else {
		// convert in an fixpoint value
		// V2 = V1*10^(db/20)
		double coef = pow(10, (effectsVolume/20) );
		effectsVolumeApply = (int32_t)(coef * (double)(1<<16));
	}
}

void audio::ess::effects::volumeSet(float _newVolume) {
	effectsVolume = _newVolume;
	effectsVolume = std::avg(-100.0f, effectsVolume, 20.0f);
	EWOLSA_INFO("Set Effect Volume at " << _newVolume << "dB  == > " << effectsVolume << "dB");
	uptateEffectVolume();
}


bool audio::ess::effects::muteGet() {
	return effectsMute;
}


void audio::ess::effects::muteSet(bool _newMute) {
	effectsMute = _newMute;
	EWOLSA_INFO("Set effects Mute at " << _newMute);
	uptateEffectVolume();
}



void audio::ess::effects::getData(int16_t* _bufferInterlace, int32_t _nbSample, int32_t _nbChannels) {
	for (size_t iii = 0; iii < ListEffectsPlaying.size(); ++iii) {
		if (ListEffectsPlaying[iii]!= nullptr) {
			ListEffectsPlaying[iii]->play(_bufferInterlace, _nbSample, _nbChannels);
		}
	}
}

