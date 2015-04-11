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

static void threadCallback2(void* _userData) {
	etk::thread::setName("ewolSA decoder");
	audio::ess::LoadedFile* decodeFile = reinterpret_cast<audio::ess::LoadedFile*>(_userData);
	decodeFile->decode();
}

void audio::ess::LoadedFile::decode() {
	m_data = audio::ess::ogg::loadAudioFile(m_file, m_nbChanRequested);
	m_nbSamples = m_data.size();
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
		EWOLSA_DEBUG("create thread");
		m_thread = new std::thread(&threadCallback2, this);
		EWOLSA_DEBUG("done 1");
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		EWOLSA_DEBUG("done 2");
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
	if (m_thread != nullptr) {
		delete m_thread;
	}
}


