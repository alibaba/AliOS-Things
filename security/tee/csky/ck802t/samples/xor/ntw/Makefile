#
# Copyright (C) 2015 The YunOS Project. All rights reserved.
#

## Use Make.rulse in tee/tw folder
NTW_DRV_TOP := ../../../src/ntw/
TW_TOP := ../../../src/tw/
CURRENT_BUILD := NTW
include $(TW_TOP)/tee.cfg
include $(TW_TOP)/Make.rules

SRCS := $(wildcard *.c *.cxx)
OBJS := $(patsubst %.cxx,%.o,$(patsubst %.c,%.o,$(SRCS)))
CFLAGS += -I../inc -I$(NTW_DRV_TOP)/inc -I$(NTW_DRV_TOP)/drv/inc
LIBS := xor_stub.o

all: $(OBJS) $(LIBS)

$(LIBS): $(OBJS)
	$(LD) -r -o $(LIBS) $(OBJS)
	@ echo "Finish building $(LIBS)..."

clean:
	@ echo "Cleaning $(LIBS)..."
	rm -f $(OBJS) $(LIBS)

