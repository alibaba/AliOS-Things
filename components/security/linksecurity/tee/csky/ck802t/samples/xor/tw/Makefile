#
# Copyright (C) 2015-2017 Alibaba Group Holding Limited
#

CROSS_COMPILE 	?= csky-abiv2-elf-

CC              := $(CROSS_COMPILE)gcc
LD              := $(CROSS_COMPILE)ld
CFLAGS          += -I. -I$(XOR_INC) -I$(TEE_INC) -mcpu=ck802t -g3 -Wall -c -Wa,-melrw

PWD             := $(shell pwd)
TW_DIR          = $(PWD)/../../../src/tw
XOR_INC         = $(PWD)/../inc
TEE_INC         = $(TW_DIR)/inc

CFLAGS          += -I../../../src/common/inc -DCONFIG_DBG

SRCS := $(wildcard *.c *.cxx)
OBJS := $(patsubst %.cxx,%.o,$(patsubst %.c,%.o,$(SRCS)))

built-in.o: $(OBJS)
	$(LD) -r $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $*.o

clean:
	rm -f *.o built-in.*
