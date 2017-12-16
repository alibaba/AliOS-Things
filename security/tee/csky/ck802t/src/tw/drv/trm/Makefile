#
# Copyright (C) 2015 The YunOS Project. All rights reserved.
#

TOP := ../..
CURRENT_BUILD := TW
include $(TOP)/tee.cfg
include $(TOP)/Make.rules

SRCS := $(wildcard *.c *.cxx)
OBJS := $(patsubst %.cxx,%.o,$(patsubst %.c,%.o,$(SRCS)))
LIBS := built-in.o

#CFLAGS += -DTRM_FAKE_KEY
#CFLAGS += -DTEE_TRM_DBG

ifeq ($(HOBBIT_NBIOT_256), Y)
CFLAGS  += -DTRM_FAKE_KEY_SEL0=1
else ifeq ($(PHOBOS_GENERAL), Y)
CFLAGS  += -DTRM_FAKE_KEY_SEL0=1
else
CFLAGS  += -DTRM_FAKE_KEY_SEL1=1
endif

all: $(OBJS) $(LIBS)

$(LIBS): $(OBJS)
	$(NOECHO)$(LD) -r -o $(LIBS) $(OBJS)
	@ echo "Finish building $(LIBS)..."

clean:
	@ echo "Cleaning $(LIBS)..."
	$(NOECHO)rm -f $(OBJS) $(LIBS)

