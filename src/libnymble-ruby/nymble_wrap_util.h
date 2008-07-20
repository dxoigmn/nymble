#ifndef __NYMBLE_WRAP_UTIL_H__
#define __NYMBLE_WRAP_UTIL_H__

#define Check_Size(v,s) if (RSTRING(v)->len != s) { rb_raise(rb_eArgError, #v " must be of size " #s); }

#endif