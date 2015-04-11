#!/usr/bin/python
import lutinModule as module
import lutinTools as tools
import lutinDebug as debug

def get_desc():
	return "audio_ess : ewol sound set interface for 'small game' audio effects"


def create(target):
	myModule = module.Module(__file__, 'audio_ess', 'LIBRARY')
	# System core
	myModule.add_src_file([
		'audio/ess/debug.cpp',
		'audio/ess/decWav.cpp',
		'audio/ess/decOgg.cpp',
		'audio/ess/effects.cpp',
		'audio/ess/ess.cpp',
		'audio/ess/music.cpp',
		'audio/ess/LoadedFile.cpp'
		])
	
	# name of the dependency
	myModule.add_module_depend(['etk', 'audio_river', 'ogg'])
	if target.name=="Android":
		# TODO : Change this ...
		myModule.add_export_flag_CC("-pthread -frtti -fexceptions")
	
	myModule.add_export_path(tools.get_current_path(__file__))
	
	# add the currrent module at the 
	return myModule


