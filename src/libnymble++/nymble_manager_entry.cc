#include "nymble_manager_entry.h"

namespace Nymble {

NymbleManagerEntry::NymbleManagerEntry(std::string sid, u_int cur_time_period)
{
  this->sid = sid;
  this->bl_last_updated = cur_time_period;
  random_bytes(this->mac_key_ns);
}

void NymbleManagerEntry::setServerId(std::string sid)
{
  this->sid = sid;
}

std::string NymbleManagerEntry::getServerId()
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

void NymbleManagerEntry::setMacKeyNS(std::string mac_key_ns)
{
  this->mac_key_ns = mac_key_ns;
}

std::string NymbleManagerEntry::getMacKeyNS()
{
  return this->mac_key_ns;
}

}; // namespace Nymble
