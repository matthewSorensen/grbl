/*
  spindle_control.c - spindle control methods
  Part of Grbl

  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Copyright (c) 2012 Sungeun K. Jeon

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

#include "settings.h"
#include "spindle_control.h"
#include "planner.h"
#include <pin_config.h>

static uint32_t current_direction;

void spindle_init()
{
  current_direction = 0;
  
  SPINDLE_ENABLE_CTRL = STANDARD_GPIO;
  SPINDLE_DIRECTION_CRL = STANDARD_GPIO;

  SPINDLE_DDR |=  SPINDLE_ENABLE_BIT | SPINDLE_DIRECTION_BIT;
  spindle_stop();
}

void spindle_stop()
{
  SPINDLE_PORT(COR) = SPINDLE_ENABLE_BIT;
}

void spindle_run(int32_t direction)
{
  if (direction != current_direction) {
    plan_synchronize();
    if (direction) {
      if(direction > 0) {
	SPINDLE_PORT(COR) = SPINDLE_DIRECTION_BIT;
      } else {
	SPINDLE_PORT(SOR) = SPINDLE_DIRECTION_BIT;
      }
      SPINDLE_PORT(SOR) = SPINDLE_ENABLE_BIT;
    } else {
      spindle_stop();     
    }
    current_direction = direction;
  }
}
