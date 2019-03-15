SIMPLELINK_SDK_INSTALL_DIR ?= $(abspath ../../../../../../)
include $(SIMPLELINK_SDK_INSTALL_DIR)/imports.mak

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
AR = "$(CCS_ARMCOMPILER)/bin/armar"

CFLAGS = $(INCS) "-I$(CCS_ARMCOMPILER)/include" --endian=little -mv7M3 --float_support=vfplib -qq --gen_func_subsections -g --symdebug:dwarf

ARFLAGS = -r

vpath %.c ../../.. ../../../fonts

%.obj: %.c
	@ echo "# Building $@"
	@ $(CC) $(CFLAGS) $< --output_file=$@

all: grlib.a

clean:
	@ $(RM) $(OBJS) grlib.a > $(DEVNULL) 2>&1

grlib.a: $(OBJS)
	@ echo "#"
	@ echo "# Archiving $@"
	@ echo "#"
	@ $(AR) $(ARFLAGS) $@ $^
