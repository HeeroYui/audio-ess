/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD 3 clauses (see license file)
 */


#include <etk/types.h>

#include <ewolsa/music.h>
#include <ewolsa/debug.h>
#include <math.h>


static bool    musicMute = false;
static float   musicVolume = 0;
static int32_t musicVolumeApply = 1<<16;
static int32_t musicFadingTime = 0;




void ewolsa::music::init(void) {
	ewolsa::music::volumeSet(0);
	ewolsa::music::muteSet(false);
}

void ewolsa::music::unInit(void) {
	ewolsa::music::volumeSet(-1000);
	ewolsa::music::muteSet(true);
}


void ewolsa::music::fading(int32_t _timeMs) {
	musicFadingTime = _timeMs;
	musicFadingTime = etk_avg(-100, musicFadingTime, 20);
	EWOLSA_INFO("Set music fading time at " << _timeMs << "ms  == > " << musicFadingTime << "ms");
}


bool ewolsa::music::listAdd(std::string _file) {
	return false;
}


bool ewolsa::music::listRm(std::string _file) {
	return false;
}


bool ewolsa::music::listClean(void) {
	return false;
}


bool ewolsa::music::listPrevious(void) {
	return false;
}


bool ewolsa::music::listNext(void) {
	return false;
}


bool ewolsa::music::listFirst(void) {
	return false;
}


bool ewolsa::music::listLast(void) {
	return false;
}



bool ewolsa::music::listPlay(void) {
	return false;
}


bool ewolsa::music::listStop(void) {
	return false;
}




bool ewolsa::music::play(std::string _file) {
	return false;
}


bool ewolsa::music::stop(void) {
	return false;
}



float ewolsa::music::volumeGet(void) {
	return musicVolume;
}


static void uptateMusicVolume(void) {
	if (musicMute == true) {
		musicVolumeApply = 0;
	} else {
		// convert in an fixpoint value
		// V2 = V1*10^(db/20)
		double coef = pow(10, (musicVolume/20) );
		musicVolumeApply = (int32_t)(coef * (double)(1<<16));
	}
}

void ewolsa::music::volumeSet(float _newVolume) {
	musicVolume = _newVolume;
	musicVolume = etk_avg(-1000, musicVolume, 40);
	EWOLSA_INFO("Set music Volume at " << _newVolume << "dB  == > " << musicVolume << "dB");
	uptateMusicVolume();
}


bool ewolsa::music::muteGet(void) {
	return musicMute;
}


void ewolsa::music::muteSet(bool _newMute) {
	musicMute = _newMute;
	EWOLSA_INFO("Set music Mute at " << _newMute);
	uptateMusicVolume();
}


void ewolsa::music::getData(int16_t * _bufferInterlace, int32_t _nbSample, int32_t _nbChannels) {
	/*static int32_t maxValue = 0;
	static float angle = 0;
	maxValue +=10;
	if (maxValue > 16000) {
		maxValue = 0;
	}
	for (int iii = 0; iii<nbSample ; iii++) {
		bufferInterlace[iii*2] = (float)maxValue * sin(angle/180.0 * M_PI);
		bufferInterlace[iii*2+1] = bufferInterlace[iii*2];
		angle+=0.9;
		if (angle >= 360) {
			angle -= 360.0;
		}
	}*/
}
