/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __EWOLSA_DEC_OGG_H__
#define __EWOLSA_DEC_OGG_H__

#include <etk/types.h>


namespace audio {
	namespace ess {
		namespace ogg {
			std::vector<int16_t> loadAudioFile(const std::string& _filename, int8_t _nbChan);
			
		}
	}
}

#endif
