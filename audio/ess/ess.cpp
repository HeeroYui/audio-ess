/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#include <etk/types.h>

#include <audio/river/Interface.h>
#include <audio/river/Manager.h>
#include <audio/ess/ess.h>
#include <audio/ess/debug.h>


class OutputInterface {
	private:
		std::shared_ptr<audio::river::Manager> m_manager;
		std::shared_ptr<audio::river::Interface> m_interface;
	public:
		OutputInterface() {
			m_manager = audio::river::Manager::create("testApplication");
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
			m_interface->setName("audio::ess::basicOutput");
			// set callback mode ...
			m_interface->setOutputCallback(std::bind(&OutputInterface::onDataNeeded,
			                                         this,
			                                         std::placeholders::_1,
			                                         std::placeholders::_2,
			                                         std::placeholders::_3,
			                                         std::placeholders::_4,
			                                         std::placeholders::_5,
			                                         std::placeholders::_6));
			m_interface->start();
		}
		~OutputInterface() {
			if (m_interface == nullptr) {
				return;
			}
			m_interface->stop();
			m_interface.reset();
			m_manager.reset();
		}
		void onDataNeeded(void* _data,
		                  const std::chrono::system_clock::time_point& _playTime,
		                  const size_t& _nbChunk,
		                  enum audio::format _format,
		                  uint32_t _frequency,
		                  const std::vector<audio::channel>& _map) {
			if (_format != audio::format_int16) {
				EWOLSA_ERROR("call wrong type ... (need int16_t)");
			}
			// call music
			audio::ess::music::getData(static_cast<int16_t*>(_data), _nbChunk, _map.size());
			// call Effects
			audio::ess::effects::getData(static_cast<int16_t*>(_data), _nbChunk, _map.size());
		}
};
std::shared_ptr<OutputInterface> g_ioInterface;

void audio::ess::init() {
	g_ioInterface = std::make_shared<OutputInterface>();
}

void audio::ess::unInit() {
	g_ioInterface.reset();
}


