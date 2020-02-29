#
# Copyright (C) 2015-2017 Alibaba Group Holding Limited
#

## Use Make.rulse in tee/tw folder
TEST_BED_TOP := ../../../test_bed/
TW_TOP := ../../../../src/tw/
CURRENT_BUILD := NTW
include $(TW_TOP)/tee.cfg
include $(TW_TOP)/Make.rules

CFLAGS  += -I../inc/ -I$(TEST_BED_TOP)/inc/ -I$(TEST_BED_TOP)/drv/inc/

SRCS := $(wildcard *.c *.cxx)
OBJS := $(patsubst %.cxx,%.ntw.o,$(patsubst %.c,%.ntw.o,$(SRCS)))

.PHONY: all clean

all: built-in.o

built-in.o: $(OBJS)
	$(LD) -r $^ -o $@

clean:
	rm -f *.o built-in.*
