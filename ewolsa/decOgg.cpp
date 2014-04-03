/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD 3 clauses (see license file)
 */

#include <etk/types.h>
#include <etk/os/FSNode.h>
#include <ewolsa/debug.h>
#include <ewolsa/decOgg.h>
#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>


static size_t LocalReadFunc(void *ptr, size_t size, size_t nmemb, void *datasource) {
	etk::FSNode* file = static_cast<etk::FSNode*>(datasource);
	return file->fileRead(ptr, size, nmemb);
}

static int localSeekFunc(void *datasource, ogg_int64_t offset, int whence) {
	etk::FSNode* file = static_cast<etk::FSNode*>(datasource);
	enum etk::seekNode mode = etk::FSN_SEEK_START;
	if (whence == SEEK_SET) {
		mode = etk::FSN_SEEK_START;
	} else if (whence == SEEK_END) {
		mode = etk::FSN_SEEK_END;
	} else if (whence == SEEK_CUR) {
		mode = etk::FSN_SEEK_CURRENT;
	}
	if (file->fileSeek(offset, mode) == true) {
		return 0;
	} else {
		return -1;
	}
}

static int localCloseFunc(void *datasource) {
	etk::FSNode* file = static_cast<etk::FSNode*>(datasource);
	file->fileClose();
}

static long localTellFunc(void *datasource) {
	etk::FSNode* file = static_cast<etk::FSNode*>(datasource);
	return file->fileTell();
}

int16_t* ewolsa::ogg::loadAudioFile(const std::string& _filename, int8_t _nbChan, int32_t& _nbSampleOut) {
	_nbSampleOut = 0;
	OggVorbis_File vf;
	int32_t eof=0;
	int32_t current_section;
	ov_callbacks tmpCallback = {
		LocalReadFunc,
		localSeekFunc,
		localCloseFunc,
		localTellFunc
	};
	etk::FSNode fileAccess(_filename);
	// Start loading the XML : 
	//EWOLSA_DEBUG("open file (OGG) \"" << fileAccess << "\"");
	if (false == fileAccess.exist()) {
		//EWOLSA_ERROR("File Does not exist : \"" << fileAccess << "\"");
		return NULL;
	}
	int32_t fileSize = fileAccess.fileSize();
	if (0 == fileSize) {
		//EWOLSA_ERROR("This file is empty : \"" << fileAccess << "\"");
		return NULL;
	}
	if (false == fileAccess.fileOpenRead()) {
		//EWOLSA_ERROR("Can not open the file : \"" << fileAccess << "\"");
		return NULL;
	}
	if (ov_open_callbacks(&fileAccess, &vf, NULL, 0, tmpCallback) < 0) {
		//EWOLSA_ERROR("Input does not appear to be an Ogg bitstream.");
		return NULL;
	}
	vorbis_info *vi=ov_info(&vf,-1);
	_nbSampleOut = ov_pcm_total(&vf,-1) / vi->channels;
	/*
	{
		char **ptr=ov_comment(&vf,-1)->user_comments;
		while(*ptr){
			EWOLSA_DEBUG("comment : " << *ptr);
			++ptr;
		}
		EWOLSA_DEBUG("Bitstream is " << (int32_t)vi->channels << " channel, " << (int32_t)vi->rate << "Hz");
		EWOLSA_DEBUG("Decoded length: " << _nbSampleOut << " samples");
		EWOLSA_DEBUG("Encoded by: " << ov_comment(&vf,-1)->vendor);
		EWOLSA_DEBUG("time: " << ((float)_nbSampleOut/(float)vi->rate)/60.0);
	}
	*/
	int16_t* outputData = new int16_t[_nbSampleOut*_nbChan*sizeof(int16_t)];
	if (NULL == outputData) {
		//EWOLSA_ERROR("Allocation ERROR try to allocate " << (int32_t)(_nbSampleOut*_nbChan*sizeof(int16_t) ) << "bytes");
		return NULL;
	}
	int32_t pos = 0;
	char pcmout[4096];
	while(!eof){
		long ret=ov_read(&vf, pcmout, sizeof(pcmout), &current_section);
		if (ret == 0) {
			/* EOF */
			eof=1;
		} else if (ret < 0) {
			if(ret==OV_EBADLINK){
				//EWOLSA_ERROR("Corrupt bitstream section! Exiting.");
				// TODO : Remove pointer data ...
				return NULL;
			}
		} else {
			int16_t* pointerIn = (int16_t*)pcmout;
			int16_t* pointerOut = outputData+pos;
			if (_nbChan == vi->channels) {
				memcpy(pointerOut, pointerIn, ret);
				pos += ret/2;
			} else {
				if (    _nbChan == 1
				     && vi->channels == 2) {
					for (int32_t iii=0; iii<ret/4 ; ++iii) {
						pointerOut[iii] = pointerIn[iii*2];
					}
					pos += ret/4;
				} else if (    _nbChan == 2
				            && vi->channels == 1) {
					for (int32_t iii=0; iii<ret/2 ; ++iii) {
						pointerOut[iii*2]   = pointerIn[iii];
						pointerOut[iii*2+1] = pointerIn[iii];
					}
					pos += ret;
				} else {
					//EWOLSA_ERROR("Can not convert " << (int32_t)vi->channels << " channels in " << _nbChan << " channels");
				}
			}
			// fwrite(pcmout,1,ret,stdout);
			//EWOLSA_DEBUG("get data : " << ret << " Bytes");
		}
	}
	/* cleanup */
	ov_clear(&vf);
	//EWOLSA_DEBUG("Done.");
	return outputData;
}

