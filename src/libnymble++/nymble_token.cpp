#include "nymble_token.h"

namespace Nymble {

Token::Token()
{
  
}

Token::Token(Token* token)
{
  memcpy(this->seed, token->seed, DIGEST_SIZE);
  memcpy(this->nymble, token->nymble, DIGEST_SIZE);
}

Token::Token(Marshal::Token* token)
{
  memcpy(this->seed, token->seed().data(), DIGEST_SIZE);
  
  Ticket::computeNymble(this->seed, this->nymble);
}

Token::Token(u_char* seed)
{
  memcpy(this->seed, seed, DIGEST_SIZE);
  Ticket::computeNymble(this->seed, this->nymble);
}

void Token::evolve(u_int time_period_delta)
{
  Ticket::evolveSeed(this->seed, time_period_delta, this->seed);
  Ticket::computeNymble(this->seed, this->nymble);
}

u_char* Token::getNymble()
{
  return this->nymble;
}

u_int Token::marshal(u_char* out, u_int size)
{
  Marshal::Token token;
  
  token.set_seed(this->seed, DIGEST_SIZE);
  
  if (out != NULL) {
    token.SerializeToArray(out, size);
  }
  
  return token.ByteSize();
}

Token* Token::unmarshal(u_char* bytes, u_int size)
{
  Marshal::Token token;
  
  if (token.ParseFromArray(bytes, size)) {
    return new Token(&token);
  }
  
  return NULL;
}

}; // namespace Nymble
