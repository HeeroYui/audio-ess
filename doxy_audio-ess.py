#!/usr/bin/python
import os
import doxy.module as module
import doxy.debug as debug
import doxy.tools as tools

def create(target, module_name):
	my_module = module.Module(__file__, module_name)
	my_module.set_version("version.txt")
	my_module.set_title("audio-ess: ewol sound set")
	my_module.set_website("http://musicdsp.github.io/" + module_name)
	my_module.set_website_sources("http://github.com/musicdsp/" + module_name)
	my_module.add_path([
	    "audio",
	    "doc"
	    ])
	my_module.add_sample_path([
	    "sample",
	    ])
	my_module.add_depend([
	    'audio-river',
	    'ejson'
	    ])
	my_module.add_exclude_symbols([
	    '*operator<<*',
	    ])
	my_module.add_exclude_file([
	    'debug.h',
	    ])
	my_module.add_file_patterns([
	    'ess.h',
	    '*.md',
	    ])
	
	return my_module