/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#include <etk/types.h>

#include <airtaudio/Interface.h>
#include <ewolsa/ewolsa.h>
#include <ewolsa/debug.h>



airtaudio::Interface* g_dac;

static int airtAudioCallBack(void *_outputBuffer,
                             void *_inputBuffer,
                             unsigned int _nBufferFrames,
                             double _streamTime,
                             airtaudio::streamStatus _status,
                             void* _userData) {
	if (_outputBuffer == NULL) {
		EWOLSA_ERROR("Null output buffer pointer");
		return -1;
	}
	// Reset output data, in case ...
	memset(_outputBuffer, 0, _nBufferFrames*2*sizeof(int16_t));
	// call music
	ewolsa::music::getData((int16_t*)_outputBuffer, _nBufferFrames, 2);
	// call Effects
	ewolsa::effects::getData((int16_t*)_outputBuffer, _nBufferFrames, 2);
	return 0;
}



void ewolsa::init() {
	if (g_dac != NULL) {
		EWOLSA_ERROR("multiple init requested ... at the audio system ...");
		return;
	}
	ewolsa::effects::init();
	ewolsa::music::init();
	g_dac = new airtaudio::Interface();
	if (g_dac == NULL) {
		EWOLSA_ERROR("Can not create AirTAudio interface");
		return;
	}
	g_dac->instanciate();
	if ( g_dac->getDeviceCount() < 1 ) {
		EWOLSA_ERROR("No audio devices found!");
		return;
	}
	airtaudio::StreamParameters parameters;
	parameters.deviceId = g_dac->getDefaultOutputDevice();
	parameters.nChannels = 2;
	parameters.firstChannel = 0;
	unsigned int bufferFrames = 256;
	EWOLSA_DEBUG("init Stream ...");
	g_dac->openStream(&parameters, NULL, airtaudio::SINT16, 48000, &bufferFrames, &airtAudioCallBack, NULL);
	g_dac->startStream();
}


void ewolsa::unInit() {
	if (g_dac == NULL) {
		EWOLSA_ERROR("multiple un-init requested ... at the audio system ...");
		return;
	}
	ewolsa::effects::init();
	ewolsa::music::init();
	
	// Stop the stream
	g_dac->stopStream();
	if ( g_dac->isStreamOpen() ) {
		g_dac->closeStream();
	}
	delete(g_dac);
	g_dac = NULL;
}


