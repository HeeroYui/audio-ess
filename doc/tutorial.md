Tutoral: use sound-set                              {#audio_ess_tuto}
======================

@tableofcontents

Objectifs:                                          {#audio_ess_tuto_objectif}
==========

  - Understand config file
  - Create a simple playing sounds

Create a configuration file                         {#audio_ess_tuto_config_file}
===========================

configuration file is really simple:
```{.sh}
soundSet.json
```
it contain 2 mains object:
  - **musics:** That containig the list of all music files
  - **effects:** That contain all effects files

A simple example:
```{.json}
{
	musics:{
		"BG-1":"DATA:audio/Clean Soul.ogg"
		"BG-22":"DATA:audio/Dark knight.ogg"
	},
	effects:{
		"end":"DATA:audio/end.wav",
		"error":"DATA:audio/error.wav",
		"levelup":"DATA:audio/levelup.wav",
		"ok":"DATA:audio/ok.wav"
	}
}
```

what you need to do first:                          {#audio_ess_tuto_include}
==========================

Include and basic init:
```{.cpp}
#include <audio/ess/ess.h>

//in the main application:
int main(...) {
	audio::ess::init();
	/*
	 * Do your stuff
	 */
	audio::ess::unInit();
	return 0;
}

```

Select your sound set file:
```{.cpp}
	audio::ess::soundSetLoad("DATA:soundSet.json");
```

Play a background music:                            {#audio_ess_tuto_play_music}
========================

Start playing:
```{.cpp}
	audio::ess::musicPlay("BG-22");
```

Stop playing:
```{.cpp}
	audio::ess::musicStop();
```

Change volume: (reduce at -5dB)
```{.cpp}
	audio::ess::musicSetVolume(-5.0f);
```

Play an effect:                                     {#audio_ess_tuto_play_effect}
===============

Start playing:
```{.cpp}
	audio::ess::effectPlay("error");
	sleep(1);
	audio::ess::effectPlay("levelup");
```

or (faster way):
```{.cpp}
	int32_t effectId = audio::ess::effectGetId("error");
	audio::ess::effectPlay(effectId);
```
**Note:** An effect can not stopped ...

Change volume: (reduce at -5dB)
```{.cpp}
	audio::ess::effectSetVolume(-5.0f);
```
