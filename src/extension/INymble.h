/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM INymble.idl
 */

#ifndef __gen_INymble_h__
#define __gen_INymble_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    INymble */
#define INYMBLE_IID_STR "9af36a2f-9b04-4936-acfc-7bffa77bc297"

#define INYMBLE_IID \
  {0x9af36a2f, 0x9b04, 0x4936, \
    { 0xac, 0xfc, 0x7b, 0xff, 0xa7, 0x7b, 0xc2, 0x97 }}

class NS_NO_VTABLE NS_SCRIPTABLE INymble : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(INYMBLE_IID)

  /* void user_initialize (in string pseudonym, [array, size_is (aLength)] in PRUint8 aArray, in PRUint32 aLength); */
  NS_SCRIPTABLE NS_IMETHOD User_initialize(const char *pseudonym, PRUint8 *aArray, PRUint32 aLength) = 0;

  /* void user_set_certificate ([array, size_is (aLength)] in PRUint8 aArray, in PRUint32 aLength); */
  NS_SCRIPTABLE NS_IMETHOD User_set_certificate(PRUint8 *aArray, PRUint32 aLength) = 0;

  /* string user_pseudonym (); */
  NS_SCRIPTABLE NS_IMETHOD User_pseudonym(char **_retval) = 0;

  /* void user_entry_initialize (in string server_id, in string credential, in unsigned long time_period); */
  NS_SCRIPTABLE NS_IMETHOD User_entry_initialize(const char *server_id, const char *credential, PRUint32 time_period) = 0;

  /* boolean user_entry_exists (in string server_id); */
  NS_SCRIPTABLE NS_IMETHOD User_entry_exists(const char *server_id, PRBool *_retval) = 0;

  /* string user_credential_get (in string server_id, in unsigned long time_period); */
  NS_SCRIPTABLE NS_IMETHOD User_credential_get(const char *server_id, PRUint32 time_period, char **_retval) = 0;

  /* boolean user_blacklist_update (in string server_id, in string blacklist, in unsigned long link_window, in unsigned long time_period); */
  NS_SCRIPTABLE NS_IMETHOD User_blacklist_update(const char *server_id, const char *blacklist, PRUint32 link_window, PRUint32 time_period, PRBool *_retval) = 0;

  /* boolean user_blacklist_check (in string server_id); */
  NS_SCRIPTABLE NS_IMETHOD User_blacklist_check(const char *server_id, PRBool *_retval) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(INymble, INYMBLE_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_INYMBLE \
  NS_SCRIPTABLE NS_IMETHOD User_initialize(const char *pseudonym, PRUint8 *aArray, PRUint32 aLength); \
  NS_SCRIPTABLE NS_IMETHOD User_set_certificate(PRUint8 *aArray, PRUint32 aLength); \
  NS_SCRIPTABLE NS_IMETHOD User_pseudonym(char **_retval); \
  NS_SCRIPTABLE NS_IMETHOD User_entry_initialize(const char *server_id, const char *credential, PRUint32 time_period); \
  NS_SCRIPTABLE NS_IMETHOD User_entry_exists(const char *server_id, PRBool *_retval); \
  NS_SCRIPTABLE NS_IMETHOD User_credential_get(const char *server_id, PRUint32 time_period, char **_retval); \
  NS_SCRIPTABLE NS_IMETHOD User_blacklist_update(const char *server_id, const char *blacklist, PRUint32 link_window, PRUint32 time_period, PRBool *_retval); \
  NS_SCRIPTABLE NS_IMETHOD User_blacklist_check(const char *server_id, PRBool *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_INYMBLE(_to) \
  NS_SCRIPTABLE NS_IMETHOD User_initialize(const char *pseudonym, PRUint8 *aArray, PRUint32 aLength) { return _to User_initialize(pseudonym, aArray, aLength); } \
  NS_SCRIPTABLE NS_IMETHOD User_set_certificate(PRUint8 *aArray, PRUint32 aLength) { return _to User_set_certificate(aArray, aLength); } \
  NS_SCRIPTABLE NS_IMETHOD User_pseudonym(char **_retval) { return _to User_pseudonym(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD User_entry_initialize(const char *server_id, const char *credential, PRUint32 time_period) { return _to User_entry_initialize(server_id, credential, time_period); } \
  NS_SCRIPTABLE NS_IMETHOD User_entry_exists(const char *server_id, PRBool *_retval) { return _to User_entry_exists(server_id, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD User_credential_get(const char *server_id, PRUint32 time_period, char **_retval) { return _to User_credential_get(server_id, time_period, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD User_blacklist_update(const char *server_id, const char *blacklist, PRUint32 link_window, PRUint32 time_period, PRBool *_retval) { return _to User_blacklist_update(server_id, blacklist, link_window, time_period, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD User_blacklist_check(const char *server_id, PRBool *_retval) { return _to User_blacklist_check(server_id, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_INYMBLE(_to) \
  NS_SCRIPTABLE NS_IMETHOD User_initialize(const char *pseudonym, PRUint8 *aArray, PRUint32 aLength) { return !_to ? NS_ERROR_NULL_POINTER : _to->User_initialize(pseudonym, aArray, aLength); } \
  NS_SCRIPTABLE NS_IMETHOD User_set_certificate(PRUint8 *aArray, PRUint32 aLength) { return !_to ? NS_ERROR_NULL_POINTER : _to->User_set_certificate(aArray, aLength); } \
  NS_SCRIPTABLE NS_IMETHOD User_pseudonym(char **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->User_pseudonym(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD User_entry_initialize(const char *server_id, const char *credential, PRUint32 time_period) { return !_to ? NS_ERROR_NULL_POINTER : _to->User_entry_initialize(server_id, credential, time_period); } \
  NS_SCRIPTABLE NS_IMETHOD User_entry_exists(const char *server_id, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->User_entry_exists(server_id, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD User_credential_get(const char *server_id, PRUint32 time_period, char **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->User_credential_get(server_id, time_period, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD User_blacklist_update(const char *server_id, const char *blacklist, PRUint32 link_window, PRUint32 time_period, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->User_blacklist_update(server_id, blacklist, link_window, time_period, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD User_blacklist_check(const char *server_id, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->User_blacklist_check(server_id, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public INymble
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_INYMBLE

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, INymble)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* void user_initialize (in string pseudonym, [array, size_is (aLength)] in PRUint8 aArray, in PRUint32 aLength); */
NS_IMETHODIMP _MYCLASS_::User_initialize(const char *pseudonym, PRUint8 *aArray, PRUint32 aLength)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void user_set_certificate ([array, size_is (aLength)] in PRUint8 aArray, in PRUint32 aLength); */
NS_IMETHODIMP _MYCLASS_::User_set_certificate(PRUint8 *aArray, PRUint32 aLength)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* string user_pseudonym (); */
NS_IMETHODIMP _MYCLASS_::User_pseudonym(char **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void user_entry_initialize (in string server_id, in string credential, in unsigned long time_period); */
NS_IMETHODIMP _MYCLASS_::User_entry_initialize(const char *server_id, const char *credential, PRUint32 time_period)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean user_entry_exists (in string server_id); */
NS_IMETHODIMP _MYCLASS_::User_entry_exists(const char *server_id, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* string user_credential_get (in string server_id, in unsigned long time_period); */
NS_IMETHODIMP _MYCLASS_::User_credential_get(const char *server_id, PRUint32 time_period, char **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean user_blacklist_update (in string server_id, in string blacklist, in unsigned long link_window, in unsigned long time_period); */
NS_IMETHODIMP _MYCLASS_::User_blacklist_update(const char *server_id, const char *blacklist, PRUint32 link_window, PRUint32 time_period, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean user_blacklist_check (in string server_id); */
NS_IMETHODIMP _MYCLASS_::User_blacklist_check(const char *server_id, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_INymble_h__ */
