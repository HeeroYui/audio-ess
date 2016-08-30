#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug


def get_type():
	return "LIBRARY"

def get_desc():
	return "ewol sound set interface for 'small game' audio effects"

def get_licence():
	return "APACHE-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return ["Mr DUPIN Edouard <yui.heero@gmail.com>"]

def get_version():
	return [0,3,0]

def create(target, module_name):
	my_module = module.Module(__file__, module_name, get_type())
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
	return my_module


