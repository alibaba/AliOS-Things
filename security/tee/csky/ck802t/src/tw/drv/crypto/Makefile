#
# Copyright (C) 2015 The YunOS Project. All rights reserved.
#

TOP := ../..
CURRENT_BUILD := TW
include $(TOP)/tee.cfg
include $(TOP)/Make.rules

SRCS := $(wildcard ./*.c)

ifeq ($(RSA_SUPPORT), Y)
SRCS += $(wildcard ./rsa/*.c)
CFLAGS += -I./rsa/
endif

ifeq ($(AES_SUPPORT), Y)
SRCS += $(wildcard ./aes/*.c)
CFLAGS += -I./aes/
endif

ifeq ($(SHA_SUPPORT), Y)
ifeq ($(PLATFORM), csky_phobos)
SRCS += ./sha/sha_drv_phobos.c
else
SRCS += ./sha/sha_drv.c
endif
CFLAGS += -I./sha/
endif
ifeq ($(TRNG_SUPPORT), Y)
SRCS += $(wildcard ./trng/*.c)
CFLAGS += -I./trng/
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

