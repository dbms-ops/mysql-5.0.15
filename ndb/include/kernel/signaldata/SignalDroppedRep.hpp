/* Copyright (C) 2003 MySQL AB

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
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef SIGNAL_DROPPED_HPP
#define SIGNAL_DROPPED_HPP

#include "SignalData.hpp"

class SignalDroppedRep {

  /**
   * Reciver(s)
   */
  friend class SimulatedBlock;

  /**
   * Sender (TransporterCallback.cpp)
   */
  friend void execute(void * , struct SignalHeader* const, Uint8, 
		      Uint32* const, struct LinearSectionPtr ptr[3]);

  friend bool printSIGNAL_DROPPED_REP(FILE *, const Uint32 *, Uint32, Uint16);  
public:
private:
  Uint32 originalGsn;
  Uint32 originalLength;
  Uint32 originalSectionCount;
  Uint32 originalData[1];
};

#endif
