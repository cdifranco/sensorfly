# $Id$

#############################################################################
#
# Copyright 2002 - 2007
# Nanotron Technologies
#
# Author: S. Rohdemann and B. Jozefini
#
# BuildNumber = "BuildNumber : 2267";
#
# Description :
#    This file contains the rules for the nanoNET ATMega128 board portation
#
#############################################################################

#
# $Log$
#

#
# General Compiler/Linker options
#
CROSS_COMPILE = avr-
AS			= $(CROSS_COMPILE)as
LD			= $(CROSS_COMPILE)ld
CC			= $(CROSS_COMPILE)gcc
AR			= $(CROSS_COMPILE)ar

MAINEXE = main

ELF := $(MAINEXE).elf
HEX := $(MAINEXE).hex
#EXTCOFF := $(MAINEXE).cof
EEP := $(MAINEXE).eep
LSS := $(MAINEXE).lss
SYM := $(MAINEXE).sym
MAP := $(MAINEXE).map

#
# General Compiler/Linker options
#
INC	        := -I./h
OPTFLAGS	:= -Os -fomit-frame-pointer
DEBUGFLAGS	:= -gstabs
FLAGS		:= -Wall -Wstrict-prototypes \
				-Wno-trigraphs -fno-strict-aliasing -fno-common
CFLAGS		:= $(FLAGS) $(DEBUGFLAGS) $(INC)
AFLAGS		:= -D__ASSEMBLY__ $(CPPFLAGS)
LFLAGS		:= -Wl,-Map=$(MAP),--cref

LFLAGS		+= -Wl,-u,vfprintf -lprintf_flt

CCVERSION	:= $(shell $(CC) --verbose 2>&1 | tail -1)

#
# Special Cross-Compile Options
#
CFLAGS		+= -mmcu=atmega644 -funsigned-char -funsigned-bitfields \
				-fpack-struct -fshort-enums
EXTRA_LIBS	+= -lm

OBJCOPY := avr-objcopy
OBJDUMP := avr-objdump
NM := avr-nm
SIZE = avr-size

# Output format
FORMAT = ihex

# Convert ELF to COFF for use in debugging / simulating in AVR Studio.
COFFCONVERT := $(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000

OBJS = main.o \
	   app.o \
	   hwclock.o \
	   usart.o \
	   nnspi.o \
	   phy.o \
	   ntrxinit.o \
	   ntrxutil.o \
     ntrxiqpar.o \
	   ntrxranging.o \
		 packet.o	

all:	$(HEX) $(EXTCOFF) $(EEP) $(LSS) $(SYM) elfsize

$(ELF): $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS)  -o $@ $(OBJS) $(EXTRA_LIBS)

%.hex: %.elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.cof: %.elf
	$(COFFCONVERT) -O coff-ext-avr $< $@

%.eep: %.elf
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

%.lss: %.elf
	$(OBJDUMP) -h -S $< > $@

%.sym: %.elf
	$(NM) -n $< > $@

%.o:	%.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c $<

doc:
	doxygen doxy.cfg

doc_recursive:
	doxygen doxy.cfg

elfsize:
	@if [ -f $(ELF) ]; then $(SIZE) -A $(ELF); fi

clean:
	-rm -f $(OBJS) $(ELF) $(HEX) $(EXTCOFF) $(EEP) $(LSS) $(SYM) $(MAP)
	rm -fr html latex ppstags.xml mconfig.hlp

.PHONY: all clean elfsize doc doc_recursive
