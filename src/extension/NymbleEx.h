#include "INymble.h"

extern "C" {
  #include "user.h"
}

class Nymble : public INymble
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_INYMBLE

  Nymble();

private:
  ~Nymble();

protected:
  user_t *user;
  
  /* additional members */
};

