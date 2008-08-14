#!/usr/bin/env ruby
require 'mkmf'

$CFLAGS << ' -Wall -g'

dir_config('ssl', '/opt/local')
have_library('ssl')

dir_config('protobuf')
have_library('protobuf')

$INCFLAGS << ' -I../libnymble++/'

$srcs = Dir['*.cpp'] + Dir['../libnymble++/*.cpp'] + Dir['../libnymble++/libjson/*.cpp']
$objs = $srcs.map { |src| File.basename(src, '.cpp') + '.o' }

create_makefile('nymble', '.:../libnymble++/')
