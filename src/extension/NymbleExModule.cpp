#include "nsIGenericFactory.h"
#include "NymbleEx.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(Nymble)

static nsModuleComponentInfo components[] =
{
  {
     "", 
     INYMBLE_IID,
     "@nymble.cs.dartmouth.edu/nymble/Nymble;1",
     NymbleConstructor,
  }
};

NS_IMPL_NSGETMODULE("NymbleModule", components)