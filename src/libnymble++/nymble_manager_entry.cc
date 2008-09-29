#include "nymble_manager_entry.h"

namespace Nymble {

NymbleManagerEntry::NymbleManagerEntry(u_char* sid, u_int cur_time_period)
{
  u_char mac_key_ns[DIGEST_SIZE];
  RAND_bytes(mac_key_ns, DIGEST_SIZE);

  this->setServerId(sid);
  this->setLastUpdated(cur_time_period);
  this->setMacKeyNS(mac_key_ns);
}

void NymbleManagerEntry::setServerId(u_char* sid)
{
  memcpy(this->sid, sid, DIGEST_SIZE);
}

u_char* NymbleManagerEntry::getServerId()
{
  return this->sid;
}

void NymbleManagerEntry::setLastUpdated(u_int bl_last_updated)
{
  this->bl_last_updated = bl_last_updated;
}

u_int NymbleManagerEntry::getLastUpdated()
{
  return this->bl_last_updated;
}

void NymbleManagerEntry::setMacKeyNS(u_char* mac_key_ns)
{
  memcpy(this->mac_key_ns, mac_key_ns, DIGEST_SIZE);
}

u_char* NymbleManagerEntry::getMacKeyNS()
{
  return this->mac_key_ns;
}

}; // namespace Nymble
