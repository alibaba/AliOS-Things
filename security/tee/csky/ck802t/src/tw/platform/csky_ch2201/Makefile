#
# Copyright (C) 2015-2017 Alibaba Group Holding Limited
#

TOP := ../..
CURRENT_BUILD := TW
include $(TOP)/tee.cfg
include $(TOP)/Make.rules

SRCS := $(wildcard *.c *.S )
OBJS := $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(SRCS)))
ENTRY := startup.o
OBJS := $(filter-out $(ENTRY), $(OBJS))
LIBS := plat.o

CFLAGS  += -I./inc
ASFLAGS += -I./inc

all: $(OBJS) $(LIBS)

$(LIBS): $(OBJS) $(ENTRY)
	$(NOECHO)$(LD) -r -o $(LIBS) $(OBJS)
	@echo "Finish building $(LIBS)..."

clean:
	@echo "Cleaning $(LIBS)..."
	$(NOECHO)rm -f $(OBJS) $(ENTRY) $(LIBS)

