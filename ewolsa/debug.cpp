/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD 3 clauses (see license file)
 */

#include <ewolsa/debug.h>

int32_t ewolsa::getLogId() {
	static int32_t g_val = etk::log::registerInstance("ewol-sa");
	return g_val;
}
