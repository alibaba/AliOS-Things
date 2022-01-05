# doc/Makefile.am
#
# Copyright (C) 2015-2016 Carsten Schoenert <c.schoenert@t-online.de>
#
# This file is part of the CoAP C library libcoap. Please see README and
# COPYING for terms of use.

# just do anything if 'HAVE_CUNIT' is defined
if HAVE_CUNIT

# picking up the default warning CFLAGS
AM_CFLAGS = -I$(top_builddir)/include/coap$(LIBCOAP_API_VERSION) -I$(top_srcdir)/include/coap$(LIBCOAP_API_VERSION) $(WARNING_CFLAGS) $(CUNIT_CFLAGS) $(DTLS_CFLAGS) -std=c99

noinst_PROGRAMS = \
 testdriver

testdriver_SOURCES = \
 testdriver.c \
 test_error_response.c \
 test_options.c \
 test_pdu.c \
 test_sendqueue.c \
 test_session.c \
 test_uri.c \
 test_wellknown.c \
 test_tls.c

testdriver_LDADD = $(CUNIT_LIBS) $(DTLS_LIBS) $(top_builddir)/.libs/libcoap-$(LIBCOAP_NAME_SUFFIX).la

# If there is a API change to something $(LIBCOAP_API_VERSION) > 1 there is
# nothing to adopt here. No needed to implement something here because the test
# unit will always be build againts the actual header files!

CLEANFILES = testdriver

all-am: testdriver

endif # HAVE_CUNIT
