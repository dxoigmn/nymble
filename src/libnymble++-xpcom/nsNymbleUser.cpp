#include "nsNymbleUser.h"

NS_IMPL_ISUPPORTS1(nsNymbleUser, nsINymbleUser)

nsNymbleUser::nsNymbleUser()
{
  this->user = new User();
}

nsNymbleUser::~nsNymbleUser()
{
  delete this->user;
}

/* void setVerifyKeyN ([array, size_is (len)] in PRUint8 bytes, in PRUint32 len); */
NS_IMETHODIMP nsNymbleUser::SetVerifyKeyN(PRUint8 *bytes, PRUint32 len)
{
  this->user->setVerifyKeyN(&bytes, len);
  
  return NS_OK;
}

/* void setPseudonym (in string marshalled_pseudonym); */
NS_IMETHODIMP nsNymbleUser::SetPseudonym(const char *marshalled_pseudonym)
{
  Pseudonym* pseudonym = Pseudonym::unmarshall((char*) marshalled_pseudonym);
  
  this->user->setPseudonym(pseudonym);
  
  delete pseudonym;
  
  return NS_OK;
}

/* string getPseudonym (); */
NS_IMETHODIMP nsNymbleUser::GetPseudonym(char **_retval)
{
  Pseudonym* pseudonym = this->user->getPseudonym();
  
  if (pseudonym == NULL) {
    *_retval = NULL;
  } else {
    u_int size = pseudonym->marshall();
    *_retval = (char*) malloc(size+1);
    *_retval[size] = 0;
    pseudonym->marshall(*_retval);
  }
  
  return NS_OK;
}

/* string addBlacklist (in string marshalled_blacklist); */
NS_IMETHODIMP nsNymbleUser::AddBlacklist(const char *marshalled_blacklist, char **_retval)
{
  Blacklist* blacklist = Blacklist::unmarshall((char*) marshalled_blacklist);
  
  u_char* server_id = this->user->addBlacklist(blacklist);
  
  if (server_id == NULL) {
    *_retval = NULL;
  } else {
    *_retval = (char*) malloc(DIGEST_SIZE*2+1);
    *_retval[DIGEST_SIZE*2] = 0;
    Nymble::hexencode(*_retval, server_id, DIGEST_SIZE);
  }
  
  delete blacklist;
  
  return NS_OK;
}

/* boolean addCredential (in string marshalled_credential); */
NS_IMETHODIMP nsNymbleUser::AddCredential(const char *marshalled_credential, PRBool *_retval)
{
  Credential* credential = Credential::unmarshall((char*) marshalled_credential);
  
  *_retval = this->user->addCredential(credential);
  
  delete credential;
  
  return NS_OK;
}


/* string getTicket (in string serverId); */
NS_IMETHODIMP nsNymbleUser::GetTicket(const char *serverId, char **_retval)
{
  u_char server_id[DIGEST_SIZE];
  
  Nymble::hexdecode(server_id, (char*) serverId, DIGEST_SIZE*2);
  Ticket* ticket = this->user->getTicket(server_id);
  
  if (ticket == NULL) {
    *_retval = NULL;
  } else {
    u_int size = ticket->marshall();
    *_retval = (char*) malloc(size+1);
    (*_retval)[size] = 0;
    ticket->marshall(*_retval);
  }
  
  return NS_OK;
}
