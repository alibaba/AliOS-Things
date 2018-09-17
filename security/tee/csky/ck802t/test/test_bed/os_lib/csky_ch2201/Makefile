#
# Copyright (C) 2015-2017 Alibaba Group Holding Limited
#

TOP := ../../../../src/tw/
CURRENT_BUILD := NTW
include $(TOP)/tee.cfg
include $(TOP)/Make.rules

CFLAGS  += -I./driver/intr -I./driver/uart -I./inc -I../../inc
ASFLAGS += -I./driver/intr -I./driver/uart -I./inc -I../../inc

SRCS  := $(wildcard *.c *.S) $(wildcard ./driver/intr/*.c) $(wildcard ./driver/uart/*.c)
OBJS  := $(patsubst %.S,%.o, $(patsubst %.c,%.o,$(SRCS)))
ENTRY := startup.o
OBJS  := $(filter-out $(ENTRY), $(OBJS))
LIBS  := built-in.o

all: $(OBJS) $(LIBS)

$(LIBS): $(ENTRY) $(OBJS)
	$(LD) -r -o $(LIBS) $(OBJS)
	@ echo "Finish building $(LIBS)..."

clean:
	@ echo "Cleaning $(LIBS)..."
	rm -f $(OBJS) $(LIBS) $(ENTRY)

