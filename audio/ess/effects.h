/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#ifndef __EWOLSA_EFFECTS_H__
#define __EWOLSA_EFFECTS_H__

#include <etk/types.h>

namespace audio {
	namespace ess {
		// note effect is loaded in memory (then don't create long effect) and unload only when requested
		namespace effects {
			void init();
			void unInit();
			// note : support file (Mono, 16bit, 48kHz) : .raw or .wav (no encodage) or .ogg (decoded with tremor lib)
			int32_t add(const std::string& _file);
			void rm(int32_t _effectId);
			void play(int32_t _effectId, float _xxx=0, float _yyy=0);
			// in db
			float volumeGet();
			void volumeSet(float _newVolume);
			bool muteGet();
			void muteSet(bool _newMute);
			void getData(int16_t* _bufferInterlace, int32_t _nbSample, int32_t _nbChannels);
			
		};
	}
}

#endif

