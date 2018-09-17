#
# Copyright (C) 2015-2017 Alibaba Group Holding Limited
#

TOP := ../../
CURRENT_BUILD := TW
include $(TOP)/tee.cfg
include $(TOP)/Make.rules

SRCS :=

SRCS += $(wildcard $(PLATFORM)/*.c)

OBJS := $(patsubst %.cxx,%.o,$(patsubst %.c,%.o,$(SRCS)))
LIBS := built-in.o

all: $(OBJS) $(LIBS)

$(LIBS): $(OBJS)
	$(NOECHO)$(LD) -r -o $(LIBS) $(OBJS)
	@echo "Finish building $(LIBS)..."

clean:
	@echo "Cleaning $(LIBS) $(OBJS)..."
	$(NOECHO)rm -f $(OBJS) $(LIBS)

