#!/usr/bin/env ruby
require 'mkmf'

$CFLAGS << ' -std=c99 -Wall -Werror -pedantic'

dir_config('ssl', '/usr/local')
have_library('ssl')

find_header('nymble.h', '../libnymble/')
find_header('sexp.h', '../libnymble/sexp/')
find_header('malloc.h', '/usr/include/sys/')

$srcs = Dir['*.c'] + Dir['../libnymble/sexp/*.c'] + Dir['../libnymble/*.c'] 
$objs = $srcs.map { |src| File.basename(src, '.c') + '.o' }

create_makefile('libnymble', '.:../libnymble/:../libnymble/sexp/')
