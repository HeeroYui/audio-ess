/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */


#include <etk/types.h>

#include <audio/ess/music.h>
#include <audio/ess/debug.h>
#include <audio/ess/LoadedFile.h>
#include <math.h>

audio::ess::Music::Music(const ememory::SharedPtr<audio::river::Manager>& _manager) :
  m_manager(_manager),
  m_position(0) {
	std::unique_lock<std::mutex> lock(m_mutex);
	//Set stereo output:
	std::vector<audio::channel> channelMap;
	channelMap.push_back(audio::channel_frontLeft);
	channelMap.push_back(audio::channel_frontRight);
	m_interface = m_manager->createOutput(48000,
	                                      channelMap,
	                                      audio::format_int16,
	                                      "speaker");
	if (m_interface == nullptr) {
		EWOLSA_ERROR("can not allocate output interface ... ");
		return;
	}
	m_interface->setName("audio::ess::music");
	m_interface->addVolumeGroup("MUSIC");
	// set callback mode ...
	m_interface->setOutputCallback(std::bind(&audio::ess::Music::onDataNeeded,
	                                         this,
	                                         std::placeholders::_1,
	                                         std::placeholders::_2,
	                                         std::placeholders::_3,
	                                         std::placeholders::_4,
	                                         std::placeholders::_5,
	                                         std::placeholders::_6));
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
                                     const std::vector<audio::channel>& _map){
	if (_format != audio::format_int16) {
		EWOLSA_ERROR("call wrong type ... (need int16_t)");
	}
	std::unique_lock<std::mutex> lock(m_mutex);
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
	int32_t processTimeMax = std::min(int32_t(_nbChunk*_map.size()), int32_t(m_current->m_nbSamples - m_position));
	processTimeMax = std::max(0, processTimeMax);
	int16_t * pointer = static_cast<int16_t*>(_data);
	int16_t * newData = &m_current->m_data[m_position];
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

void audio::ess::Music::load(const std::string& _file, const std::string& _name) {
	auto it = m_list.find(_name);
	if (it != m_list.end()) {
		return;
	}
	ememory::SharedPtr<audio::ess::LoadedFile> tmp = ememory::makeShared<audio::ess::LoadedFile>(_file, 2);
	if (tmp == nullptr) {
		EWOLSA_ERROR("can not load audio Music = " << _file);
		return;
	}
	std::unique_lock<std::mutex> lock(m_mutex);
	m_list.insert(std::pair<std::string,ememory::SharedPtr<audio::ess::LoadedFile>>(_name,tmp));
}

void audio::ess::Music::play(const std::string& _name) {
	auto it = m_list.find(_name);
	if (it == m_list.end()) {
		EWOLSA_ERROR("Can not Play music : " << _name);
		return;
	}
	// in all case we stop the current playing music ...
	stop();
	std::unique_lock<std::mutex> lock(m_mutex);
	m_next = it->second;
	EWOLSA_INFO("Playing track " << (m_current==nullptr?-1:m_current->getUId()) << " request next : " << m_next->getUId());
}

void audio::ess::Music::stop() {
	if (m_current == nullptr) {
		EWOLSA_INFO("No current audio is playing");
	}
	std::unique_lock<std::mutex> lock(m_mutex);
	m_current.reset();
}

void audio::ess::Music::clear() {
	stop();
	m_current.reset();
	m_next.reset();
	m_position = 0;
	m_list.clear();
}
