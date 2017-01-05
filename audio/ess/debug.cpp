/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <audio/ess/debug.hpp>

int32_t audio::ess::getLogId() {
	static int32_t g_val = elog::registerInstance("audio-ess");
	return g_val;
}
