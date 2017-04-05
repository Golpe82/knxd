/*
    EIBD eib bus access and management daemon
    Copyright (C) 2005-2011 Martin Koegler <mkoegler@auto.tuwien.ac.at>
 
    cEMI support for USB
    Copyright (C) 2013 Meik Felser <felser@cs.fau.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "cemi.h"
#include "emi.h"

unsigned int
CEMIDriver::maxPacketLen()
{
  return 50;
}

CEMIDriver::CEMIDriver (LowLevelIface* c, IniSectionPtr& s, LowLevelDriver *i) : EMI_Common(c,s,i)
{
  t->setAuxName("CEMI");
  sendLocal_done.set<CEMIDriver,&CEMIDriver::sendLocal_done_cb>(this);
}

CEMIDriver::~CEMIDriver()
{
}

void
CEMIDriver::sendLocal_done_cb(bool success)
{
  if (!success || sendLocal_done_next == N_bad)
    {
      errored();
      LowLevelFilter::stopped();
    }
  else if (sendLocal_done_next == N_down)
    LowLevelFilter::stopped();
  else if (sendLocal_done_next == N_up)
    LowLevelFilter::started();
  else if (sendLocal_done_next == N_reset)
    EMI_Common::started();
}

void CEMIDriver::cmdEnterMonitor() { errored(); stopped(); }
void CEMIDriver::cmdLeaveMonitor() { errored(); stopped(); }
void CEMIDriver::cmdOpen() { LowLevelDriver::started(); }
void CEMIDriver::cmdClose() { LowLevelDriver::stopped(); }

void CEMIDriver::started()
{
  sendReset();
}

void CEMIDriver::sendReset()
{
  sendLocal_done_next = N_reset;
  const uchar t1[] = { 0x10, 0x40, 0x40, 0x16 };
  send_Local (CArray (t1, sizeof (t1)), 2);
}

const uint8_t *
CEMIDriver::getIndTypes()
{ 
  static const uint8_t indTypes[] = { 0x2E, 0x29, 0x2B };
  return indTypes;
}   

