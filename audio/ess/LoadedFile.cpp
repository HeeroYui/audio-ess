/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */


#include <etk/types.h>
#include <ethread/tools.h>
#include <audio/ess/debug.h>
#include <audio/ess/LoadedFile.h>
#include <audio/ess/decWav.h>
#include <audio/ess/decOgg.h>
#include <unistd.h>

#if defined(__TARGET_OS__Android)
	void* audio::ess::LoadedFile::threadCallback(void* _userData) {
		audio::ess::LoadedFile* threadHandle = static_cast<audio::ess::LoadedFile*>(_userData);
		if (threadHandle != nullptr) {
			threadHandle->threadCall();
		}
		return nullptr;
	}
#endif

void audio::ess::LoadedFile::threadCall() {
	decode();
}

void audio::ess::LoadedFile::decode() {
	EWOLSA_ERROR("Start decode OGG : " << m_file);
	m_data = audio::ess::ogg::loadAudioFile(m_file, m_nbChanRequested);
	m_nbSamples = m_data.size();
	EWOLSA_ERROR("End decode OGG : " << m_file << " size=" << m_nbSamples);
}

audio::ess::LoadedFile::LoadedFile(const std::string& _fileName, int8_t _nbChanRequested) :
  #if !defined(__TARGET_OS__Android)
  	m_thread(nullptr),
  #endif
  m_file(_fileName),
  m_nbSamples(0),
  m_nbChanRequested(_nbChanRequested),
  m_requestedTime(1) {
	std::string tmpName = etk::tolower(m_file);
	// select the corect Loader :
	if (etk::end_with(tmpName, ".wav") == true) {
		m_data = audio::ess::wav::loadAudioFile(m_file, m_nbChanRequested);
		m_nbSamples = m_data.size();
	} else if (etk::end_with(tmpName, ".ogg") == true) {
		#if 1
			EWOLSA_INFO("create thread");
			#if defined(__TARGET_OS__Android)
				pthread_create(&m_thread, nullptr, &audio::ess::LoadedFile::threadCallback, this);
			#else
				m_thread = std::make_shared<std::thread>(&audio::ess::LoadedFile::threadCall, this);
				if (m_thread == nullptr) {
					EWOLSA_ERROR("Can not create thread ...");
					return;
				}
			#endif
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		#else
			decode();
		#endif
	} else {
		EWOLSA_ERROR("Extention not managed '" << m_file << "' Sopported extention : .wav / .ogg");
		return;
	}
}

audio::ess::LoadedFile::~LoadedFile() {
	#if defined(__TARGET_OS__Android)
		void* ret = nullptr;
		int val = pthread_join(m_thread, &ret);
	#else
		if (m_thread != nullptr) {
			m_thread->join();
		}
		m_thread.reset();
	#endif
}


