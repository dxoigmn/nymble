#include "nsIGenericFactory.h"
#include "nsNymbleUser.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(nsNymbleUser)

static nsModuleComponentInfo components[] =
{
  {
     "", 
     NS_INYMBLEUSER_IID,
     "@nymble.cs.dartmouth.edu/nymble/NymbleUser;1",
     nsNymbleUserConstructor,
  }
};

NS_IMPL_NSGETMODULE("NymbleUserModule", components)