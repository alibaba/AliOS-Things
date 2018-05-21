#
# Copyright (C) 2015 The YunOS Project. All rights reserved.
#

TOP := ../..
CURRENT_BUILD := TW
include $(TOP)/tee.cfg
include $(TOP)/Make.rules

SRCS :=
ifeq ($(PLATFORM), csky_phobos)
SRCS += ./tee_asc_phobos.c
else
ifeq ($(PLATFORM), csky_ch2201)
SRCS += ./tee_asc_ch2201.c
endif
endif

OBJS := $(patsubst %.cxx,%.o,$(patsubst %.c,%.o,$(SRCS)))
LIBS := built-in.o

#CFLAGS += -DTEE_ASC_DBG

all: $(OBJS) $(LIBS)

$(LIBS): $(OBJS)
	$(NOECHO)$(LD) -r -o $(LIBS) $(OBJS)
	@echo "Finish building $(LIBS)..."

clean:
	@echo "Cleaning $(LIBS)..."
	$(NOECHO)rm -f $(OBJS) $(LIBS)

