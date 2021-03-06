#include "nsNymbleUser.h"

NS_IMPL_ISUPPORTS1(nsNymbleUser, nsINymbleUser)

nsNymbleUser::nsNymbleUser()
{
  this->user = new Nymble::User();
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

/* void readVerifyKey (in string verify_key_path); */
NS_IMETHODIMP nsNymbleUser::ReadVerifyKey(const char *verify_key_path)
{
  this->user->readVerifyKey((char*) verify_key_path);
  return NS_OK;
}

/* void setPseudonym (in string marshalled_pseudonym); */
NS_IMETHODIMP nsNymbleUser::SetPseudonym(const char *marshalled_pseudonym)
{
  Nymble::Pseudonym* pseudonym = new Nymble::Pseudonym();
  
  Nymble::Pseudonym::unmarshal((char*) marshalled_pseudonym, pseudonym);
  
  this->user->setPseudonym(pseudonym);
  
  delete pseudonym;
  
  return NS_OK;
}

/* string getPseudonym (); */
NS_IMETHODIMP nsNymbleUser::GetPseudonym(char **_retval)
{
  Nymble::Pseudonym* pseudonym = this->user->getPseudonym();
  
  if (pseudonym == NULL) {
    *_retval = NULL;
  } else {
    *_retval = (char*) malloc(pseudonym->marshal() + 1);
    pseudonym->marshal(*_retval);
  }
  
  return NS_OK;
}

/* string addBlacklist (in string marshalled_blacklist); */
NS_IMETHODIMP nsNymbleUser::AddBlacklist(const char *marshalled_blacklist, char **_retval)
{
  Nymble::Blacklist* blacklist = new Nymble::Blacklist();
  
  Nymble::Blacklist::unmarshal((char*) marshalled_blacklist, blacklist);
  
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
  Nymble::Credential* credential = new Nymble::Credential();
  
  Nymble::Credential::unmarshal((char*) marshalled_credential, credential);
  
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

  Nymble::Ticket* ticket;
  
  if (server_id_len == DIGEST_SIZE) {
    ticket = this->user->getTicket(server_id);
  }
  
  if (ticket == NULL) {
    *_retval = NULL;
  } else {
    *_retval = (char*) malloc(ticket->marshal() + 1);
    ticket->marshal(*_retval);
  }
  
  return NS_OK;
}
