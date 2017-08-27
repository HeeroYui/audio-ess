/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>
#include <thread>
#include <ememory/memory.hpp>

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
					ememory::SharedPtr<std::thread> m_thread;
				#endif
			public:
				LoadedFile(const etk::String& _fileName, int8_t _nbChanRequested=1);
				~LoadedFile();
				etk::String m_file;
				int32_t m_nbSamples;
				int32_t m_nbChanRequested;
				int32_t m_requestedTime;
				etk::Vector<float> m_data;
			public:
				const etk::String& getName() {
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

