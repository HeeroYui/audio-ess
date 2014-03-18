/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD 3 clauses (see license file)
 */

#ifndef __EWOLSA_DEC_WAV_H__
#define __EWOLSA_DEC_WAV_H__

#include <etk/types.h>

namespace ewolsa {
	int16_t* loadAudioFile(const std::string& _filename, int8_t _nbChan, int32_t& _nbSampleOut);
};

#endif

