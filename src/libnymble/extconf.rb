#!/usr/bin/env ruby
require 'mkmf'

$CFLAGS << ' -std=c99 -Wall -Werror -pedantic'

dir_config('ssl', '/usr/local')
have_library('ssl')

$objs = Dir['*.c'].map{|x| x[0..-3] + '.o'} + ['sexp/sexp-basic.o', 'sexp/sexp-output.o', 'sexp/sexp-input.o']

COMPILE_C.concat(' -o $*.o')

create_makefile('libnymble')
