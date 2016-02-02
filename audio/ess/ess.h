/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.h>
#include <audio/ess/effects.h>
#include <audio/ess/music.h>

namespace audio {
	namespace ess {
		void init();
		void unInit();
		
		void soundSetParse(const std::string& _data);
		void soundSetLoad(const std::string& _file);
		
		void musicPlay(const std::string& _name);
		void musicStop();
		void musicSetVolume(float _dB);
		float musicGetVolume();
		void musicSetMute(bool _mute);
		bool musicGetMute();
		
		int32_t effectGetId(const std::string& _name);
		void effectPlay(int32_t _id, const vec3& _pos=vec3(0,0,0));
		void effectPlay(const std::string& _name, const vec3& _pos=vec3(0,0,0));
		void effectSetVolume(float _dB);
		float effectGetVolume();
		void effectSetMute(bool _mute);
		bool effectGetMute();
	}
}

