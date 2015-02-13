/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#include <etk/types.h>
#include <ewolsa/debug.h>
#include <ewolsa/LoadedFile.h>
#include <ewolsa/decWav.h>
#include <ewolsa/decOgg.h>

static void threadCallback2(void* _userData) {
	etk::log::setThreadName("ewolSA decoder");
	ewolsa::LoadedFile* decodeFile = reinterpret_cast<ewolsa::LoadedFile*>(_userData);
	decodeFile->decode();
}

void ewolsa::LoadedFile::decode() {
	m_data = ewolsa::ogg::loadAudioFile(m_file, m_nbChanRequested);
	m_nbSamples = m_data.size();
}

ewolsa::LoadedFile::LoadedFile(const std::string& _fileName, int8_t _nbChanRequested) :
  m_thread(nullptr),
  m_file(_fileName),
  m_nbSamples(0),
  m_nbChanRequested(_nbChanRequested),
  m_requestedTime(1){
	std::string tmpName = etk::tolower(m_file);
	// select the corect Loader :
	if (etk::end_with(tmpName, ".wav") == true) {
		m_data = ewolsa::wav::loadAudioFile(m_file, m_nbChanRequested);
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


ewolsa::LoadedFile::~LoadedFile() {
	if (m_thread != nullptr) {
		delete m_thread;
	}
}


