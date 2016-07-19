/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.h>
#include <etk/math/Vector3D.h>
#include <audio/river/Interface.h>
#include <audio/river/Manager.h>
#include <audio/ess/LoadedFile.h>
#include <mutex>

namespace audio {
	namespace ess {
		class Effects {
			private:
				mutable std::mutex m_mutex;
				ememory::SharedPtr<audio::river::Manager> m_manager;
				ememory::SharedPtr<audio::river::Interface> m_interface;
			public:
				Effects(const ememory::SharedPtr<audio::river::Manager>& _manager);
				~Effects();
			private:
				void onDataNeeded(void* _data,
				                  const audio::Time& _playTime,
				                  const size_t& _nbChunk,
				                  enum audio::format _format,
				                  uint32_t _sampleRate,
				                  const std::vector<audio::channel>& _map);
				std::vector<std::pair<ememory::SharedPtr<audio::ess::LoadedFile>, int32_t>> m_playing; //!< current music read
				std::vector<std::pair<std::string, ememory::SharedPtr<audio::ess::LoadedFile>>> m_list; //!< list of all effect loaded
			public:
				void load(const std::string& _file, const std::string& _name);
				int32_t getId(const std::string& _name);
				void play(const std::string& _name, const vec3& _pos = vec3(0,0,0));
				void play(int32_t _id, const vec3& _pos = vec3(0,0,0));
				void stop();
				void clear();
		};
	}
}

