/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#ifndef __EWOLSA_LOADED_FILE_H__
#define __EWOLSA_LOADED_FILE_H__

#include <etk/types.h>
#if defined(__TARGET_OS__Android)
	#include <pthread.h>
#else
	#include <thread>
#endif

namespace audio {
	namespace ess {
		class LoadedFile {
			private:
				#if defined(__TARGET_OS__Android)
					pthread_t m_thread;
				#else
					std::thread* m_thread;
				#endif
			public:
				LoadedFile(const std::string& _fileName, int8_t _nbChanRequested=1);
				~LoadedFile();
				std::string m_file;
				int32_t m_nbSamples;
				int32_t m_nbChanRequested;
				int32_t m_requestedTime;
				std::vector<int16_t> m_data;
			public:
				const std::string& getName() {
					return m_file;
				};
				void decode();
		};
	}
}

#endif

