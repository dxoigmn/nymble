#include "nymble_pseudonym_manager_wrap.h"

VALUE rb_pm_new(VALUE rb_self)
{
  return Data_Wrap_Struct(rb_self, NULL, rb_pm_delete, new PseudonymManager());
}

VALUE rb_pm_init(VALUE rb_self, VALUE rb_hmac_key_np)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cPseudonymManager);
  Check_Type(rb_hmac_key_np, T_STRING);
  Check_Size(rb_hmac_key_np, DIGEST_SIZE);
  
  PseudonymManager* pm = (PseudonymManager*) DATA_PTR(rb_self);
  u_char* hmac_key_np = (u_char*) RSTRING_PTR(rb_hmac_key_np);
  
  pm->setHmacKeyNP(hmac_key_np);
  
  return rb_self;
}

VALUE rb_pm_create_pseudonym(VALUE rb_self, VALUE rb_user_id)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cPseudonymManager);
  Check_Type(rb_user_id, T_STRING);
  Check_Size(rb_user_id, DIGEST_SIZE);
  
  PseudonymManager* pm = (PseudonymManager*) DATA_PTR(rb_self);
  u_char* user_id = (u_char*) RSTRING_PTR(rb_user_id);
  
  Pseudonym* pseudonym = pm->createPseudonym(user_id);
  
  return Data_Wrap_Struct(rb_cPseudonym, NULL, rb_pseudonym_delete, pseudonym);
}

void rb_pm_delete(PseudonymManager* pm)
{
  delete pm;
}