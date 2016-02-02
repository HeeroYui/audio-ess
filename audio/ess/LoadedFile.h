/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.h>
#include <thread>

namespace audio {
	namespace ess {
		class LoadedFile {
			private:
				int32_t m_uid;
			public:
				int32_t getUId() {
					return m_uid;
				}
			private:
				#if defined(__TARGET_OS__Android)
					pthread_t m_thread;
				#else
					std11::shared_ptr<std11::thread> m_thread;
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
			private:
				#if defined(__TARGET_OS__Android)
					static void* threadCallback(void* _userData);
				#endif
				void threadCall();
		};
	}
}

