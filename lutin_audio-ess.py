#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug

def get_desc():
	return "ewol sound set interface for 'small game' audio effects"


def create(target):
	myModule = module.Module(__file__, 'audio-ess', 'LIBRARY')
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
	myModule.add_header_file([
		'audio/ess/effects.h',
		'audio/ess/ess.h',
		'audio/ess/music.h',
		'audio/ess/LoadedFile.h'
		])
	
	# name of the dependency
	myModule.add_module_depend(['etk', 'audio-river', 'ogg', 'ejson'])
	if target.name=="Android":
		# TODO : Change this ...
		myModule.add_export_flag('c++', ["-pthread", "-frtti", "-fexceptions"])
	
	myModule.add_path(tools.get_current_path(__file__))
	
	# add the currrent module at the 
	return myModule


