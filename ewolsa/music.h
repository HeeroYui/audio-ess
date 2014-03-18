/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD 3 clauses (see license file)
 */


#ifndef __EWOLSA_MUSIC_H__
#define __EWOLSA_MUSIC_H__

#include <etk/types.h>

namespace ewolsa {
	namespace music {
		void init(void);
		void unInit(void);
		void fading(int32_t _timeMs);
		// list playing system : is cyclic ...
		bool listAdd(std::string _file);
		bool listRm(std::string _file);
		bool listClean(void);
		bool listPrevious(void);
		bool listNext(void);
		bool listFirst(void);
		bool listLast(void);
		bool listPlay(void); // List playing
		bool listStop(void); // List stopping
		
		bool play(std::string _file); // play specific file ... pause the list element;
		bool stop(void);
		
		// in db
		float volumeGet(void);
		void volumeSet(float _newVolume);
		bool muteGet(void);
		void muteSet(bool _newMute);
		void getData(int16_t * _bufferInterlace, int32_t _nbSample, int32_t _nbChannels);
		
	};
};

#endif

