/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <audio/ess/debug.h>

int32_t audio::ess::getLogId() {
	static int32_t g_val = etk::log::registerInstance("audio-ess");
	return g_val;
}
