/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <elog/log.hpp>


namespace audio {
	namespace ess {
		int32_t getLogId();
	}
}
#define EWOLSA_BASE(info,data) ELOG_BASE(audio::ess::getLogId(),info,data)

#define EWOLSA_CRITICAL(data)      EWOLSA_BASE(1, data)
#define EWOLSA_ERROR(data)         EWOLSA_BASE(2, data)
#define EWOLSA_WARNING(data)       EWOLSA_BASE(3, data)
#ifdef DEBUG
	#define EWOLSA_INFO(data)          EWOLSA_BASE(4, data)
	#define EWOLSA_DEBUG(data)         EWOLSA_BASE(5, data)
	#define EWOLSA_VERBOSE(data)       EWOLSA_BASE(6, data)
	#define EWOLSA_TODO(data)          EWOLSA_BASE(4, "TODO : " << data)
#else
	#define EWOLSA_INFO(data)          do { } while(false)
	#define EWOLSA_DEBUG(data)         do { } while(false)
	#define EWOLSA_VERBOSE(data)       do { } while(false)
	#define EWOLSA_TODO(data)          do { } while(false)
#endif

#define EWOLSA_ASSERT(cond,data) \
	do { \
		if (!(cond)) { \
			EWOLSA_CRITICAL(data); \
			assert(!#cond); \
		} \
	} while (0)


