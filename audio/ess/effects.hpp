/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>
#include <etk/math/Vector3D.hpp>
#include <audio/river/Interface.hpp>
#include <audio/river/Manager.hpp>
#include <audio/ess/LoadedFile.hpp>
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
				                  const etk::Vector<audio::channel>& _map);
				etk::Vector<etk::Pair<ememory::SharedPtr<audio::ess::LoadedFile>, int32_t>> m_playing; //!< current music read
				etk::Vector<etk::Pair<etk::String, ememory::SharedPtr<audio::ess::LoadedFile>>> m_list; //!< list of all effect loaded
			public:
				void load(const etk::String& _file, const etk::String& _name);
				int32_t getId(const etk::String& _name);
				void play(const etk::String& _name, const vec3& _pos = vec3(0,0,0));
				void play(int32_t _id, const vec3& _pos = vec3(0,0,0));
				void stop();
				void clear();
		};
	}
}

