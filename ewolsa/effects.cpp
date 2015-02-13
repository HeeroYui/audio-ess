/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#include <etk/types.h>

#include <ewolsa/debug.h>
#include <ewolsa/effects.h>
#include <ewolsa/decWav.h>
#include <math.h>
#include <ewolsa/LoadedFile.h>
#include <mutex>

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
		ewolsa::LoadedFile* m_effect; // reference to the effects
		int32_t m_playTime; // position in sample playing in the audio effects
	public :
		RequestPlay(ewolsa::LoadedFile * _effect) :
		  m_freeSlot(false),
		  m_effect(_effect),
		  m_playTime(0) {
			
		};
		void reset(ewolsa::LoadedFile * _effect) {
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
std::vector<ewolsa::LoadedFile*> ListEffects;
std::vector<RequestPlay*> ListEffectsPlaying;

void ewolsa::effects::init() {
	ewolsa::effects::volumeSet(0);
	ewolsa::effects::muteSet(false);
}

void ewolsa::effects::unInit() {
	ewolsa::effects::volumeSet(-1000);
	ewolsa::effects::muteSet(true);
}

int32_t ewolsa::effects::add(const std::string& _file) {
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
	ewolsa::LoadedFile * tmpEffect = new ewolsa::LoadedFile(_file);
	if (nullptr == tmpEffect) {
		EWOLSA_ERROR("Error to load the effects : \"" << _file << "\"");
		return -1;
	}
	ListEffects.push_back(tmpEffect);
	return ListEffects.size()-1;
}

void ewolsa::effects::rm(int32_t _effectId) {
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


void ewolsa::effects::play(int32_t _effectId, float _xxx, float _yyy) {
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


float ewolsa::effects::volumeGet() {
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

void ewolsa::effects::volumeSet(float _newVolume) {
	effectsVolume = _newVolume;
	effectsVolume = std::avg(-100.0f, effectsVolume, 20.0f);
	EWOLSA_INFO("Set music Volume at " << _newVolume << "dB  == > " << effectsVolume << "dB");
	uptateEffectVolume();
}


bool ewolsa::effects::muteGet() {
	return effectsMute;
}


void ewolsa::effects::muteSet(bool _newMute) {
	effectsMute = _newMute;
	EWOLSA_INFO("Set effects Mute at " << _newMute);
	uptateEffectVolume();
}



void ewolsa::effects::getData(int16_t* _bufferInterlace, int32_t _nbSample, int32_t _nbChannels) {
	for (size_t iii = 0; iii < ListEffectsPlaying.size(); ++iii) {
		if (ListEffectsPlaying[iii]!= nullptr) {
			ListEffectsPlaying[iii]->play(_bufferInterlace, _nbSample, _nbChannels);
		}
	}
}

