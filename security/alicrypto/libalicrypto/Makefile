#
# Copyright (C) 2017 The YunOS Project. All rights reserved.
#

TOP := ../

#TOOLCHAIN_PRE := arm-none-eabi-
#TOOLCHAIN_PRE := arm-linux-gnueabihf-
TOOLCHAIN_PRE := 
CC    			= $(TOOLCHAIN_PRE)gcc
LD              = $(TOOLCHAIN_PRE)ld
AR              = $(TOOLCHAIN_PRE)ar
RANLIB          = $(TOOLCHAIN_PRE)ranlib

LOCAL_DIR := .

CRYPT_TEST := N
CRYPT_TYPE := MBED

LIB := $(TOP)/mbedtls/library/libmbedcrypto.a

CFLAGS = -Wall -g -O2 -I$(LOCAL_DIR)/mbed/inc -I$(LOCAL_DIR)/sw \
	-I$(LOCAL_DIR)/inc -I$(TOP)/mbedtls/include/mbedtls/ \
	-I$(TOP)/../yunos_iot/aos/include

ifeq ($(m32),1)
CFLAGS += -m32
endif

CFLAGS += -Wformat
CFLAGS += -DCONFIG_CRYPT_MBED=1 -DCONFIG_DBG_CRYPT=1
#-DCONFIG_NO_ALIOS=1

ifeq ($(gcov),1)
CFLAGS  += -fprofile-arcs -ftest-coverage
LDFLAGS += --coverage
endif

ifeq ($(CRYPT_TYPE), MBED)
SRCS +=  \
           $(LOCAL_DIR)/mbed/cipher/aes.c \
           $(LOCAL_DIR)/mbed/hash/hash.c \
           $(LOCAL_DIR)/mbed/asym/rsa.c \
           $(LOCAL_DIR)/mbed/mac/hmac.c \

endif

ifeq ($(CRYPT_TEST), Y)
#TEST_SRCS += $(LOCAL_DIR)/mbed/test/mbed_rsa_test.c
TEST_SRCS += \
           $(LOCAL_DIR)/test/ali_crypto_test.c      \
	       $(LOCAL_DIR)/test/ali_crypto_test_comm.c \
	       $(LOCAL_DIR)/test/ali_crypto_test_hash.c \
	       $(LOCAL_DIR)/test/ali_crypto_test_rand.c \
	       $(LOCAL_DIR)/test/ali_crypto_test_aes.c  \
	       $(LOCAL_DIR)/test/ali_crypto_test_rsa.c  \
	       $(LOCAL_DIR)/test/ali_crypto_test_hmac.c \

CFLAGS += -I$(LOCAL_DIR)/test/inc
OUT_E := ali_crypto_test
endif

SRCS += $(LOCAL_DIR)/sw/ali_crypto_rand.c
CFLAGS += -I$(LOCAL_DIR)/sw/inc

SRCS += $(LOCAL_DIR)/ali_crypto.c
OBJS := $(patsubst %.cxx,%.o,$(patsubst %.c,%.o,$(SRCS)))
TEST_OBJS := $(patsubst %.cxx,%.o,$(patsubst %.c,%.o,$(TEST_SRCS)))
OUT := libalicrypto.a

all: $(OUT_E) $(OUT) $(OBJS) $(TEST_OBJS)
$(OUT): $(OBJS) $(LIB)
	$(AR) rc $(OUT) $(OBJS)
	$(RANLIB) $(OUT)
$(OUT_E): $(OBJS) $(TEST_OBJS) $(LIB)
	$(CC) $(LDFLAGS) $(CFLAGS) $(LIB) $^ -o $@.elf
	$(CC) $(LDFLAGS) $(CFLAGS) $(LIB) $^ -o $@
%.o: %.c
	echo $(CC) $(CFLAGS) $<
	$(CC) -c $(CFLAGS) $< -o $*.o

clean:
	rm -f $(OBJS) $(OUT_E) $(OUT_E).elf $(OUT) $(TEST_OBJS)
