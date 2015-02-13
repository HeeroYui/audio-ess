/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#include <etk/types.h>

#include <river/Interface.h>
#include <river/Manager.h>
#include <ewolsa/ewolsa.h>
#include <ewolsa/debug.h>


class OutputInterface {
	private:
		std::shared_ptr<river::Manager> m_manager;
		std::shared_ptr<river::Interface> m_interface;
	public:
		OutputInterface() {
			m_manager = river::Manager::create("testApplication");
			//Set stereo output:
			std::vector<audio::channel> channelMap;
			channelMap.push_back(audio::channel_frontLeft);
			channelMap.push_back(audio::channel_frontRight);
			m_interface = m_manager->createOutput(48000,
			                                      channelMap,
			                                      audio::format_int16,
			                                      "speaker",
			                                      "ewolsa::basicOutput");
			if (m_interface == nullptr) {
				EWOLSA_ERROR("can not allocate output interface ... ");
				return;
			}
			// set callback mode ...
			m_interface->setOutputCallback(1024,
			                               std::bind(&OutputInterface::onDataNeeded,
			                                         this,
			                                         std::placeholders::_1,
			                                         std::placeholders::_2,
			                                         std::placeholders::_3,
			                                         std::placeholders::_4,
			                                         std::placeholders::_5));
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
		void onDataNeeded(const std::chrono::system_clock::time_point& _playTime,
		                  const size_t& _nbChunk,
		                  const std::vector<audio::channel>& _map,
		                  void* _data,
		                  enum audio::format _type) {
			if (_type != audio::format_int16) {
				EWOLSA_ERROR("call wrong type ... (need int16_t)");
			}
			// call music
			ewolsa::music::getData(static_cast<int16_t*>(_data), _nbChunk, _map.size());
			// call Effects
			//ewolsa::effects::getData(static_cast<int16_t*>(_data), _nbChunk, _map.size());
		}
};
std::shared_ptr<OutputInterface> g_ioInterface;

void ewolsa::init() {
	g_ioInterface = std::make_shared<OutputInterface>();
}

void ewolsa::unInit() {
	g_ioInterface.reset();
}


