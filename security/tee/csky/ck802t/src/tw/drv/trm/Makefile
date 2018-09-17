#
# Copyright (C) 2015-2017 Alibaba Group Holding Limited
#

TOP := ../..
CURRENT_BUILD := TW
include $(TOP)/tee.cfg
include $(TOP)/Make.rules

TEST := Y
ID2_AES_KEY_BITS := 128

SRCS := tee_trm.c
LIBS := built-in.o

ifeq ($(TEST), Y)
SRCS += tee_trm_fake.c
ifeq ($(ID2_AES_KEY_BITS), 128)
CFLAGS += -DCONFIG_AES_128_BIT=1
else ifeq ($(ID2_AES_KEY_BITS), 192)
CFLAGS += -DCONFIG_AES_192_BIT=1
else ifeq ($(ID2_AES_KEY_BITS), 256)
CFLAGS += -DCONFIG_AES_256_BIT=1
endif
else
SRCS += tee_trm_id2.c
endif

OBJS := $(patsubst %.cxx,%.o,$(patsubst %.c,%.o,$(SRCS)))

#CFLAGS += -DTEE_TRM_DBG

ifeq ($(PLATFORM), csky_phobos)
CFLAGS  += -DTRM_FAKE_KEY_SEL0=1
else ifeq ($(PLATFORM), csky_ch2201)
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

