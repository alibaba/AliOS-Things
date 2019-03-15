
# Also see "../../config/config.h"

#TOOLCHAIN_PRE := arm-none-eabi-
#TOOLCHAIN_PRE := arm-linux-gnueabihf-
TOOLCHAIN_PRE := 
CC    			= $(TOOLCHAIN_PRE)gcc
LD              = $(TOOLCHAIN_PRE)ld
AR              = $(TOOLCHAIN_PRE)ar

CFLAGS	?= -O2
WARNING_CFLAGS ?=  -Wall -W -Wdeclaration-after-statement
LDFLAGS ?=

ifeq ($(m32),1)
CFLAGS += -m32
endif

LOCAL_CFLAGS = $(WARNING_CFLAGS)  -D_FILE_OFFSET_BITS=64
LOCAL_CFLAGS += -I./inc  -I../../config

#-DCONFIG_NO_ALIOS=1
LOCAL_LDFLAGS =

ifdef DEBUG
LOCAL_CFLAGS += -g3
endif

# MicroBlaze specific options:
# CFLAGS += -mno-xl-soft-mul -mxl-barrel-shift

# To compile on Plan9:
# CFLAGS += -D_BSD_EXTENSION

# if were running on Windows build for Windows
ifdef WINDOWS
WINDOWS_BUILD=1
endif

# To compile as a shared library:
ifdef SHARED
# all code is position-indep with mingw, avoid warning about useless flag
ifndef WINDOWS_BUILD
LOCAL_CFLAGS += -fPIC -fpic
endif
endif


SOEXT_CRYPTO=so.0

DLEXT=so
# OSX shared library extension:
# DLEXT=dylib

# Windows shared library extension:
ifdef WINDOWS_BUILD
DLEXT=dll
endif

OBJS_CRYPTO=	aes.o   sm4.o	\
		bignum.o asn1parse.o	\
		md5.o hash_wrap.o	\
		rsa.o		sha1.o		\
		sha256.o	\
		threading.o \
		hmac.o \
		oid.o \


.SILENT:

.PHONY: all static shared clean

ifndef SHARED
all: static
else
all: shared static
endif

static: libalicrypto.a 
shared: libalicrypto.$(DLEXT) 

# crypto
libalicrypto.a: $(OBJS_CRYPTO)
	echo "  AR    $@"
	$(AR) -rc $@ $(OBJS_CRYPTO)
	echo "  RL    $@"
	$(AR) -s $@

libalicrypto.$(SOEXT_CRYPTO): $(OBJS_CRYPTO)
	echo "  LD    $@"
	$(CC) -shared -Wl,-soname,$@ $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@ $(OBJS_CRYPTO)

.c.o:
	echo "  CC    $< 	CFLAGS $(CFLAGS)"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -c $<

clean:
ifndef WINDOWS
	rm -f *.o libalicrypto*
else
	del /Q /F *.o libalicrypto*
endif
