#
# Copyright (C) 2015-2017 Alibaba Group Holding Limited
#

TOP := ..
CURRENT_BUILD := TW
include $(TOP)/tee.cfg
include $(TOP)/Make.rules

SRCS := $(wildcard *.c *.cxx)
SRCS  += $(wildcard ./lpm/*.c ./lpm/*.S)
OBJS := $(patsubst %.S,%.o,$(patsubst %.cxx,%.o,$(patsubst %.c,%.o,$(SRCS))))
LIBS := mon_emu.o

CFLAGS  += -I$(TOP)/arch/c_sky/inc
CFLAGS += -DLPM_BTM_EMU
ASFLAGS += -DLPM_BTM_EMU
all: $(OBJS) $(LIBS)

$(LIBS): $(OBJS)
	$(NOECHO)$(LD) -r -o $(LIBS) $(OBJS)
	@echo "Finish building $(LIBS)..."

clean:
	@echo "Cleaning $(LIBS)..."
	$(NOECHO)rm -f $(OBJS) $(LIBS)

