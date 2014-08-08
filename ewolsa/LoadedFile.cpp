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

static void* threadCallback2(void *_ptr) {
	ewolsa::LoadedFile* decodeFile = (ewolsa::LoadedFile*)_ptr;
	decodeFile->decode();
	return NULL;
}

void ewolsa::LoadedFile::decode() {
	m_data = ewolsa::ogg::loadAudioFile(m_file, m_nbChanRequested, m_nbSamples);
}

ewolsa::LoadedFile::LoadedFile(const std::string& _fileName, int8_t _nbChanRequested) :
  m_file(_fileName),
  m_nbSamples(0),
  m_nbChanRequested(_nbChanRequested),
  m_requestedTime(1),
  m_data(NULL){
	m_thread = NULL;
	std::string tmpName = std::tolower(m_file);
	// select the corect Loader :
	if (end_with(tmpName, ".wav") == true) {
		m_data = ewolsa::wav::loadAudioFile(m_file, m_nbChanRequested, m_nbSamples);
	} else if (end_with(tmpName, ".ogg") == true) {
		EWOLSA_DEBUG("create thread");
		pthread_create(&m_thread2, NULL, &threadCallback2, this);
		EWOLSA_DEBUG("done 1");
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		EWOLSA_DEBUG("done 2");
	} else {
		EWOLSA_ERROR("Extention not managed '" << m_file << "' Sopported extention : .wav / .ogg");
		return;
	}
	/*
	if (m_data == NULL) {
		// write an error ...
		EWOLSA_ERROR("Can not open file : " << _fileName);
	}
	*/
}


ewolsa::LoadedFile::~LoadedFile() {
	// TODO : wait end of thread...
	if (m_data != NULL) {
		delete[] m_data;
		m_data = NULL;
	}
}


