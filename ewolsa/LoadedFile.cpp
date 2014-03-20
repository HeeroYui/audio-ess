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


ewolsa::LoadedFile::LoadedFile(const std::string& _file, int8_t _nbChanRequested) :
  m_file(_file),
  m_nbSamples(0),
  m_requestedTime(1),
  m_data(NULL){
	m_data = ewolsa::loadAudioFile(_file, _nbChanRequested, m_nbSamples);
	if (m_data == NULL) {
		// write an error ...
		EWOLSA_ERROR("Can not open file : " << _file);
	}
}


ewolsa::LoadedFile::~LoadedFile(void) {
	if (m_data != NULL) {
		delete[] m_data;
		m_data = NULL;
	}
}


