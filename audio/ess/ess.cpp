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
#include <ejson/ejson.h>

#undef __class__
#define __class__ "audio::ess"

std::shared_ptr<audio::river::Manager> g_audioManager;
std::shared_ptr<audio::ess::Effects> g_effects;
std::shared_ptr<audio::ess::Music> g_music;

void audio::ess::init() {
	g_audioManager = audio::river::Manager::create("ewol-sound-set");
	g_effects = std::make_shared<audio::ess::Effects>(g_audioManager);
	g_music = std::make_shared<audio::ess::Music>(g_audioManager);
}

void audio::ess::unInit() {
	g_effects.reset();
	g_music.reset();
	g_audioManager.reset();
}

void audio::ess::soundSetParse(const std::string& _data) {
	ejson::Document doc;
	doc.parse(_data);
	ejson::Object obj = doc["musics"].toObject();
	if (    obj.exist() == true
	     && g_music != nullptr) {
		for (auto &it : obj.getKeys()) {
			std::string file = obj.getStringValue(it);
			EWOLSA_INFO("load Music : '" << it << "' file=" << file);
			g_music->load(file, it);
		}
	}
	obj = doc["effects"].toObject();
	if (    obj.exist() == true
	     && g_effects != nullptr) {
		for (auto &it : obj.getKeys()) {
			std::string file = obj.getStringValue(it);
			EWOLSA_INFO("load Effect : '" << it << "' file=" << file);
			g_effects->load(file, it);
		}
	}
}

void audio::ess::soundSetLoad(const std::string& _file) {
	soundSetParse(etk::FSNodeReadAllData(_file));
}

void audio::ess::musicPlay(const std::string& _name) {
	if (g_music == nullptr) {
		return;
	}
	g_music->play(_name);
}

void audio::ess::musicStop() {
	if (g_music == nullptr) {
		return;
	}
	g_music->stop();
}

void audio::ess::musicSetVolume(float _dB) {
	if (g_audioManager == nullptr) {
		return;
	}
	g_audioManager->setVolume("MUSIC", _dB);
}

float audio::ess::musicGetVolume() {
	if (g_audioManager == nullptr) {
		return 0.0f;
	}
	return g_audioManager->getVolume("MUSIC");
}

void audio::ess::musicSetMute(bool _mute) {
	if (g_audioManager == nullptr) {
		return;
	}
	g_audioManager->setMute("MUSIC", _mute);
}

bool audio::ess::musicGetMute() {
	if (g_audioManager == nullptr) {
		return 0.0f;
	}
	return g_audioManager->getMute("MUSIC");
}

int32_t audio::ess::effectGetId(const std::string& _name) {
	if (g_effects == nullptr) {
		return -1;
	}
	return g_effects->getId(_name);
}

void audio::ess::effectPlay(int32_t _id, const vec3& _pos) {
	if (g_effects == nullptr) {
		return;
	}
	g_effects->play(_id, _pos);
}

void audio::ess::effectPlay(const std::string& _name, const vec3& _pos) {
	if (g_effects == nullptr) {
		return;
	}
	g_effects->play(_name, _pos);
}

void audio::ess::effectSetVolume(float _dB) {
	if (g_audioManager == nullptr) {
		return;
	}
	g_audioManager->setVolume("EFFECT", _dB);
}

float audio::ess::effectGetVolume() {
	if (g_audioManager == nullptr) {
		return 0.0f;
	}
	return g_audioManager->getVolume("EFFECT");
}

void audio::ess::effectSetMute(bool _mute) {
	if (g_audioManager == nullptr) {
		return;
	}
	g_audioManager->setMute("EFFECT", _mute);
}

bool audio::ess::effectGetMute() {
	if (g_audioManager == nullptr) {
		return 0.0f;
	}
	return g_audioManager->getMute("EFFECT");
}

