/*
  spindle_control.c - spindle control methods
  Part of Grbl

  Copyright (c) 2012-2014 Sungeun K. Jeon
  Copyright (c) 2009-2011 Simen Svale Skogsrud

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "system.h"
#include "spindle_control.h"
#include "protocol.h"
#include "gcode.h"


void spindle_init()
{    
  SPINDLE_ENABLE_CTRL = STANDARD_OUTPUT;
  SPINDLE_DIRECTION_CRL = STANDARD_OUTPUT;

  SPINDLE_ENABLE_DDR |=  (1<<SPINDLE_ENABLE_BIT); // Configure as output pin
  SPINDLE_DIRECTION_DDR |= (1<<SPINDLE_DIRECTION_BIT); // Configure as output pin
  spindle_stop();
}


void spindle_stop()
{
  SPINDLE_ENABLE_PORT(COR) = (1 << SPINDLE_ENABLE_BIT);
}


void spindle_run(uint8_t direction, float rpm) 
{
  if (sys.state == STATE_CHECK_MODE) { return; }
  
  // Empty planner buffer to ensure spindle is set when programmed.
  protocol_auto_cycle_start();  //temp fix for M3 lockup
  protocol_buffer_synchronize(); 
  
  // Halt or set spindle direction and rpm. 
  if (direction == SPINDLE_DISABLE) {
    spindle_stop();
  } else {
    if (direction == SPINDLE_ENABLE_CW) {
      SPINDLE_DIRECTION_PORT(COR) = (1<<SPINDLE_DIRECTION_BIT);
    } else {
      SPINDLE_DIRECTION_PORT(SOR) = (1<<SPINDLE_DIRECTION_BIT);
    }

    SPINDLE_ENABLE_PORT(SOR) = (1<<SPINDLE_ENABLE_BIT);
  }
}
