#ifndef __NS_NYMBLE_USER_H__
#define __NS_NYMBLE_USER_H__

#include "nsINymbleUser.h"
#include "nymble_user.h"
#include "nymble_pseudonym.h"
#include "nymble_blacklist.h"
#include "nymble_credential.h"
#include "nymble_ticket.h"
#include <string.h>

class nsNymbleUser : public nsINymbleUser
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSINYMBLEUSER

  nsNymbleUser();
  
private:
  ~nsNymbleUser();
  
  User* user;
};

#endif