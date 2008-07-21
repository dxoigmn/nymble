#include "nymble_manager_entry.h"

NymbleManagerEntry::NymbleManagerEntry(u_char* server_id, u_int cur_time_period)
{
  u_char hmac_key_ns[DIGEST_SIZE];
  RAND_bytes(hmac_key_ns, DIGEST_SIZE);

  this->setServerId(server_id);
  this->setLastUpdated(cur_time_period);
  this->setHmacKeyNS(hmac_key_ns);
}

void NymbleManagerEntry::setServerId(u_char* server_id)
{
  memcpy(this->server_id, server_id, DIGEST_SIZE);
}

u_char* NymbleManagerEntry::getServerId()
{
  return this->server_id;
}

void NymbleManagerEntry::setLastUpdated(u_int bl_last_updated)
{
  this->bl_last_updated = bl_last_updated;
}

u_int NymbleManagerEntry::getLastUpdated()
{
  return this->bl_last_updated;
}

void NymbleManagerEntry::setHmacKeyNS(u_char* hmac_key_ns)
{
  memcpy(this->hmac_key_ns, hmac_key_ns, DIGEST_SIZE);
}

u_char* NymbleManagerEntry::getHmacKeyNS()
{
  return this->hmac_key_ns;
}

void NymbleManagerEntry::signCredential(Credential* credential)
{
  for (Tickets::iterator ticket = credential->begin(); ticket != credential->end(); ++ticket) {
    (*ticket)->hmac(this->hmac_key_ns, true);
  }
}