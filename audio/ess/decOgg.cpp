/**
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <etk/types.hpp>
#include <etk/os/FSNode.hpp>
#include <audio/ess/debug.hpp>
#include <audio/ess/decOgg.hpp>
#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>
#include <ememory/UniquePtr.hpp>


static size_t LocalReadFunc(void *ptr, size_t size, size_t nmemb, void *datasource) {
	etk::FSNode* file = static_cast<etk::FSNode*>(datasource);
	return file->fileRead(ptr, size, nmemb);
}

static int localSeekFunc(void *datasource, ogg_int64_t offset, int whence) {
	etk::FSNode* file = static_cast<etk::FSNode*>(datasource);
	enum etk::seekNode mode = etk::seekNode_start;
	if (whence == SEEK_SET) {
		mode = etk::seekNode_start;
	} else if (whence == SEEK_END) {
		mode = etk::seekNode_end;
	} else if (whence == SEEK_CUR) {
		mode = etk::seekNode_current;
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
	return 0;
}

static long localTellFunc(void *datasource) {
	etk::FSNode* file = static_cast<etk::FSNode*>(datasource);
	return file->fileTell();
}

etk::Vector<float> audio::ess::ogg::loadAudioFile(const etk::String& _filename, int8_t _nbChan) {
	etk::Vector<float> out;
	OggVorbis_File vf;
	int32_t eof=0;
	int32_t current_section;
	ov_callbacks tmpCallback = {
		LocalReadFunc,
		localSeekFunc,
		localCloseFunc,
		localTellFunc
	};
	ememory::UniquePtr<etk::FSNode> fileAccess = ememory::UniquePtr<etk::FSNode>(ETK_NEW(etk::FSNode, _filename));
	// Start loading the XML : 
	//EWOLSA_DEBUG("open file (OGG) \"" << fileAccess << "\"");
	if (false == fileAccess->exist()) {
		EWOLSA_ERROR("File Does not exist : \"" << *fileAccess << "\"");
		return out;
	}
	int32_t fileSize = fileAccess->fileSize();
	if (0 == fileSize) {
		EWOLSA_ERROR("This file is empty : \"" << *fileAccess << "\"");
		return out;
	}
	if (false == fileAccess->fileOpenRead()) {
		EWOLSA_ERROR("Can not open the file : \"" << *fileAccess << "\"");
		return out;
	}
	if (ov_open_callbacks(&(*fileAccess), &vf, nullptr, 0, tmpCallback) < 0) {
		EWOLSA_ERROR("Input does not appear to be an Ogg bitstream.");
		return out;
	}
	vorbis_info *vi=ov_info(&vf,-1);
	int32_t nbSampleOut = ov_pcm_total(&vf,-1) / vi->channels;
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
	out.resize(nbSampleOut*_nbChan, 0);
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
				return out;
			}
		} else {
			int16_t* pointerIn = (int16_t*)pcmout;
			float* pointerOut = &out[0]+pos;
			if (_nbChan == vi->channels) {
				// 1/32768 = 0.00003051757f
				for (int32_t iii=0; iii<ret/2; ++iii) {
					pointerOut[iii] = float(pointerIn[iii]) * 0.00003051757f;
				}
				pos += ret/2;
			} else {
				if (    _nbChan == 1
				     && vi->channels == 2) {
					for (int32_t iii=0; iii<ret/4 ; ++iii) {
						pointerOut[iii] = float(pointerIn[iii*2]) * 0.00003051757f;
					}
					pos += ret/4;
				} else if (    _nbChan == 2
				            && vi->channels == 1) {
					for (int32_t iii=0; iii<ret/2 ; ++iii) {
						pointerOut[iii*2]   = float(pointerIn[iii]) * 0.00003051757f;
						pointerOut[iii*2+1] = float(pointerIn[iii]) * 0.00003051757f;
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
	// cleanup
	ov_clear(&vf);
	//EWOLSA_DEBUG("Done.");
	return out;
}

