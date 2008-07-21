#include "nymble_token_wrap.h"

void rb_token_delete(Token* token)
{
  delete token;
}