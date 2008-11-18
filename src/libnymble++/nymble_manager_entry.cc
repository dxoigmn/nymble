#include "nymble_manager_entry.h"

namespace Nymble {

NymbleManagerEntry::NymbleManagerEntry(std::string sid, u_int time_last_updated)
{
  this->sid = sid;
  this->time_last_updated = time_last_updated;
  random_bytes(DIGEST_SIZE, &this->daisy_l);
  random_bytes(DIGEST_SIZE, &this->mac_key_ns);
}

void NymbleManagerEntry::setServerId(std::string sid)
{
  this->sid = sid;
}

std::string NymbleManagerEntry::getServerId()
{
  return this->sid;
}


void NymbleManagerEntry::setMacKeyNS(std::string mac_key_ns)
{
  this->mac_key_ns = mac_key_ns;
}

std::string NymbleManagerEntry::getMacKeyNS()
{
  return this->mac_key_ns;
}


void NymbleManagerEntry::setDaisyL(std::string daisy_l)
{
  this->daisy_l = daisy_l;
}

std::string NymbleManagerEntry::getDaisyL()
{
  return this->daisy_l;
}

void NymbleManagerEntry::setTimeLastUpdated(u_int time_last_updated)
{
  this->time_last_updated = time_last_updated;
}

u_int NymbleManagerEntry::getTimeLastUpdated()
{
  return this->time_last_updated;
}


}; // namespace Nymble
