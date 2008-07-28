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


/* void setTimePeriod (in PRUint32 time_period); */
NS_IMETHODIMP nsNymbleUser::SetTimePeriod(PRUint32 time_period)
{
    this->user->setTimePeriod(time_period);
    
    return NS_OK;
}

/* void setLinkWindow (in PRUint32 link_window); */
NS_IMETHODIMP nsNymbleUser::SetLinkWindow(PRUint32 link_window)
{
    this->user->setLinkWindow(link_window);
    
    return NS_OK;
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
  Pseudonym* pseudonym = new Pseudonym();
  
  Pseudonym::unmarshall((char*) marshalled_pseudonym, pseudonym);
  
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
    *_retval = (char*) malloc(pseudonym->marshall() + 1);
    pseudonym->marshall(*_retval);
  }
  
  return NS_OK;
}

/* string addBlacklist (in string marshalled_blacklist); */
NS_IMETHODIMP nsNymbleUser::AddBlacklist(const char *marshalled_blacklist, char **_retval)
{
  Blacklist* blacklist = new Blacklist();
  
  Blacklist::unmarshall((char*) marshalled_blacklist, blacklist);
  
  u_char* server_id = this->user->addBlacklist(blacklist);
  
  if (server_id == NULL) {
    *_retval = NULL;
  } else {
    *_retval = (char*) malloc(Nymble::hexencode(server_id, DIGEST_SIZE) + 1);
    Nymble::hexencode(server_id, DIGEST_SIZE, *_retval);
  }
  
  delete blacklist;
  
  return NS_OK;
}

/* boolean addCredential (in string marshalled_credential); */
NS_IMETHODIMP nsNymbleUser::AddCredential(const char *marshalled_credential, PRBool *_retval)
{
  Credential* credential = new Credential();
  
  Credential::unmarshall((char*) marshalled_credential, credential);
  
  *_retval = this->user->addCredential(credential);
  
  delete credential;
  
  return NS_OK;
}


/* string getTicket (in string serverId); */
NS_IMETHODIMP nsNymbleUser::GetTicket(const char *serverId, char **_retval)
{
  u_int server_id_len = Nymble::hexdecode((char*) serverId);
  u_char server_id[server_id_len];
  
  Nymble::hexdecode((char*) serverId, server_id);

  Ticket* ticket;
  
  if (server_id_len == DIGEST_SIZE) {
    ticket = this->user->getTicket(server_id);
  }
  
  if (ticket == NULL) {
    *_retval = NULL;
  } else {
    *_retval = (char*) malloc(ticket->marshall() + 1);
    ticket->marshall(*_retval);
  }
  
  return NS_OK;
}
