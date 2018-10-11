SIMPLELINK_SDK_INSTALL_DIR ?= $(abspath ../../../../../../)
include $(SIMPLELINK_SDK_INSTALL_DIR)/imports.mak

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
AR = "$(IAR_ARMCOMPILER)/bin/iarchive"

CFLAGS = $(INCS) "-I$(IAR_ARMCOMPILER)/inc" --cpu Cortex-M3 --aeabi --guard_calls --endian=little --thumb --silent -e --use_unix_directory_separators --diag_suppress=Pa050,Go005 -On --debug

ARFLAGS =

vpath %.c ../../.. ../../../fonts

%.obj: %.c
	@ echo "# Building $@"
	@ $(CC) $(CFLAGS) $< -o $@

all: grlib.a

clean:
	@ $(RM) $(OBJS) grlib.a > $(DEVNULL) 2>&1

grlib.a: $(OBJS)
	@ echo "#"
	@ echo "# Archiving $@"
	@ echo "#"
	@ $(AR) $(ARFLAGS) $@ $^
