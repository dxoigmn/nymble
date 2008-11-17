#include "nymble_pseudonym_manager_wrap.h"

VALUE rb_cPseudonymManager;

VALUE rb_pm_new(VALUE rb_self)
{
  return Data_Wrap_Struct(rb_self, NULL, rb_pm_delete, new Nymble::PseudonymManager());
}

VALUE rb_pm_init(VALUE rb_self, VALUE rb_mac_key_np)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cPseudonymManager);
  Check_Type(rb_mac_key_np, T_STRING);
  
  Nymble::PseudonymManager* pm = (Nymble::PseudonymManager*) DATA_PTR(rb_self);
  std::string hmac_key_np(RSTRING_PTR(rb_mac_key_np), RSTRING_LEN(rb_mac_key_np));
  
  pm->setMacKeyNP(hmac_key_np);
  
  return rb_self;
}

VALUE rb_pm_link_window(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cPseudonymManager);
  
  Nymble::PseudonymManager* pm = (Nymble::PseudonymManager*) DATA_PTR(rb_self);
  
  return INT2FIX(pm->getLinkWindow());
}

VALUE rb_pm_link_window_set(VALUE rb_self, VALUE rb_link_window)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cPseudonymManager);
  Check_Type(rb_link_window, T_FIXNUM);
  
  Nymble::PseudonymManager* pm = (Nymble::PseudonymManager*) DATA_PTR(rb_self);
  u_int link_window = FIX2UINT(rb_link_window);
  
  pm->setLinkWindow(link_window);
  
  return rb_self;
}

VALUE rb_pm_time_period(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cPseudonymManager);
  
  Nymble::PseudonymManager* pm = (Nymble::PseudonymManager*) DATA_PTR(rb_self);
  
  return INT2FIX(pm->getTimePeriod());
}

VALUE rb_pm_time_period_set(VALUE rb_self, VALUE rb_time_period)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cPseudonymManager);
  Check_Type(rb_time_period, T_FIXNUM);
  
  Nymble::PseudonymManager* pm = (Nymble::PseudonymManager*) DATA_PTR(rb_self);
  u_int time_period = FIX2UINT(rb_time_period);
  
  pm->setTimePeriod(time_period);
  
  return rb_self;
}

VALUE rb_pm_create_pseudonym(VALUE rb_self, VALUE rb_user_id)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cPseudonymManager);
  Check_Type(rb_user_id, T_STRING);
  
  Nymble::PseudonymManager* pm = (Nymble::PseudonymManager*) DATA_PTR(rb_self);
  std::string user_id(RSTRING_PTR(rb_user_id), RSTRING_LEN(rb_user_id));
  Nymble::Pseudonym pseudonym;
  
  if (!pm->createPseudonym(user_id, &pseudonym)) {
    return Qnil;
  }
  
  std::string pseudonym_str;
  pseudonym.SerializeToString(&pseudonym_str);
  
  return rb_str_new(pseudonym_str.c_str(), pseudonym_str.length());
}

void rb_pm_delete(Nymble::PseudonymManager* pm)
{
  delete pm;
}