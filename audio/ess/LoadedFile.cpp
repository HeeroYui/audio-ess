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

void audio::ess::LoadedFile::threadCallback() {
	//etk::thread::setName("audio-ess decoder");
	//decode();
}
static void threadCallback222() {
	//etk::thread::setName("audio-ess decoder");
	//decode();
}

void audio::ess::LoadedFile::decode() {
	EWOLSA_INFO("Start decode OGG : " << m_file);
	m_data = audio::ess::ogg::loadAudioFile(m_file, m_nbChanRequested);
	m_nbSamples = m_data.size();
	EWOLSA_INFO("End decode OGG : " << m_file << " size=" << m_nbSamples);
}

audio::ess::LoadedFile::LoadedFile(const std::string& _fileName, int8_t _nbChanRequested) :
  m_thread(nullptr),
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
			EWOLSA_INFO("create thread");
			
			//m_thread = std::make_shared<std::thread>(&audio::ess::LoadedFile::threadCallback, this);
			//m_thread = std::make_shared<std::thread>(&threadCallback222);
			static std::thread plop(&threadCallback222);
			plop.detach();
			EWOLSA_INFO("done 1");
			std::this_thread::sleep_for(std::chrono::milliseconds(10000));
			EWOLSA_INFO("done 2");
		#else
			EWOLSA_DEBUG("done 1");
			decode();
			EWOLSA_DEBUG("done 2");
		#endif
	} else {
		EWOLSA_ERROR("Extention not managed '" << m_file << "' Sopported extention : .wav / .ogg");
		return;
	}
}

audio::ess::LoadedFile::~LoadedFile() {
	if (m_thread != nullptr) {
		//m_thread->join();
	}
	//m_thread.reset();
}


