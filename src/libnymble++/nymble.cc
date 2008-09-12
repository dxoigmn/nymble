#include "nymble.h"

namespace Nymble {

Nymble::Nymble()
{
  setLinkWindow(0);
  setTimePeriod(1);
}

void Nymble::setLinkWindow(u_int link_window)
{
  this->cur_link_window = link_window;
}

u_int Nymble::getLinkWindow()
{
  return this->cur_link_window;
}

void Nymble::setTimePeriod(u_int time_period)
{
  this->cur_time_period = time_period;
}

u_int Nymble::getTimePeriod()
{
  return this->cur_time_period;
}

}; // namespace Nymble
