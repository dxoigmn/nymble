#!/usr/bin/env ruby
require 'mkmf'

$CFLAGS << ' -Wall -g'

dir_config('ssl', '/opt/local')
have_library('ssl')

dir_config('json', '/opt/local')
have_library('json')

$INCFLAGS << ' -I../libnymble++/ -I../libnymble++/libjson/'

$srcs = Dir['*.cpp'] + Dir['../libnymble++/*.cpp'] + Dir['../libnymble++/libjson/*.cpp']
$objs = $srcs.map { |src| File.basename(src, '.cpp') + '.o' }
$libs = append_library($libs, "supc++")

create_makefile('nymble', '.:../libnymble++/:../libnymble++/libjson/')
