/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#include <etk/types.h>
#include <etk/thread/tools.h>
#include <audio/ess/debug.h>
#include <audio/ess/LoadedFile.h>
#include <audio/ess/decWav.h>
#include <audio/ess/decOgg.h>
#include <unistd.h>

#if defined(__TARGET_OS__Android)
	static void* threadCallback2(void* _userData) {
#else
	static void threadCallback2(void* _userData) {
#endif
	etk::thread::setName("ewolSA decoder");
	audio::ess::LoadedFile* decodeFile = static_cast<audio::ess::LoadedFile*>(_userData);
	if (decodeFile != nullptr) {
		decodeFile->decode();
	}
	
	#if defined(__TARGET_OS__Android)
		return nullptr;
	#endif
}

void audio::ess::LoadedFile::decode() {
	EWOLSA_INFO("Start decode OGG : " << m_file);
	m_data = audio::ess::ogg::loadAudioFile(m_file, m_nbChanRequested);
	m_nbSamples = m_data.size();
	EWOLSA_INFO("End decode OGG : " << m_file << " size=" << m_nbSamples);
}

audio::ess::LoadedFile::LoadedFile(const std::string& _fileName, int8_t _nbChanRequested) :
  #if !defined(__TARGET_OS__Android)
  	m_thread(nullptr),
  #endif
  m_file(_fileName),
  m_nbSamples(0),
  m_nbChanRequested(_nbChanRequested),
  m_requestedTime(1){
	std::string tmpName = etk::tolower(m_file);
	// select the corect Loader :
	if (etk::end_with(tmpName, ".wav") == true) {
		m_data = audio::ess::wav::loadAudioFile(m_file, m_nbChanRequested);
		m_nbSamples = m_data.size();
	} else if (etk::end_with(tmpName, ".ogg") == true) {
		#if 1
			EWOLSA_DEBUG("create thread");
			#if defined(__TARGET_OS__Android)
				pthread_create(&m_thread, nullptr, &threadCallback2, this);
			#else
				m_thread = new std::thread(&threadCallback2, this);
			#endif
			EWOLSA_DEBUG("done 1");
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			EWOLSA_DEBUG("done 2");
		#else
			EWOLSA_DEBUG("done 1");
			decode();
			EWOLSA_DEBUG("done 2");
		#endif
	} else {
		EWOLSA_ERROR("Extention not managed '" << m_file << "' Sopported extention : .wav / .ogg");
		return;
	}
	/*
	if (m_data == nullptr) {
		// write an error ...
		EWOLSA_ERROR("Can not open file : " << _fileName);
	}
	*/
}


audio::ess::LoadedFile::~LoadedFile() {
	#if defined(__TARGET_OS__Android)
		// TODO : ...
	#else
		if (m_thread != nullptr) {
			delete m_thread;
		}
	#endif
}


