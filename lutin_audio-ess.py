#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug

def get_desc():
	return "ewol sound set interface for 'small game' audio effects"


def create(target):
	my_module = module.Module(__file__, 'audio-ess', 'LIBRARY')
	# System core
	my_module.add_src_file([
		'audio/ess/debug.cpp',
		'audio/ess/decWav.cpp',
		'audio/ess/decOgg.cpp',
		'audio/ess/effects.cpp',
		'audio/ess/ess.cpp',
		'audio/ess/music.cpp',
		'audio/ess/LoadedFile.cpp'
		])
	my_module.add_header_file([
		'audio/ess/effects.h',
		'audio/ess/ess.h',
		'audio/ess/music.h',
		'audio/ess/LoadedFile.h'
		])
	
	# name of the dependency
	my_module.add_module_depend(['etk', 'audio-river', 'ogg', 'ejson'])
	if target.name=="Android":
		# TODO : Change this ...
		my_module.add_export_flag('c++', ["-pthread", "-frtti", "-fexceptions"])
	
	my_module.add_path(tools.get_current_path(__file__))
	
	# add the currrent module at the 
	return my_module


