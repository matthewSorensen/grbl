#  Part of Grbl
#
#  The MIT License (MIT)
#
#  Grbl(tm) - Embedded CNC g-code interpreter and motion-controller
#  Copyright (c) 2009-2011 Simen Svale Skogsrud
#  Copyright (c) 2012 Sungeun K. Jeon
#  Copyright (c) 2014 Matthew D. Sorensen
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#  THE SOFTWARE.

all: grbl.hex

# User configurable firmware setting:
CLOCK = 48000000
CPU =MK20DX256
# For 3.1. For 3.0, MK20DX128

TEENSY_PATH = /home/matthew/498/teensy-toolchain
COMPILER = $(TEENSY_PATH)/hardware/tools/arm-none-eabi/bin

CFLAGS = -Wall -g -Os -mcpu=cortex-m4 -mthumb -nostdlib -MMD -DF_CPU=$(CLOCK) -DUSB_SERIAL -Ivendor/ -D__$(CPU)__
LDFLAGS = -Os -Wl,--gc-sections -mcpu=cortex-m4 -mthumb -Tvendor/mk20dx256.ld
LIBS = -lm
CC = $(COMPILER)/arm-none-eabi-gcc
CXX = $(COMPILER)/arm-none-eabi-g++
OBJCOPY = $(COMPILER)/arm-none-eabi-objcopy
SIZE = $(COMPILER)/arm-none-eabi-size


OBJECTS    = main.o motion_control.o gcode.o spindle_control.o coolant_control.o \
             protocol.o stepper.o settings.o planner.o nuts_bolts.o limits.o \
             print.o report.o


VENDOR_C = $(wildcard vendor/*.c)
VENDOR_OBJECTS = $(patsubst %.c,%.o,$(VENDOR_C))

%.hex: %.elf
	$(SIZE) $<
	$(OBJCOPY) -O ihex -R .eeprom $< $@


grbl.elf: $(OBJECTS) $(VENDOR_OBJECTS) 
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(VENDOR_OBJECTS) $(LIBS) 

-include $(OBJS:.o=.d)


clean:
	rm -f *.o *.d *.elf *.hex vendor/*.o

