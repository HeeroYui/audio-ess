/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#ifndef __EWOLSA_EFFECTS_H__
#define __EWOLSA_EFFECTS_H__

#include <etk/types.h>
#include <audio/river/Interface.h>
#include <audio/river/Manager.h>
#include <audio/ess/LoadedFile.h>
#include <mutex>

namespace audio {
	namespace ess {
		class Effect {
			private:
				mutable std::mutex m_mutex;
				std::shared_ptr<audio::river::Manager> m_manager;
				std::shared_ptr<audio::river::Interface> m_interface;
			public:
				Effect(const std::shared_ptr<audio::river::Manager>& _manager);
				~Effect();
			private:
				void onDataNeeded(void* _data,
				                  const audio::Time& _playTime,
				                  const size_t& _nbChunk,
				                  enum audio::format _format,
				                  uint32_t _sampleRate,
				                  const std::vector<audio::channel>& _map);
				std::vector<std::pair<std::shared_ptr<audio::ess::LoadedFile>, int32_t>> m_playing; //!< current music read
				std::vector<std::pair<std::string, std::shared_ptr<audio::ess::LoadedFile>>> m_list; //!< list of all effect loaded
			public:
				void load(const std::string& _file, const std::string& _name);
				int32_t getId(const std::string& _name);
				void play(const std::string& _name, const vec3& pos = vec3(0,0,0));
				void play(int32_t _id, const vec3& pos = vec3(0,0,0));
				void stop();
				void clear();
		};
	}
}

#endif

