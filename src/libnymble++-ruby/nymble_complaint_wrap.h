#ifndef __NYMBLE_COMPLAINT_WRAP_H__
#define __NYMBLE_COMPLAINT_WRAP_H__

#include "ruby.h"
#include "nymble_wrap_util.h"
#include "nymble_complaint.h"

extern VALUE rb_cComplaint;

VALUE rb_complaint_unmarshal(VALUE rb_self, VALUE rb_bytes);
VALUE rb_complaint_marshal(VALUE rb_self);

void rb_complaint_delete(Nymble::Complaint* complaint);

#endif