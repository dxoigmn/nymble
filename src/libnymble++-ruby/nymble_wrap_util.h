#ifndef __NYMBLE_WRAP_UTIL_H__
#define __NYMBLE_WRAP_UTIL_H__

#include "ruby.h"

#define Check_Size(v,s)   if (RSTRING_LEN(v) != s) { rb_raise(rb_eArgError, #v " must be of size " #s); }
#define Check_Class(v,c)  if (CLASS_OF(v) != c) { rb_raise(rb_eArgError, #v " must be of class " #c); }

#endif