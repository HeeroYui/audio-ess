/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>
#include <etk/uri/uri.hpp>

namespace audio {
	namespace ess {
		namespace wav {
			etk::Vector<float> loadAudioFile(const etk::Uri& _uri, int8_t _nbChan);
		}
	}
}

