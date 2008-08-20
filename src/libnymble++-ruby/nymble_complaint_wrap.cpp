#include "nymble_complaint_wrap.h"

VALUE rb_complaint_unmarshal(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  u_char* bytes = (u_char*) RSTRING_PTR(rb_bytes);
  u_int size = RSTRING_LEN(rb_bytes);
  Nymble::Complaint* complaint = Nymble::Complaint::unmarshal(bytes, size);
  
  if (complaint == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_complaint_delete, complaint);
}

VALUE rb_complaint_marshal(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cComplaint);
  
  Nymble::Complaint* complaint = (Nymble::Complaint*) DATA_PTR(rb_self);
  u_int marshalled_size = complaint->marshal();
  u_char marshalled[marshalled_size];
  
  complaint->marshal(marshalled, marshalled_size);
  
  return rb_str_new((char*) marshalled, marshalled_size);
}

void rb_complaint_delete(Nymble::Complaint* complaint)
{
  delete complaint;
}