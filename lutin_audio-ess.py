#!/usr/bin/python
import lutin.tools as tools
import lutin.debug as debug


def get_type():
	return "LIBRARY"

def get_desc():
	return "ewol sound set interface for 'small game' audio effects"

def get_licence():
	return "MPL-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return "version.txt"

def get_version():
	return "version.txt"

def configure(target, my_module):
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
	    'audio/ess/effects.hpp',
	    'audio/ess/ess.hpp',
	    'audio/ess/music.hpp',
	    'audio/ess/LoadedFile.hpp'
	    ])
	# name of the dependency
	my_module.add_depend(['etk', 'audio-river', 'ogg', 'ejson'])
	if "Android" in target.get_type():
		# TODO : Change this ...
		my_module.add_export_flag('c++', ["-pthread", "-frtti", "-fexceptions"])
	
	my_module.add_path(".")
	return True


