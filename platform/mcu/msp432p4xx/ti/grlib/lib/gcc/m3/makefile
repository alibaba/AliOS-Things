SIMPLELINK_SDK_INSTALL_DIR ?= $(abspath ../../../../../../)
include $(SIMPLELINK_SDK_INSTALL_DIR)/imports.mak

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
AR = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc-ar"

CFLAGS = $(INCS) -mcpu=cortex-m3 -march=armv7-m -mthumb -mfloat-abi=soft -Wall -ffunction-sections -fdata-sections -c -g -gstrict-dwarf

ARFLAGS = -r

vpath %.c ../../.. ../../../fonts

%.obj: %.c
	@ echo "# Building $@"
	@ $(CC) $(CFLAGS) $< -c -o $@

all: grlib.a

clean:
	@ $(RM) $(OBJS) grlib.a > $(DEVNULL) 2>&1

grlib.a: $(OBJS)
	@ echo "#"
	@ echo "# Archiving $@"
	@ echo "#"
	@ $(AR) $(ARFLAGS) $@ $^
