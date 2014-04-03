/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD 3 clauses (see license file)
 */


#include <etk/types.h>
#include <ewolsa/debug.h>
#include <ewolsa/LoadedFile.h>
#include <ewolsa/decWav.h>
#include <ewolsa/decOgg.h>
#include <thread>


ewolsa::LoadedFile::LoadedFile(const std::string& _fileName, int8_t _nbChanRequested) :
  m_file(_fileName),
  m_nbSamples(0),
  m_nbSamplesTotal(0),
  m_nbChanRequested(_nbChanRequested),
  m_requestedTime(1),
  m_data(NULL) {
	m_stopRequested = false;
	std::string tmpName = std::tolower(m_file);
	// select the corect Loader :
	if (end_with(tmpName, ".wav") == true) {
		m_data = ewolsa::wav::loadAudioFile(m_file, m_nbChanRequested, m_nbSamples);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		m_nbSamplesTotal = m_nbSamples;
	} else if (end_with(tmpName, ".ogg") == true) {
		pthread_create(&m_thread2, NULL, &ewolsa::ogg::loadFileThreadedMode, this);
	} else {
		EWOLSA_ERROR("Extention not managed '" << m_file << "' Sopported extention : .wav / .ogg");
		return;
	}
}


ewolsa::LoadedFile::~LoadedFile(void) {
	m_stopRequested = true;
	// TODO : wait end of thread...
	if (m_data != NULL) {
		delete[] m_data;
		m_data = NULL;
	}
}


