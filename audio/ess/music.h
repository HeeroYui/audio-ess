/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.h>
#include <audio/river/Interface.h>
#include <audio/river/Manager.h>
#include <audio/ess/LoadedFile.h>
#include <mutex>
#include <map>


namespace audio {
	namespace ess {
		class Music {
			private:
				mutable std::mutex m_mutex;
				std::shared_ptr<audio::river::Manager> m_manager;
				std::shared_ptr<audio::river::Interface> m_interface;
			public:
				Music(const std::shared_ptr<audio::river::Manager>& _manager);
				~Music();
			private:
				void onDataNeeded(void* _data,
				                  const audio::Time& _playTime,
				                  const size_t& _nbChunk,
				                  enum audio::format _format,
				                  uint32_t _sampleRate,
				                  const std::vector<audio::channel>& _map);
				std::shared_ptr<audio::ess::LoadedFile> m_current; //!< current music read
				int32_t m_position; //!< current position of music read
				std::map<std::string, std::shared_ptr<audio::ess::LoadedFile> > m_list; //!< list of all music loaded
				std::shared_ptr<audio::ess::LoadedFile> m_next; //!< next music to read
			public:
				void load(const std::string& _file, const std::string& _name);
				void play(const std::string& _name);
				void stop();
				void clear();
		};
	}
}

