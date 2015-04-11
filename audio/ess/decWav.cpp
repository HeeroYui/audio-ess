/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.h>
#include <etk/os/FSNode.h>
#include <audio/ess/debug.h>
#include <audio/ess/decWav.h>


typedef struct {
	char       riffTag[4];     //!< 00-03
	uint32_t   size;           //!< 04-07
	char       waveTag[4];     //!< 08-0b
	char       fmtTag[4];      //!< 0c-0f
	uint32_t   waveFormatSize; //!< 10-13
	struct {
		uint16_t type;           //!< 00-01
		uint16_t channelCount;   //!< 02-03
		uint32_t samplesPerSec;  //!< 04-07
		uint32_t bytesPerSec;    //!< 08-0b
		uint16_t bytesPerFrame;  //!< 0c-0d
		uint16_t bitsPerSample;  //!< 0e-0f
	}waveFormat;               //!< 14-23
	char       dataTag[4];     //!< 24-27
	uint32_t   dataSize;       //!< 28-2b
}waveHeader;


#define CONVERT_UINT32(littleEndien,data) (littleEndien)?(((uint32_t)((uint8_t*)data)[0] | (uint32_t)((uint8_t*)data)[1] << 8 | (uint32_t)((uint8_t*)data)[2] << 16 | (uint32_t)((uint8_t*)data)[3] << 24)): \
                                                         (((uint32_t)((uint8_t*)data)[3] | (uint32_t)((uint8_t*)data)[2] << 8 | (uint32_t)((uint8_t*)data)[1] << 16 | (uint32_t)((uint8_t*)data)[0] << 24))

#define CONVERT_INT32(littleEndien,data)  (littleEndien)?(((int32_t)((uint8_t*)data)[0] | (int32_t)((uint8_t*)data)[1] << 8 | (int32_t)((uint8_t*)data)[2] << 16 | (int32_t)((int8_t*)data)[3] << 24)): \
                                                         (((int32_t)((uint8_t*)data)[3] | (int32_t)((uint8_t*)data)[2] << 8 | (int32_t)((uint8_t*)data)[1] << 16 | (int32_t)((int8_t*)data)[0] << 24))

#define CONVERT_UINT24(littleEndien,data) (littleEndien)?(((uint32_t)((uint8_t*)data)[0]<<8 | (uint32_t)((uint8_t*)data)[1] << 16 | (uint32_t)((uint8_t*)data)[2] << 24)): \
                                                         (((uint32_t)((uint8_t*)data)[2]<<8 | (uint32_t)((uint8_t*)data)[1] << 16 | (uint32_t)((uint8_t*)data)[0] << 24))

#define CONVERT_INT24(littleEndien,data)  (littleEndien)?(((int32_t)((uint8_t*)data)[0]<<8 | (int32_t)((uint8_t*)data)[1] << 16 | (int32_t)((int8_t*)data)[2] << 24)): \
                                                         (((int32_t)((uint8_t*)data)[2]<<8 | (int32_t)((uint8_t*)data)[1] << 16 | (int32_t)((int8_t*)data)[0] << 24))

#define CONVERT_UINT16(littleEndien,data) (littleEndien)?(((uint16_t)((uint8_t*)data)[0] | (uint16_t)((uint8_t*)data)[1] << 8)): \
                                                         (((uint16_t)((uint8_t*)data)[1] | (uint16_t)((uint8_t*)data)[0] << 8))

#define CONVERT_INT16(littleEndien,data)  (littleEndien)?(((int16_t)((uint8_t*)data)[0] | (int16_t)((int8_t*)data)[1] << 8)): \
                                                         (((int16_t)((uint8_t*)data)[1] | (int16_t)((int8_t*)data)[0] << 8))

#define COMPR_PCM   (1)
#define COMPR_MADPCM (2)
#define COMPR_ALAW   (6)
#define COMPR_MULAW  (7)
#define COMPR_ADPCM  (17)
#define COMPR_YADPCM (20)
#define COMPR_GSM    (49)
#define COMPR_G721   (64)
#define COMPR_MPEG   (80)

