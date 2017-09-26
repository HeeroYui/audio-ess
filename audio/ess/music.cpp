/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */


#include <etk/types.hpp>

#include <audio/ess/music.hpp>
#include <audio/ess/debug.hpp>
#include <audio/ess/LoadedFile.hpp>
#include <cmath>

audio::ess::Music::Music(const ememory::SharedPtr<audio::river::Manager>& _manager) :
  m_manager(_manager),
  m_position(0) {
	ethread::UniqueLock lock(m_mutex);
	//Set stereo output:
	etk::Vector<audio::channel> channelMap;
	channelMap.pushBack(audio::channel_frontLeft);
	channelMap.pushBack(audio::channel_frontRight);
	m_interface = m_manager->createOutput(48000,
	                                      channelMap,
	                                      audio::format_float,
	                                      "speaker");
	if (m_interface == nullptr) {
		EWOLSA_ERROR("can not allocate output interface ... ");
		return;
	}
	m_interface->setName("audio::ess::music");
	m_interface->addVolumeGroup("MUSIC");
	// set callback mode ...
	m_interface->setOutputCallback([=] (void* _data,
	                                    const audio::Time& _playTime,
	                                    const size_t& _nbChunk,
	                                    enum audio::format _format,
	                                    uint32_t _sampleRate,
	                                    const etk::Vector<audio::channel>& _map) {
	                                	audio::ess::Music::onDataNeeded(_data, _playTime, _nbChunk, _format, _sampleRate, _map);
	                                });
	m_interface->start();
}

audio::ess::Music::~Music() {
	if (m_interface != nullptr) {
		m_interface->stop();
	}
	m_interface.reset();
	m_manager.reset();
	m_list.clear();
	m_current.reset();
	m_next.reset();
}


void audio::ess::Music::onDataNeeded(void* _data,
                                     const audio::Time& _playTime,
                                     const size_t& _nbChunk,
                                     enum audio::format _format,
                                     uint32_t _sampleRate,
                                     const etk::Vector<audio::channel>& _map){
	if (_format != audio::format_float) {
		EWOLSA_ERROR("call wrong type ... (need float)");
	}
	EWOLSA_VERBOSE("           get data Music: "<< _nbChunk);
	ethread::UniqueLock lock(m_mutex);
	if (m_current != m_next) {
		EWOLSA_INFO("change track " << (m_current==nullptr?-1:m_current->getUId()) << " ==> " << (m_next==nullptr?-1:m_next->getUId()));
		m_current = m_next;
		m_position = 0;
	}
	if (m_current == nullptr) {
		// nothing to play ...
		return;
	}
	if (m_current->m_data.size() == 0) {
		return;
	}
	int32_t processTimeMax = etk::min(int32_t(_nbChunk*_map.size()), int32_t(m_current->m_nbSamples - m_position));
	processTimeMax = etk::max(0, processTimeMax);
	float * pointer = static_cast<float*>(_data);
	float * newData = &m_current->m_data[m_position];
	EWOLSA_VERBOSE("AUDIO : Play slot... nb sample : " << processTimeMax << " map=" << _map << " _nbChunk=" << _nbChunk);
	for (int32_t iii=0; iii<processTimeMax; iii++) {
		*pointer++ = *newData++;
	}
	m_position += processTimeMax;
	// check end of playing ...
	if (m_current->m_nbSamples <= m_position) {
		m_position = 0;
		m_current.reset();
	}
}

void audio::ess::Music::load(const etk::String& _file, const etk::String& _name) {
	auto it = m_list.find(_name);
	if (it != m_list.end()) {
		return;
	}
	ememory::SharedPtr<audio::ess::LoadedFile> tmp = ememory::makeShared<audio::ess::LoadedFile>(_file, 2);
	if (tmp == nullptr) {
		EWOLSA_ERROR("can not load audio Music = " << _file);
		return;
	}
	ethread::UniqueLock lock(m_mutex);
	m_list.add(_name, tmp);
}

void audio::ess::Music::play(const etk::String& _name) {
	auto it = m_list.find(_name);
	if (it == m_list.end()) {
		EWOLSA_ERROR("Can not Play music : " << _name);
		return;
	}
	// in all case we stop the current playing music ...
	stop();
	ethread::UniqueLock lock(m_mutex);
	m_next = it->second;
	EWOLSA_INFO("Playing track " << (m_current==nullptr?-1:m_current->getUId()) << " request next : " << m_next->getUId());
}

void audio::ess::Music::stop() {
	if (m_current == nullptr) {
		EWOLSA_INFO("No current audio is playing");
	}
	ethread::UniqueLock lock(m_mutex);
	m_current.reset();
}

void audio::ess::Music::clear() {
	stop();
	m_current.reset();
	m_next.reset();
	m_position = 0;
	m_list.clear();
}
