/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */


#include <etk/types.hpp>
#include <ethread/tools.hpp>
#include <audio/ess/debug.hpp>
#include <audio/ess/LoadedFile.hpp>
#include <audio/ess/decWav.hpp>
#include <audio/ess/decOgg.hpp>

void audio::ess::LoadedFile::threadCall() {
	decode();
}

void audio::ess::LoadedFile::decode() {
	EWOLSA_ERROR("Start decode OGG : " << m_file);
	m_data = audio::ess::ogg::loadAudioFile(m_file, m_nbChanRequested);
	m_nbSamples = m_data.size();
	EWOLSA_ERROR("End decode OGG : " << m_file << " size=" << m_nbSamples);
}

audio::ess::LoadedFile::LoadedFile(const etk::String& _fileName, int8_t _nbChanRequested) :
  m_file(_fileName),
  m_nbSamples(0),
  m_nbChanRequested(_nbChanRequested),
  m_requestedTime(1) {
	etk::String tmpName = etk::toLower(m_file);
	// select the corect Loader :
	if (etk::end_with(tmpName, ".wav") == true) {
		m_data = audio::ess::wav::loadAudioFile(m_file, m_nbChanRequested);
		m_nbSamples = m_data.size();
	} else if (etk::end_with(tmpName, ".ogg") == true) {
		#if 1
			EWOLSA_INFO("create thread");
			m_thread = ememory::makeShared<ethread::Thread>([&](){decode();}, "audioDecoding");
			if (m_thread == null) {
				EWOLSA_ERROR("Can not create thread ...");
				return;
			}
			ethread::sleepMilliSeconds((1));
		#else
			decode();
		#endif
	} else {
		EWOLSA_ERROR("Extention not managed '" << m_file << "' Sopported extention : .wav / .ogg");
		return;
	}
}

audio::ess::LoadedFile::~LoadedFile() {
	if (m_thread != null) {
		m_thread->join();
	}
	m_thread.reset();
}


