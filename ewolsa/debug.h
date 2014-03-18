/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD v3 (see license file)
 */

#ifndef __EWOLSA_DEBUG_H__
#define __EWOLSA_DEBUG_H__

#include <etk/types.h>
#include <etk/debugGeneric.h>

extern const char * ewolSaLibName;

#define EWOLSA_CRITICAL(data)    ETK_CRITICAL(ewolSaLibName, data)
#define EWOLSA_WARNING(data)     ETK_WARNING(ewolSaLibName, data)
#define EWOLSA_ERROR(data)       ETK_ERROR(ewolSaLibName, data)
#define EWOLSA_INFO(data)        ETK_INFO(ewolSaLibName, data)
#define EWOLSA_DEBUG(data)       ETK_DEBUG(ewolSaLibName, data)
#define EWOLSA_VERBOSE(data)     ETK_VERBOSE(ewolSaLibName, data)
#define EWOLSA_ASSERT(cond,data) ETK_ASSERT(ewolSaLibName, cond, data)
#define EWOLSA_CHECK_INOUT(cond) ETK_CHECK_INOUT(ewolSaLibName, cond)
#define EWOLSA_TODO(cond)        ETK_TODO(ewolSaLibName, cond)

#endif

