/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */


#include <etk/types.hpp>

#include <audio/ess/debug.hpp>
#include <audio/ess/effects.hpp>
#include <audio/ess/decWav.hpp>
#include <cmath>

audio::ess::Effects::Effects(const ememory::SharedPtr<audio::river::Manager>& _manager) :
  m_manager(_manager) {
	ethread::UniqueLock lock(m_mutex);
	//Set stereo output:
	etk::Vector<audio::channel> channelMap;
	channelMap.pushBack(audio::channel_frontCenter);
	m_interface = m_manager->createOutput(48000,
	                                      channelMap,
	                                      audio::format_float,
	                                      "speaker");
	if (m_interface == nullptr) {
		EWOLSA_ERROR("can not allocate output interface ... ");
		return;
	}
	m_interface->setName("audio::ess::Effects");
	m_interface->addVolumeGroup("EFFECT");
	// set callback mode ...
	m_interface->setOutputCallback(std::bind(&audio::ess::Effects::onDataNeeded,
	                                         this,
	                                         std::placeholders::_1,
	                                         std::placeholders::_2,
	                                         std::placeholders::_3,
	                                         std::placeholders::_4,
	                                         std::placeholders::_5,
	                                         std::placeholders::_6));
	m_interface->start();
}

audio::ess::Effects::~Effects() {
	if (m_interface != nullptr) {
		m_interface->stop();
	}
	m_interface.reset();
	m_manager.reset();
	m_list.clear();
	m_playing.clear();
}


static bool playData(const ememory::SharedPtr<audio::ess::LoadedFile>& _file, int32_t& _position, float* _bufferInterlace, int32_t _nbSample) {
	if (    _file == nullptr
	     || _file->m_data.size() == 0) {
		return true;
	}
	int32_t processTimeMax = etk::min(_nbSample, _file->m_nbSamples - _position);
	processTimeMax = etk::max(0, processTimeMax);
	float * pointer = _bufferInterlace;
	const float * newData = &_file->m_data[_position];
	//EWOLSA_DEBUG("AUDIO : Play slot... nb sample : " << processTimeMax << " playTime=" <<m_playTime << " nbCannels=" << nbChannels);
	for (int32_t iii=0; iii<processTimeMax; iii++) {
		*_bufferInterlace += *newData;
		//EWOLSA_DEBUG("AUDIO : element : " << *pointer);
		_bufferInterlace++;
		newData++;
	}
	_position += processTimeMax;
	// check end of playing ...
	if (_file->m_nbSamples <= _position) {
		return true;
	}
	return false;
}

void audio::ess::Effects::onDataNeeded(void* _data,
                                       const audio::Time& _playTime,
                                       const size_t& _nbChunk,
                                       enum audio::format _format,
                                       uint32_t _sampleRate,
                                       const etk::Vector<audio::channel>& _map){
	if (_format != audio::format_float) {
		EWOLSA_ERROR("call wrong type ... (need float)");
	}
	ethread::UniqueLock lock(m_mutex);
	auto it = m_playing.begin();
	while (it != m_playing.end()) {
		bool ret = playData((*it).first, (*it).second, static_cast<float*>(_data), _nbChunk);
		if (ret == true) {
			it = m_playing.erase(it);
		} else {
			++it;
		}
	}
}

void audio::ess::Effects::load(const etk::String& _file, const etk::String& _name) {
	// load the file:
	ememory::SharedPtr<audio::ess::LoadedFile> tmp = ememory::makeShared<audio::ess::LoadedFile>(_file, 2);
	if (tmp == nullptr) {
		EWOLSA_ERROR("can not load audio Effects = " << _file);
		return;
	}
	ethread::UniqueLock lock(m_mutex);
	int32_t id = -1;
	for (size_t iii=0; iii<m_list.size(); ++iii) {
		if (m_list[iii].first == _name) {
			id = iii;
			break;
		}
	}
	if (-1 <= id) {
		m_list.pushBack(etk::Pair<etk::String,ememory::SharedPtr<audio::ess::LoadedFile>>(_name,tmp));
	} else {
		m_list[id].second = tmp;
	}
}

int32_t audio::ess::Effects::getId(const etk::String& _name) {
	ethread::UniqueLock lock(m_mutex);
	for (size_t iii=0; iii<m_list.size(); ++iii) {
		if (m_list[iii].first == _name) {
			return iii;
		}
	}
	m_list.pushBack(etk::Pair<etk::String,ememory::SharedPtr<audio::ess::LoadedFile>>(_name,nullptr));
	EWOLSA_WARNING("Can not find element name : '" << _name << "' added it ... (empty) ");
	return m_list.size()-1;
}

void audio::ess::Effects::play(int32_t _id, const vec3& _pos) {
	ethread::UniqueLock lock(m_mutex);
	if (    _id < 0
	     || _id >= m_list.size()) {
		EWOLSA_ERROR(" Can not play element audio with ID=" << _id << " out of [0.." << m_list.size() << "[");
		return;
	}
	m_playing.pushBack(etk::Pair<ememory::SharedPtr<audio::ess::LoadedFile>, int32_t>(m_list[_id].second, 0));
}

void audio::ess::Effects::play(const etk::String& _name, const vec3& _pos) {
	play(getId(_name),_pos);
}

void audio::ess::Effects::stop() {
	if (m_playing.size() == 0) {
		EWOLSA_INFO("No current audio is playing");
	}
	ethread::UniqueLock lock(m_mutex);
	m_playing.clear();
}

void audio::ess::Effects::clear() {
	stop();
	ethread::UniqueLock lock(m_mutex);
	m_list.clear();
}