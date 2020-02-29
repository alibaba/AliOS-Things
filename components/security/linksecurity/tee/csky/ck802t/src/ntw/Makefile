#
# Copyright (C) 2015-2017 Alibaba Group Holding Limited
#
TW_TOP := ../tw
CURRENT_BUILD := NTW
include $(TW_TOP)/tee.cfg
include $(TW_TOP)/Make.rules

CFLAGS += -I./inc -I./osa/inc

SRCS := $(wildcard ./osa/*.c) $(wildcard ./*.c)
OBJS := $(patsubst %.c,%.ntw.o,$(patsubst %.S,%.ntw.o,$(SRCS)))

DRV := tee_drv.o
DEP_OBJS := ./drv/drv.sdk.o ./lpm/ntw.lpmapi.o

ifeq ($(CONFIG_TST_STUB), Y)
DEP_OBJS += ./stub/tst_stub/tststub.o
endif

.PHONY: all clean

all: $(OBJS) $(DRV)

$(DRV): $(OBJS)
	$(NOECHO)$(LD) -r -o $(DRV) $(OBJS) $(DEP_OBJS)
	@echo "Finish building $@..."

clean:
	@echo "Cleaning up ..."
	$(NOECHO)rm -f $(OBJS) $(DRV)