std::vector<int16_t> audio::ess::wav::loadAudioFile(const std::string& _filename, int8_t _nbChan) {
	waveHeader myHeader;
	memset(&myHeader, 0, sizeof(waveHeader));
	etk::FSNode fileAccess(_filename);
	// Start loading the XML : 
	EWOLSA_DEBUG("open file (WAV) \"" << fileAccess << "\"");

	if (false == fileAccess.exist()) {
		EWOLSA_ERROR("File Does not exist : \"" << fileAccess << "\"");
		return std::vector<int16_t>();
	}
	int32_t fileSize = fileAccess.fileSize();
	if (0 == fileSize) {
		EWOLSA_ERROR("This file is empty : \"" << fileAccess << "\"");
		return std::vector<int16_t>();
	}
	if (false == fileAccess.fileOpenRead()) {
		EWOLSA_ERROR("Can not open the file : \"" << fileAccess << "\"");
		return std::vector<int16_t>();
	}
	// try to find endienness :
	if (fileSize < (int64_t)sizeof(waveHeader)) {
		EWOLSA_ERROR("File : \"" << fileAccess << "\"  == > has not enouth data inside might be minumum of " << (int32_t)(sizeof(waveHeader)));
		return std::vector<int16_t>();
	}
	// ----------------------------------------------
	// read the header :
	// ----------------------------------------------
	if (fileAccess.fileRead(&myHeader.riffTag, 1, 4)!=4) {
		EWOLSA_ERROR("Can not 4 element in the file : \"" << fileAccess << "\"");
		return std::vector<int16_t>();
	}
	bool littleEndien = false;
	if(    myHeader.riffTag[0] == 'R'
	    && myHeader.riffTag[1] == 'I'
	    && myHeader.riffTag[2] == 'F'
	    && (myHeader.riffTag[3] == 'F' || myHeader.riffTag[3] == 'X') ) {
		if (myHeader.riffTag[3] == 'F' ) {
			littleEndien = true;
		}
	} else {
		EWOLSA_ERROR("file: \"" << fileAccess << "\" Does not start with \"RIF\" " );
		return std::vector<int16_t>();
	}
	// get the data size :
	unsigned char tmpData[32];
	if (fileAccess.fileRead(tmpData, 1, 4)!=4) {
		EWOLSA_ERROR("Can not 4 element in the file : \"" << fileAccess << "\"");
		return std::vector<int16_t>();
	}
	myHeader.size = CONVERT_UINT32(littleEndien, tmpData);
	
	// get the data size :
	if (fileAccess.fileRead(&myHeader.waveTag, 1, 4)!=4) {
		EWOLSA_ERROR("Can not 4 element in the file : \"" << fileAccess << "\"");
		return std::vector<int16_t>();
	}
	if(    myHeader.waveTag[0] != 'W'
	    || myHeader.waveTag[1] != 'A'
	    || myHeader.waveTag[2] != 'V'
	    || myHeader.waveTag[3] != 'E' ) {
		EWOLSA_ERROR("file: \"" << fileAccess << "\" This is not a wave file " << myHeader.waveTag[0] << myHeader.waveTag[1] << myHeader.waveTag[2] << myHeader.waveTag[3] );
		return std::vector<int16_t>();
	}
	
	// get the data size :
	if (fileAccess.fileRead(&myHeader.fmtTag, 1, 4)!=4) {
		EWOLSA_ERROR("Can not 4 element in the file : \"" << fileAccess << "\"");
		return std::vector<int16_t>();
	}
	if(    myHeader.fmtTag[0] != 'f'
	    || myHeader.fmtTag[1] != 'm'
	    || myHeader.fmtTag[2] != 't'
	    || myHeader.fmtTag[3] != ' ' ) {
		EWOLSA_ERROR("file: \"" << fileAccess << "\" header error ..."  << myHeader.fmtTag[0] << myHeader.fmtTag[1] << myHeader.fmtTag[2] << myHeader.fmtTag[3]);
		return std::vector<int16_t>();
	}
	// get the data size :
	if (fileAccess.fileRead(tmpData, 1, 4)!=4) {
		EWOLSA_ERROR("Can not 4 element in the file : \"" << fileAccess << "\"");
		return std::vector<int16_t>();
	}
	myHeader.waveFormatSize = CONVERT_UINT32(littleEndien, tmpData);
	
	if (myHeader.waveFormatSize != 16) {
		EWOLSA_ERROR("file : \"" << fileAccess << "\"   == > header error ...");
		return std::vector<int16_t>();
	}
	if (fileAccess.fileRead(tmpData, 1, 16)!=16) {
		EWOLSA_ERROR("Can not 16 element in the file : \"" << fileAccess << "\"");
		return std::vector<int16_t>();
	}
	unsigned char * tmppp = tmpData;
	myHeader.waveFormat.type = CONVERT_UINT16(littleEndien, tmppp);
	tmppp += 2;
	myHeader.waveFormat.channelCount = CONVERT_UINT16(littleEndien, tmppp);
	tmppp += 2;
	myHeader.waveFormat.samplesPerSec = CONVERT_UINT32(littleEndien, tmppp);
	tmppp += 4;
	myHeader.waveFormat.bytesPerSec = CONVERT_UINT32(littleEndien, tmppp);
	tmppp += 4;
	myHeader.waveFormat.bytesPerFrame = CONVERT_UINT16(littleEndien, tmppp);
	tmppp += 2;
	myHeader.waveFormat.bitsPerSample = CONVERT_UINT16(littleEndien, tmppp);
	EWOLSA_DEBUG("audio properties : ");
	EWOLSA_DEBUG("    type : "          << myHeader.waveFormat.type);
	EWOLSA_DEBUG("    channelCount : "  << myHeader.waveFormat.channelCount);
	EWOLSA_DEBUG("    samplesPerSec : " << myHeader.waveFormat.samplesPerSec);
	EWOLSA_DEBUG("    bytesPerSec : "   << myHeader.waveFormat.bytesPerSec);
	EWOLSA_DEBUG("    bytesPerFrame : " << myHeader.waveFormat.bytesPerFrame);
	EWOLSA_DEBUG("    bitsPerSample : " << myHeader.waveFormat.bitsPerSample);
	// get the data size :
	if (fileAccess.fileRead(&myHeader.dataTag, 1, 4)!=4) {
		EWOLSA_ERROR("Can not 4 element in the file : \"" << fileAccess << "\"");
		return std::vector<int16_t>();
	}
	if(    myHeader.dataTag[0] != 'd'
	    || myHeader.dataTag[1] != 'a'
	    || myHeader.dataTag[2] != 't'
	    || myHeader.dataTag[3] != 'a' ) {
		EWOLSA_ERROR("file: \"" << fileAccess << "\" header error ..."  << myHeader.dataTag[0] << myHeader.dataTag[1] << myHeader.dataTag[2] << myHeader.dataTag[3]);
		return std::vector<int16_t>();
	}
	// get the data size :
	if (fileAccess.fileRead(tmpData, 1, 4)!=4) {
		EWOLSA_ERROR("Can not 4 element in the file : \"" << fileAccess << "\"");
		return std::vector<int16_t>();
	}
	myHeader.dataSize = CONVERT_UINT32(littleEndien, tmpData);
	
	// ----------------------------------------------
	// end of the header reading done ...
	// ----------------------------------------------
	
	//Parse the data and transform it if needed ...
	if (COMPR_PCM != myHeader.waveFormat.type) {
		EWOLSA_ERROR("File : \"" << fileAccess << "\"  == > support only PCM compression ...");
		return std::vector<int16_t>();
	}
	if (myHeader.waveFormat.channelCount == 0 || myHeader.waveFormat.channelCount>2) {
		EWOLSA_ERROR("File : \"" << fileAccess << "\"  == > support only mono or stereo ..." << myHeader.waveFormat.channelCount);
		return std::vector<int16_t>();
	}
	if ( ! (    myHeader.waveFormat.bitsPerSample == 16
	         || myHeader.waveFormat.bitsPerSample == 24
	         || myHeader.waveFormat.bitsPerSample == 32 ) ) {
		EWOLSA_ERROR("File : \"" << fileAccess << "\"  == > not supported bit/sample ..." << myHeader.waveFormat.bitsPerSample);
		return std::vector<int16_t>();
	}
	if( ! (   44100 == myHeader.waveFormat.samplesPerSec
	       || 48000 == myHeader.waveFormat.samplesPerSec) ) {
		EWOLSA_ERROR("File : \"" << fileAccess << "\"  == > not supported frequency " << myHeader.waveFormat.samplesPerSec << " != 48000");
		return std::vector<int16_t>();
	}
	EWOLSA_DEBUG("    dataSize : " << myHeader.dataSize);
	//int32_t globalDataSize = myHeader.dataSize;
	int32_t nbSample = (myHeader.dataSize/((myHeader.waveFormat.bitsPerSample/8)*myHeader.waveFormat.channelCount));
	int32_t outputSize = _nbChan*nbSample;
	std::vector<int16_t> outputData;
	outputData.resize(outputSize, 0);
	int16_t * tmpOut = &outputData[0];
	for( int32_t iii=0; iii<nbSample; iii++) {
		int32_t left;
		int32_t right;
		char audioSample[8];
		if (myHeader.waveFormat.bitsPerSample == 16) {
			if (myHeader.waveFormat.channelCount == 1) {
				if (fileAccess.fileRead(audioSample, 1, 2)!=2) {
					EWOLSA_ERROR("Read Error at position : " << iii);
					return std::vector<int16_t>();
				}
				left = ((int32_t)((int16_t)CONVERT_INT16(littleEndien, audioSample))) << 16;
				right = left;
			} else {
				if (fileAccess.fileRead(audioSample, 1, 4)!=4) {
					EWOLSA_ERROR("Read Error at position : " << iii);
					return std::vector<int16_t>();
				}
				left = (int32_t)((int16_t)CONVERT_INT16(littleEndien, audioSample)) << 16;
				right = (int32_t)((int16_t)CONVERT_INT16(littleEndien, audioSample+2)) << 16;
			}
		} else if (myHeader.waveFormat.bitsPerSample == 24) {
			if (myHeader.waveFormat.channelCount == 1) {
				if (fileAccess.fileRead(audioSample, 1, 3)!=3) {
					EWOLSA_ERROR("Read Error at position : " << iii);
					return std::vector<int16_t>();
				}
				left = CONVERT_INT24(littleEndien, audioSample);
				right = left;
			} else {
				if (fileAccess.fileRead(audioSample, 1, 6)!=6) {
					EWOLSA_ERROR("Read Error at position : " << iii);
					return std::vector<int16_t>();
				}
				left = CONVERT_INT24(littleEndien, audioSample);
				right = CONVERT_INT24(littleEndien, audioSample+3);
			}
		} else if (myHeader.waveFormat.bitsPerSample == 32) {
			if (myHeader.waveFormat.channelCount == 1) {
				if (fileAccess.fileRead(audioSample, 1, 4)!=4) {
					EWOLSA_ERROR("Read Error at position : " << iii);
					return std::vector<int16_t>();
				}
				left = CONVERT_INT32(littleEndien, audioSample);
				right = left;
			} else {
				if (fileAccess.fileRead(audioSample, 1, 8)!=8) {
					EWOLSA_ERROR("Read Error at position : " << iii);
					return std::vector<int16_t>();
				}
				left = CONVERT_INT32(littleEndien, audioSample);
				right = CONVERT_INT32(littleEndien, audioSample+4);
			}
		}
		if (_nbChan == 1) {
			*tmpOut++ = (int16_t)(((left>>1) + (right>>1))>>16);
		} else {
			*tmpOut++ = (int16_t)(left>>16);
			*tmpOut++ = (int16_t)(right>>16);
		}
	}
	// close the file:
	fileAccess.fileClose();
	return outputData;
}


