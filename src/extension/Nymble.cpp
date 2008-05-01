#include "Nymble.h"

NS_IMPL_ISUPPORTS1(Nymble, INymble)

Nymble::Nymble()
{
  /* member initializers and constructor code */
}

Nymble::~Nymble()
{
  /* destructor code */
}

/* void user_initialize (in string pseudonym, [array, size_is(aLength)] in PRUint8 aArray, in PRUint32 aLength); */
NS_IMETHODIMP Nymble::User_initialize(const char *pseudonym, PRUint8 *aArray, PRUint32 aLength)
{
  printf("user_initialize\n");
  
  if (aLength > 0) {
    User_set_certificate(aArray, aLength);
  }
  
  sexpSimpleString *ss = (sexpSimpleString *)malloc(sizeof(sexpSimpleString));
  
  ss->length = ss->allocatedLength = strlen(pseudonym) + 1;
  ss->string = (u_char *)pseudonym;
  
  pseudonym_t *pseud = str_to_pseudonym(ss);
  
  user = user_initialize(pseud, NULL);
  
  return NS_OK;
}

/* void user_set_certificate ([array, size_is (aLength)] in PRUint8 aArray, in PRUint32 aLength); */
NS_IMETHODIMP Nymble::User_set_certificate(PRUint8 *aArray, PRUint32 aLength)
{
  X509 *certificate;

  const PRUint8 *p = aArray;

  certificate = d2i_X509(NULL, &p, aLength);

  EVP_PKEY *pkey;
  
  if (!(pkey = X509_get_pubkey(certificate))) {
  	// throw error!
  }
  
  RSA *pkey_rsa = EVP_PKEY_get1_RSA(pkey);
  
  fprintf(stderr, "%p", (void *)pkey_rsa);
  
  user->verify_key_n = pkey_rsa;
  
  return NS_OK;
}

/* string user_pseudonym (); */
NS_IMETHODIMP Nymble::User_pseudonym(char **_retval)
{
  printf("user_pseudonym\n");
  
  sexpSimpleString *ss = pseudonym_to_str(user_pseudonym(user), BASE64);
  *_retval = (char *)ss->string;
  
  return NS_OK;
}

/* void user_entry_initialize (in string server_id, in string credential, in unsigned long time_period); */
NS_IMETHODIMP Nymble::User_entry_initialize(const char *server_id, const char *credential, PRUint32 time_period)
{
  printf("user_entry_initialize\n");
  
  sexpSimpleString *ss = (sexpSimpleString *)malloc(sizeof(sexpSimpleString));
  
  ss->length = ss->allocatedLength = strlen(credential) + 1;
  ss->string = (u_char *)credential;
  
  credential_t *cred = str_to_credential(ss);
  
  u_char server_id_hash[32];

  hash(server_id_hash, (u_char *)server_id, strlen(server_id));

  user_entry_initialize(user, server_id_hash, cred, time_period);

  return NS_OK;
}

/* boolean user_entry_exists (in string server_id); */
NS_IMETHODIMP Nymble::User_entry_exists(const char *server_id, PRBool *_retval)
{
  printf("user_entry_exists\n");
  
  *_retval = user_entry_exists(user, (u_char *)server_id);
  
  return NS_OK;
}

/* string user_credential_get (in string server_id, in unsigned long time_period); */
NS_IMETHODIMP Nymble::User_credential_get(const char *server_id, PRUint32 time_period, char **_retval)
{
  printf("user_credential_get\n");
  
  u_char server_id_hash[32];
  
  hash(server_id_hash, (u_char *)server_id, strlen(server_id));
  
  printf("oink\n");
  ticket_t *ticket = user_credential_get(user, server_id_hash, time_period);
  printf("moo\n");
  sexpSimpleString *ss = ticket_to_str(ticket, ADVANCED);  
  printf("baa\n");
  *_retval = (char *)ss->string;
  
  return NS_OK;
}

NS_IMETHODIMP Nymble::User_blacklist_update(const char *server_id, const char *blacklist, PRUint32 link_window, PRUint32 time_period, PRBool *_retval)
{
  printf("user_blacklist_update\n");
  
  u_char server_id_hash[32];
    
  hash(server_id_hash, (u_char *)server_id, strlen(server_id));
  
  sexpSimpleString *ss = (sexpSimpleString *)malloc(sizeof(sexpSimpleString));
  
  ss->length = ss->allocatedLength = strlen(blacklist) + 1;
  ss->string = (u_char *)blacklist;
  
  blacklist_t *bl = str_to_blacklist(ss);
  
  *_retval = user_blacklist_update(user, server_id_hash, bl, link_window, time_period);
  
  return NS_OK;
}

NS_IMETHODIMP Nymble::User_blacklist_check(const char *server_id, PRBool *_retval)
{
  printf("user_blacklist_check\n");
  
  u_char server_id_hash[32];
  
  hash(server_id_hash, (u_char *)server_id, strlen(server_id));
  
  *_retval = user_blacklist_check(user, server_id_hash);
  
  return NS_OK;
}