# tests/Makefile.am
#
# Copyright (C) 2015-2016 Carsten Schoenert <c.schoenert@t-online.de>
#
# SPDX-License-Identifier: BSD-2-Clause
#
# This file is part of the CoAP C library libcoap. Please see README and
# COPYING for terms of use.

# just do anything if 'HAVE_CUNIT' is defined
if HAVE_CUNIT

# picking up the default warning CFLAGS
AM_CFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include $(WARNING_CFLAGS) $(CUNIT_CFLAGS) $(DTLS_CFLAGS) -std=c99

noinst_PROGRAMS = \
 testdriver

testdriver_SOURCES = \
 testdriver.c \
 test_error_response.c \
 test_encode.c \
 test_options.c \
 test_pdu.c \
 test_sendqueue.c \
 test_session.c \
 test_uri.c \
 test_wellknown.c \
 test_tls.c

# The .a file is uses instead of .la so that testdriver can always access the
# internal functions that are not globaly exposed in a .so file.
testdriver_LDADD = $(CUNIT_LIBS) $(top_builddir)/.libs/libcoap-$(LIBCOAP_NAME_SUFFIX).a ${DTLS_LIBS}

# If there is a API change to something $(LIBCOAP_API_VERSION) > 1 there is
# nothing to adopt here. No needed to implement something here because the test
# unit will always be build againts the actual header files!

CLEANFILES = testdriver

all-am: testdriver

endif # HAVE_CUNIT
