/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>
#include <audio/river/Interface.hpp>
#include <audio/river/Manager.hpp>
#include <audio/ess/LoadedFile.hpp>
#include <mutex>
#include <etk/Map.hpp>


namespace audio {
	namespace ess {
		class Music {
			private:
				mutable std::mutex m_mutex;
				ememory::SharedPtr<audio::river::Manager> m_manager;
				ememory::SharedPtr<audio::river::Interface> m_interface;
			public:
				Music(const ememory::SharedPtr<audio::river::Manager>& _manager);
				~Music();
			private:
				void onDataNeeded(void* _data,
				                  const audio::Time& _playTime,
				                  const size_t& _nbChunk,
				                  enum audio::format _format,
				                  uint32_t _sampleRate,
				                  const etk::Vector<audio::channel>& _map);
				ememory::SharedPtr<audio::ess::LoadedFile> m_current; //!< current music read
				int32_t m_position; //!< current position of music read
				etk::Map<etk::String, ememory::SharedPtr<audio::ess::LoadedFile> > m_list; //!< list of all music loaded
				ememory::SharedPtr<audio::ess::LoadedFile> m_next; //!< next music to read
			public:
				void load(const etk::String& _file, const etk::String& _name);
				void play(const etk::String& _name);
				void stop();
				void clear();
		};
	}
}

