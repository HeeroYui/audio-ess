#!/usr/bin/python
import lutinModule as module
import lutinTools as tools
import lutinDebug as debug

def get_desc():
	return "ewolsa : ewol simple audio interface for 'small game' audio effects"


def create(target):
	myModule = module.Module(__file__, 'ewolsa', 'LIBRARY')
	# System core
	myModule.add_src_file([
		'ewolsa/debug.cpp',
		'ewolsa/decWav.cpp',
		'ewolsa/decOgg.cpp',
		'ewolsa/effects.cpp',
		'ewolsa/ewolsa.cpp',
		'ewolsa/music.cpp',
		'ewolsa/LoadedFile.cpp'
		])
	
	# name of the dependency
	myModule.add_module_depend(['ewol', 'airtaudio', 'ogg'])
	
	myModule.add_export_path(tools.get_current_path(__file__))
	
	# add the currrent module at the 
	return myModule


