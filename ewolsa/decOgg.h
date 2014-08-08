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

namespace ewolsa {
	namespace ogg {
		int16_t* loadAudioFile(const std::string& _filename, int8_t _nbChan, int32_t& _nbSampleOut);
		
	}
};

#endif
