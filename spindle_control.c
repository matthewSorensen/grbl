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

#define DEFAULT_FTM_MOD (49152 - 1)

void spindle_init()
{    
  SPINDLE_DIRECTION_CRL = STANDARD_OUTPUT;
  SPINDLE_DIRECTION_DDR |= (1<<SPINDLE_DIRECTION_BIT); // Configure as output pin

  #ifdef VARIABLE_SPINDLE
    SPINDLE_PWM_TIMER(MOD) = DEFAULT_FTM_MOD;
    SPINDLE_PWM_TIMER(SC)  = 0 | FTM_SC_CLKS(1) | FTM_SC_PS(0);
    SPINDLE_ENABLE_CTRL = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;  
  #else
    SPINDLE_ENABLE_CTRL = STANDARD_OUTPUT;
    SPINDLE_ENABLE_DDR |=  (1<<SPINDLE_ENABLE_BIT); // Configure as output pin
  #endif

  spindle_stop();
}


void spindle_stop()
{
  #ifdef VARIABLE_SPINDLE
    SPINDLE_PWM_VALUE = 0;
  #else
    SPINDLE_ENABLE_PORT(COR) = (1 << SPINDLE_ENABLE_BIT);
  #endif
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
    
    #ifdef VARIABLE_SPINDLE
      #define SPINDLE_RPM_RANGE (SPINDLE_MAX_RPM-SPINDLE_MIN_RPM)
      rpm -= SPINDLE_MIN_RPM;
      if ( rpm > SPINDLE_RPM_RANGE ) { rpm = SPINDLE_RPM_RANGE; }
      current= floor( rpm*(1023.0/SPINDLE_RPM_RANGE) + 0.5);
      SPINDLE_PWM_VALUE = ((uint32_t) current * (uint32_t)(FTM1_MOD + 1)) >> 10;
      SPINDLE_PWM_TIMER(SC) = 0 | FTM_SC_CLKS(1) | FTM_SC_PS(0);
    #else
      SPINDLE_ENABLE_PORT(SOR) = (1<<SPINDLE_ENABLE_BIT);
    #endif
  }
}
