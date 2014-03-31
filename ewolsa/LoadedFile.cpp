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


ewolsa::LoadedFile::LoadedFile(const std::string& _fileName, int8_t _nbChanRequested) :
  m_file(_fileName),
  m_nbSamples(0),
  m_requestedTime(1),
  m_data(NULL){
	std::string tmpName = std::tolower(_fileName);
	// select the corect Loader :
	if (end_with(tmpName, ".wav") == true) {
		m_data = ewolsa::wav::loadAudioFile(_fileName, _nbChanRequested, m_nbSamples);
	} else if (end_with(tmpName, ".ogg") == true) {
		m_data = ewolsa::ogg::loadAudioFile(_fileName, _nbChanRequested, m_nbSamples);
	} else {
		EWOLSA_ERROR("Extention not managed '" << _fileName << "' Sopported extention : .wav / .ogg");
		return;
	}
	if (m_data == NULL) {
		// write an error ...
		EWOLSA_ERROR("Can not open file : " << _fileName);
	}
}


ewolsa::LoadedFile::~LoadedFile(void) {
	if (m_data != NULL) {
		delete[] m_data;
		m_data = NULL;
	}
}


