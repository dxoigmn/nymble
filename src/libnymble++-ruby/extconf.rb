#!/usr/bin/env ruby
require 'mkmf'

$CFLAGS << ' -Wall -g'

dir_config('ssl', '/opt/local')
have_library('ssl')

dir_config('protobuf')
have_library('protobuf')

$INCFLAGS << ' -I../libnymble++/'

$srcs = Dir['*.cc'] + Dir['../libnymble++/*.cc']
$objs = $srcs.map { |src| File.basename(src, '.cc') + '.o' }

create_makefile('nymble', '.:../libnymble++/')
