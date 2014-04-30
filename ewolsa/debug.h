/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD v3 (see license file)
 */

#ifndef __EWOLSA_DEBUG_H__
#define __EWOLSA_DEBUG_H__

#include <etk/log.h>

namespace ewolsa {
	int32_t getLogId(void);
};
// TODO : Review this problem of multiple intanciation of "std::stringbuf sb"
#define EWOLSA_BASE(info,data) \
	do { \
		if (info <= etk::log::getLevel(ewolsa::getLogId())) { \
			std::stringbuf sb; \
			std::ostream tmpStream(&sb); \
			tmpStream << data; \
			etk::log::logStream(ewolsa::getLogId(), info, __LINE__, __class__, __func__, tmpStream); \
		} \
	} while(0)

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


#endif

