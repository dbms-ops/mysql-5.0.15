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

#ifndef TRANSID_AI_HPP
#define TRANSID_AI_HPP

#include "SignalData.hpp"

class TransIdAI {
  /**
   * Sender(s)
   */
  friend class Dbtup;
  
  /**
   * Receiver(s)
   */
  friend class NdbTransaction;
  friend class Dbtc;
  friend class Dbutil;
  friend class Dblqh;
  friend class Suma;

  friend bool printTRANSID_AI(FILE *, const Uint32 *, Uint32, Uint16);
  
public:
  STATIC_CONST( HeaderLength = 3 );
  STATIC_CONST( DataLength = 22 );

  // Public methods
public:
 Uint32* getData() const;

public:
  Uint32 connectPtr;
  Uint32 transId[2];
  Uint32 attrData[DataLength];
};

inline
Uint32* TransIdAI::getData() const
{
  return (Uint32*)&attrData[0];
}

#endif
