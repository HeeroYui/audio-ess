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
#include <ewolsa/LoadedFile.h>


static size_t LocalReadFunc(void *ptr, size_t size, size_t nmemb, void *datasource) {
	etk::FSNode* file = static_cast<etk::FSNode*>(datasource);
	EWOLSA_DEBUG("read : " << size << " " << nmemb);
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
	return 0;
}

static long localTellFunc(void *datasource) {
	etk::FSNode* file = static_cast<etk::FSNode*>(datasource);
	return file->fileTell();
}

void* ewolsa::ogg::loadFileThreadedMode(void *_ptr) {
	ewolsa::LoadedFile* self = (ewolsa::LoadedFile*)_ptr;
	if (self == NULL) {
		EWOLSA_ERROR("NULL handle");
		return NULL;
	}
	self->m_data = NULL;
	self->m_nbSamples = 0;
	self->m_nbSamplesTotal = 0;
	OggVorbis_File vf;
	int32_t current_section;
	ov_callbacks tmpCallback = {
		LocalReadFunc,
		localSeekFunc,
		localCloseFunc,
		localTellFunc
	};
	EWOLSA_WARNING("open file (OGG) '" << self->m_file << "'");
	etk::FSNode fileAccess(self->m_file);
	// Start loading the XML : 
	if (false == fileAccess.exist()) {
		EWOLSA_ERROR("File Does not exist : '" << fileAccess << "'");
		return NULL;
	}
	int32_t fileSize = fileAccess.fileSize();
	if (0 == fileSize) {
		EWOLSA_ERROR("This file is empty : '" << fileAccess << "'");
		return NULL;
	}
	EWOLSA_WARNING("ogg file size = " << fileSize);
	if (false == fileAccess.fileOpenRead()) {
		EWOLSA_ERROR("Can not open the file : '" << fileAccess << "'");
		return NULL;
	}
	if (ov_open_callbacks(&fileAccess, &vf, NULL, 0, tmpCallback) < 0) {
		EWOLSA_ERROR("Input does not appear to be an Ogg bitstream.");
		return NULL;
	}
	vorbis_info *vi=ov_info(&vf,-1);
	self->m_nbSamplesTotal = ov_pcm_total(&vf,-1) / vi->channels;
	{
		char **ptr=ov_comment(&vf,-1)->user_comments;
		while(*ptr){
			EWOLSA_DEBUG("comment : " << *ptr);
			++ptr;
		}
		EWOLSA_DEBUG("Bitstream is " << (int32_t)vi->channels << " channel, " << (int32_t)vi->rate << "Hz");
		EWOLSA_DEBUG("Decoded length: " << self->m_nbSamplesTotal << " samples");
		EWOLSA_DEBUG("Encoded by: " << ov_comment(&vf,-1)->vendor);
		EWOLSA_DEBUG("time: " << ((float)self->m_nbSamplesTotal/(float)vi->rate)/60.0);
	}
	self->m_data = new int16_t[self->m_nbSamplesTotal*self->m_nbChanRequested*sizeof(int16_t)];
	if (NULL == self->m_data) {
		EWOLSA_ERROR("Allocation ERROR try to allocate " << (int32_t)(self->m_nbSamplesTotal*self->m_nbChanRequested*sizeof(int16_t) ) << "bytes");
		return NULL;
	}
	
	self->m_nbSamples = 0;
	char pcmout[4096];
	bool eof = false;
	while (    eof == false
	        && self->m_stopRequested == false) {
		long ret=ov_read(&vf, pcmout, sizeof(pcmout), &current_section);
		if (ret == 0) {
			/* EOF */
			eof = true;
		} else if (ret < 0) {
			if(ret==OV_EBADLINK){
				EWOLSA_ERROR("Corrupt bitstream section! Exiting.");
				// TODO : Remove pointer data ...
				return NULL;
			}
		} else {
			int16_t* pointerIn = (int16_t*)pcmout;
			int16_t* pointerOut = self->m_data+self->m_nbSamples;
			if (self->m_nbChanRequested == vi->channels) {
				memcpy(pointerOut, pointerIn, ret);
				self->m_nbSamples += ret/2;
			} else {
				if (    self->m_nbChanRequested == 1
				     && vi->channels == 2) {
					for (int32_t iii=0; iii<ret/4 ; ++iii) {
						pointerOut[iii] = pointerIn[iii*2];
					}
					self->m_nbSamples += ret/4;
				} else if (    self->m_nbChanRequested == 2
				            && vi->channels == 1) {
					for (int32_t iii=0; iii<ret/2 ; ++iii) {
						pointerOut[iii*2]   = pointerIn[iii];
						pointerOut[iii*2+1] = pointerIn[iii];
					}
					self->m_nbSamples += ret;
				} else {
					EWOLSA_ERROR("Can not convert " << (int32_t)vi->channels << " channels in " << self->m_nbChanRequested << " channels");
				}
			}
			// fwrite(pcmout,1,ret,stdout);
			EWOLSA_DEBUG("get data : " << (int32_t)ret << " Bytes");
		}
	}
	/* cleanup */
	ov_clear(&vf);
	EWOLSA_DEBUG("Done.");
	
	return NULL;
}
