/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD 3 clauses (see license file)
 */


#ifndef __EWOLSA_LOADED_FILE_H__
#define __EWOLSA_LOADED_FILE_H__

#include <etk/types.h>

namespace ewolsa {
	class LoadedFile {
		public :
			LoadedFile(const std::string& _file, int8_t _nbChanRequested=1);
			~LoadedFile(void);
			std::string m_file;
			int32_t m_nbSamples;
			int32_t m_requestedTime;
			int16_t* m_data;
		public:
			const std::string& getName(void) {
				return m_file;
			};
	};
};

#endif

