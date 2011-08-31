import Options
import Environment
import sys, os, shutil, glob
from os import unlink, symlink, popen
from os.path import join, dirname, abspath, normpath

srcdir = '.'
blddir = 'build'
VERSION = '0.0.1'

def set_options(opt):
	opt.tool_options('compiler_cxx')
	opt.tool_options('compiler_cc')
	opt.tool_options('misc')
	
	opt.add_option( '--mecab-includes'
		, action='store'
		, type='string'
		, default=False
		, help='Directory containing mecab header files'
		, dest='mecab_includes'
		)
	
	opt.add_option( '--mecab'
		, action='store'
		, type='string'
		, default=False
		, help='Link to a shared mecab libraries'
		, dest='mecab'
		)

def configure(conf):
	conf.check_tool('compiler_cxx')
	if not conf.env.CXX: conf.fatal('c++ compiler not found')
	conf.check_tool('compiler_cc')
	if not conf.env.CC: conf.fatal('c compiler not found')
	conf.check_tool('node_addon')
	
	o = Options.options
	
	if o.mecab_includes:
		conf.env.append_value("CPPFLAGS", '-I%s' % o.mecab_includes)
	
	if o.mecab:
		conf.env.append_value("LINKFLAGS", '-L%s' % o.mecab)
	
	# print conf.env
	
	# check mecab libs
	conf.check_cc( lib='mecab', mandatory=True )

def build(bld):
	print 'build'
	obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
	obj.target = 'MeCab'
	obj.source = './src/mecab.cc'
	obj.includes = ['.']
	obj.lib = ['mecab']

def shutdown(ctx):
	if os.path.exists('build/default/MeCab.node') and not os.path.exists('MeCab.node'):
		os.symlink('build/default/MeCab.node', 'MeCab.node' )
	pass