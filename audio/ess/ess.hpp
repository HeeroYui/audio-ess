/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>
#include <audio/ess/effects.hpp>
#include <audio/ess/music.hpp>
/**
 * @brief Audio library namespace
 */
namespace audio {
	/**
	 * @brief Audio-ess library namespace
	 */
	namespace ess {
		/**
		 * @brief Initialize the Ess singleton (Only one interface for all the Process)
		 */
		void init();
		/**
		 * @brief Release the Ess singleton (Only one interface for all the Process)
		 */
		void unInit();
		
		/**
		 * @brief Parse a JSON configuration data of a soundset
		 * @param[in] _data JSON string data to parse
		 */
		void soundSetParse(const std::string& _data);
		/**
		 * @brief Parse a configuration file of a soundset
		 * @param[in] _file JSON file to parse
		 */
		void soundSetLoad(const std::string& _file);
		
		/**
		 * @brief Play a music with his name
		 * @param[in] _name Name of the music (not the Path)
		 */
		void musicPlay(const std::string& _name);
		/**
		 * @brief Stop the music played
		 */
		void musicStop();
		/**
		 * @brief Set the music volume
		 * @param[in] _dB apply decibel volume at the music [-100..100]
		 */
		void musicSetVolume(float _dB);
		/**
		 * @brief Get the current music volume
		 * @return Volume in dB of the music
		 */
		float musicGetVolume();
		/**
		 * @brief Set mute state of the music
		 * @param[in] _mute New mute status
		 */
		void musicSetMute(bool _mute);
		/**
		 * @brief Get the current Mute status of the music
		 * @return true The music is muted
		 * @return false The music is active
		 */
		bool musicGetMute();
		
		/**
		 * @brief Get the Unique ID of an effect
		 * @param[in] _name Name of the effects
		 * @return the effect ID or -1 if it does not exist...
		 */
		int32_t effectGetId(const std::string& _name);
		/**
		 * @brief Play an effect with his ID
		 * @param[in] _id Id of the effect to play
		 * @param[in] _pos position of the efffect is played (not used / not implemented)
		 */
		void effectPlay(int32_t _id, const vec3& _pos=vec3(0,0,0));
		/**
		 * @brief Play an effect with his name
		 * @param[in] _name Name of the effect to play
		 * @param[in] _pos position of the efffect is played (not used / not implemented)
		 */
		void effectPlay(const std::string& _name, const vec3& _pos=vec3(0,0,0));
		/**
		 * @brief Set the effects volume
		 * @param[in] _dB apply decibel volume at the effects [-100..100]
		 */
		void effectSetVolume(float _dB);
		/**
		 * @brief Get the current effects volume
		 * @return Volume in dB of the effects
		 */
		float effectGetVolume();
		/**
		 * @brief Set mute state of the effects
		 * @param[in] _mute New mute status
		 */
		void effectSetMute(bool _mute);
		/**
		 * @brief Get the current Mute status of the effects
		 * @return true The effects is muted
		 * @return false The effects is active
		 */
		bool effectGetMute();
	}
}

