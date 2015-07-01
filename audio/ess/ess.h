/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#ifndef __EWOLSA_H__
#define __EWOLSA_H__

#include <etk/types.h>
#include <audio/ess/effects.h>
#include <audio/ess/music.h>

namespace audio {
	namespace ess {
		void init();
		void unInit();
		
		void loadSoundSet(const std::string& _data);
		void loadSoundSet(const etk::FSNode& _file);
		
		void musicPlay(const std::string& _name);
		void musicStop();
		void musicVolume(float _dB);
		
		int32_t effectGetId(const std::string& _name);
		void effectPlay(int32_t _name, const vec2& _pos=vec2(0,0));
		void effectPlay(const std::string& _name, const vec2& _pos=vec2(0,0));
	}
}

#endif

