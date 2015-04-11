/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#ifndef __EWOLSA_MUSIC_H__
#define __EWOLSA_MUSIC_H__

#include <etk/types.h>

namespace audio {
	namespace ess {
		namespace music {
			void init();
			void unInit();
			void fading(int32_t _timeMs);
			
			void preLoad(const std::string& _file);
			bool play(const std::string& _file);
			bool stop();
			
			// in db
			float volumeGet();
			void volumeSet(float _newVolume);
			bool muteGet();
			void muteSet(bool _newMute);
			void getData(int16_t * _bufferInterlace, int32_t _nbSample, int32_t _nbChannels);
		}
	}
}

#endif

