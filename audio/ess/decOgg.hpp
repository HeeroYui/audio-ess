/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>

namespace audio {
	namespace ess {
		namespace ogg {
			std::vector<float> loadAudioFile(const std::string& _filename, int8_t _nbChan);
			
		}
	}
}

