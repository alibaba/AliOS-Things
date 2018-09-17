#
# Copyright (C) 2015-2017 Alibaba Group Holding Limited
#

TOP := ../..
CURRENT_BUILD := TW
include $(TOP)/tee.cfg
include $(TOP)/Make.rules

SRCS := $(wildcard ./*.c)

ifeq ($(RSA_SUPPORT), Y)
SRCS += $(wildcard ./rsa/$(PLATFORM)/*.c)
CFLAGS += -I./rsa/$(PLATFORM)/
endif

ifeq ($(AES_SUPPORT), Y)
SRCS += $(wildcard ./aes/$(PLATFORM)/*.c)
CFLAGS += -I./aes/$(PLATFORM)/
endif

ifeq ($(SHA_SUPPORT), Y)
SRCS += $(wildcard ./sha/$(PLATFORM)/*.c)
CFLAGS += -I./sha/$(PLATFORM)/
endif

ifeq ($(TRNG_SUPPORT), Y)
SRCS += $(wildcard ./trng/$(PLATFORM)/*.c)
CFLAGS += -I./trng/$(PLATFORM)/
endif

OBJS := $(patsubst %.cxx,%.o,$(patsubst %.c,%.o,$(SRCS)))
LIBS := built-in.o

CFLAGS += -I$(TOP)/arch/c_sky/inc/ -I../inc

all: $(OBJS) $(LIBS)

$(LIBS): $(OBJS)
	$(NOECHO)$(LD) -r -o $(LIBS) $(OBJS)
	@echo "Finish building $(LIBS)..."

clean:
	@echo "Cleaning $(LIBS)..."
	$(NOECHO)rm -f $(OBJS) $(LIBS)

