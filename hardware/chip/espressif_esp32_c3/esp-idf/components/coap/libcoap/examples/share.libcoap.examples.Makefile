# Makefile for libcoap standalone examples
#
# Copyright (C) 2021 Olaf Bergmann <bergmann@tzi.org>
#
# This file is part of the CoAP C library libcoap. Please see README and
# COPYING for terms of use.

# Set external variable LIBCOAP if you need a specific libcoap library.
# E.g. libcoap-3-openssl, libcoap-3-gnutls, libcoap-3-mbedtls or libcoap-3-notls
#
LIBCOAP?=libcoap-3

WARNING_CFLAGS= \
	-pedantic \
	-Wall \
	-Wcast-qual \
	-Wextra \
	-Wformat-security \
	-Winline \
	-Wmissing-declarations \
	-Wmissing-prototypes \
	-Wnested-externs \
	-Wpointer-arith \
	-Wshadow \
	-Wstrict-prototypes \
	-Wswitch-default \
	-Wswitch-enum \
	-Wunused \
	-Wwrite-strings

pkgconfig=$(shell pkg-config $(1) $(2))

CPPFLAGS=-D_GNU_SOURCE $(WARNING_CFLAGS) $(call pkgconfig,--cflags,$(LIBCOAP))
CFLAGS=-std=c99
LDLIBS=$(call pkgconfig,--libs-only-l,$(LIBCOAP))
libdirs=$(call pkgconfig,--libs-only-L,$(LIBCOAP))
rpath=$(shell echo "$(libdirs)" | sed -e 's/^-L/-Wl,--rpath /')
LDFLAGS=$(libdirs) $(rpath)

all: coap-client coap-server

clean:
	rm -f *.o coap-client coap-server
