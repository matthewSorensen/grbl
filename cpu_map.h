/*
  cpu_map.h - CPU and pin mapping configuration file
  Part of Grbl v0.9

  Copyright (c) 2012-2014 Sungeun K. Jeon

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

/* The cpu_map.h file serves as a central pin mapping settings file for different processor
   types, i.e. AVR 328p or AVR Mega 2560. Grbl officially supports the Arduino Uno, but the 
   other supplied pin mappings are supplied by users, so your results may vary. */

// NOTE: This is still a work in progress. We are still centralizing the configurations to
// this file, so your success may vary for other CPUs.

// K20 NOTE: Compatibility with the AVR IO model is tricky, especially since we'd like to
// retain advanced features such as atomic output operations. As such, the existence of a
// CPU map file in the k20 branch shouldn't imply support for various CPUs, but rather
// is a ploy to make the eventual merger with master (...in 1.0 or latter...) easier.

#ifndef cpu_map_h
#define cpu_map_h

//----------------------------------------------------------------------------------------
#ifdef CPU_MAP_MK20

  // Because we're on a 32-bit system, interfacing with the IO ports requires a full
  // word for all bitmasks.
  typedef uint32_t  io_mask_t;

  // Define step pulse output pins. NOTE: All step bit pins must be on the same port.
  #define STEP_DDR        GPIOC_PDDR
  #define STEP_PORT(reg)  GPIOC_P##reg

  #define X_STEP_BIT      3  // Teensy 3.x pin 9
  #define Y_STEP_BIT      4  // ..             10
  #define Z_STEP_BIT      6  // ..             11
  #define STEP_MASK       ((1<<X_STEP_BIT)|(1<<Y_STEP_BIT)|(1<<Z_STEP_BIT)) // All step bits

  #define STEP_X_CTRL PORTC_PCR3   
  #define STEP_Y_CTRL PORTC_PCR4
  #define STEP_Z_CTRL PORTC_PCR6

  // Define step direction output pins. NOTE: All direction pins must be on the same port.
  #define DIRECTION_DDR     GPIOC_PDDR
  #define DIRECTION_PORT(reg)    GPIOC_P##reg
  #define X_DIRECTION_BIT   7  // Teensy 3.x pin 12
  #define Y_DIRECTION_BIT   5  // ..             13
  #define Z_DIRECTION_BIT   0  // ..             15
  #define DIRECTION_MASK    ((1<<X_DIRECTION_BIT)|(1<<Y_DIRECTION_BIT)|(1<<Z_DIRECTION_BIT)) // All direction bits

  #define DIR_X_CTRL PORTC_PCR7   
  #define DIR_Y_CTRL PORTC_PCR5
  #define DIR_Z_CTRL PORTC_PCR0

  // Define stepper driver enable/disable output pin.
  #define STEPPERS_DISABLE_DDR    GPIOC_PDDR
  #define STEPPERS_DISABLE_PORT(reg)   GPIOC_P##reg
  #define STEPPERS_DISABLE_BIT    2  // Teensy 3.x pin 22
  #define STEPPERS_DISABLE_MASK   (1<<STEPPERS_DISABLE_BIT)

  #define STEPPERS_DISABLE_CTRL      PORTC_PCR1

  // Define homing/hard limit switch input pins and limit interrupt vectors. 
  // NOTE: All limit bit pins must be on the same port, but not on a port with other input pins (pinout).
  #define LIMIT_DDR        GPIOB_PDDR
  #define LIMIT_PORT(reg)  GPIOB_P##reg
  #define X_LIMIT_BIT      0  // Teensy 3.x pin 16
  #define Y_LIMIT_BIT      1  // ..             17
  #define Z_LIMIT_BIT      3  // ..             18
 
  #define LIMIT_MASK       ((1<<X_LIMIT_BIT)|(1<<Y_LIMIT_BIT)|(1<<Z_LIMIT_BIT)) // All limit bits
  #define LIMIT_X_CTRL PORTB_PCR0
  #define LIMIT_Y_CTRL PORTB_PCR1
  #define LIMIT_Z_CTRL PORTB_PCR3
 

  // Define spindle enable and spindle direction output pins.
  // Teensy 3.x pin 3
  #define SPINDLE_ENABLE_DDR    GPIOA_PDDR
  #define SPINDLE_ENABLE_PORT(reg)   GPIOA_P##reg
  #define SPINDLE_ENABLE_BIT    12

  #ifdef VARIABLE_SPINDLE
    #define SPINDLE_PWM_TIMER(reg) FTM1_##reg
    #define SPINDLE_PWM_VALUE       FTM1_C0V
  #endif

  // Teensy 3.x pin 4
  #define SPINDLE_DIRECTION_DDR   GPIOA_PDDR
  #define SPINDLE_DIRECTION_PORT(reg)  GPIOA_P##reg 
  #define SPINDLE_DIRECTION_BIT   13

  #define SPINDLE_ENABLE_CTRL PORTA_PCR12
  #define SPINDLE_DIRECTION_CRL PORTA_PCR13

  // Define flood and mist coolant enable output pins.
  // Teensy 3.x pin 5
  #define COOLANT_FLOOD_DDR   GPIOD_PDDR
  #define COOLANT_FLOOD_PORT(reg)  GPIOD_P##reg
  #define COOLANT_FLOOD_BIT   7
  #define COOLANT_FLOOD_CTRL PORTD_PCR7
  #ifdef ENABLE_M7 // Mist coolant disabled by default. See config.h to enable/disable.
    // Teensy 3.x pin 6
    #define COOLANT_MIST_DDR   GPIOD_PDDR
    #define COOLANT_MIST_PORT(reg)  GPIOD_P##reg
    #define COOLANT_MIST_BIT   4
    #define COOLANT_MIST_CTRL  PORTD_PCR4
  #endif  

  // Define user-control pinouts (cycle start, reset, feed hold) input pins.
  // NOTE: All pinouts pins must be on the same port.

  #define PINOUT_DDR       GPIOB_PDDR
  #define PINOUT_PORT(reg) GPIOB_P##reg
  #define PIN_RESET        16
  #define PIN_FEED_HOLD    17
  #define PIN_CYCLE_START  19
  #define PINOUT_MASK ((1<<PIN_RESET)|(1<<PIN_FEED_HOLD)|(1<<PIN_CYCLE_START))

  #define PIN_RESET_CTRL PORTB_PCR16
  #define PIN_FEED_CTRL  PORTB_PCR17
  #define PIN_CYCLE_CTRL PORTB_PCR19

  // Note the lack of variable spindle. Fix that.
  // Also, no software debouncing on inputs.
  // Define probe switch input pin.
  #define PROBE_DDR       GPIOA_PDDR
  #define PROBE_PORT(reg) GPIOA_P##reg
  #define PROBE_BIT       5 // Teensy 3.x pin 24
  #define PROBE_MASK      (1<<PROBE_BIT)
  #define PROBE_CTRL      PORTA_PCR5

#endif

//----------------------------------------------------------------------------------------

/* 
#ifdef CPU_MAP_CUSTOM_PROC
  // For a custom pin map or different processor, copy and paste one of the default cpu map
  // settings above and modify it to your needs. Then, make sure the defined name is also
  // changed in the config.h file.
#endif
*/

#endif
