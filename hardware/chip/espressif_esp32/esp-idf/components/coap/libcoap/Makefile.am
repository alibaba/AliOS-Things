# Makefile.am for libcoap
#
# Copyright (C) 2010-2017 Olaf Bergmann <bergmann@tzi.org>
#           (C) 2015-2017 Carsten Schoenert <c.schoenert@t-online.de>
#
# This file is part of the CoAP C library libcoap. Please see README and
# COPYING for terms of use.

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

## Additional files for the distribution archive
EXTRA_DIST = \
  CONTRIBUTE \
  TODO \
  LICENSE \
  libcoap-$(LIBCOAP_API_VERSION).pc.in \
  libcoap-$(LIBCOAP_API_VERSION).map \
  libcoap-$(LIBCOAP_API_VERSION).sym \
  examples/coap_list.h \
  examples/getopt.c \
  include/coap$(LIBCOAP_API_VERSION)/utlist.h \
  tests/test_error_response.h \
  tests/test_options.h \
  tests/test_pdu.h \
  tests/test_sendqueue.h \
  tests/test_session.h \
  tests/test_tls.h \
  tests/test_uri.h \
  tests/test_wellknown.h \
  win32/coap-client/coap-client.vcxproj \
  win32/coap-client/coap-client.vcxproj.filters \
  win32/coap-rd/coap-rd.vcxproj \
  win32/coap-rd/coap-rd.vcxproj.filters \
  win32/coap-server/coap-server.vcxproj \
  win32/coap-server/coap-server.vcxproj.filters \
  win32/libcoap.sln \
  win32/libcoap.vcxproj \
  win32/libcoap.vcxproj.filters \
  win32/testdriver/testdriver.vcxproj \
  win32/testdriver/testdriver.vcxproj.filters \
  win32/testdriver/testdriver.vcxproj.user

AM_CFLAGS = -I$(top_builddir)/include/coap$(LIBCOAP_API_VERSION) -I$(top_srcdir)/include/coap$(LIBCOAP_API_VERSION) $(WARNING_CFLAGS) $(DTLS_CFLAGS) -std=c99

SUBDIRS = $(subdirs) . $(MAN_DIR) $(DOC_DIR) tests examples

## Define a libtool archive target "libcoap-@LIBCOAP_NAME_SUFFIX@.la", with
## @LIBCOAP_NAME_SUFFIX@ substituted into the generated Makefile at configure
## time.
## The libtool archive file (.la) will be installed into the directory named
## by the predefined variable $(bindir), along with the actual shared library
## file (.so).
lib_LTLIBRARIES = libcoap-@LIBCOAP_NAME_SUFFIX@.la

libcoap_@LIBCOAP_NAME_SUFFIX@_la_CFLAGS = \
  -fPIC \
  -fPIE \
  $(AM_CFLAGS)

## Define the source file list for the "libcoap.la" target.
## Note that it is not necessary to list header files which are already listed
## elsewhere in a _HEADERS variable assignment.
libcoap_@LIBCOAP_NAME_SUFFIX@_la_SOURCES = \
  src/address.c \
  src/async.c \
  src/block.c \
  src/coap_event.c \
  src/coap_hashkey.c \
  src/coap_gnutls.c \
  src/coap_io.c \
  src/coap_notls.c \
  src/coap_openssl.c \
  src/coap_session.c \
  src/coap_time.c \
  src/coap_tinydtls.c \
  src/coap_debug.c \
  src/encode.c \
  src/mem.c \
  src/net.c \
  src/option.c \
  src/pdu.c \
  src/resource.c \
  src/str.c \
  src/subscribe.c \
  src/uri.c

## Define the list of public header files and their install location.
## The API version is appended to the install folder to being able to
## co-install various versions of libcoap.
libcoap_includedir = $(includedir)/coap$(LIBCOAP_API_VERSION)/

# If there is a API change to something $(LIBCOAP_API_VERSION) > 1 the install
# prefix for the header files has to change to not conflict the older version
# if the user want's to install both versions. There will be something used like
# libcoap_include_HEADERS = \
#  $(top_srcdir)/include/coap-$(LIBCOAP_API_VERSION)/*

libcoap_include_HEADERS = \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/address.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/async.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/bits.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/block.h \
  $(top_builddir)/include/coap$(LIBCOAP_API_VERSION)/coap.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/coap_debug.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/coap_dtls.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/coap_event.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/coap_hashkey.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/coap_io.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/coap_mutex.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/coap_session.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/coap_time.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/encode.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/libcoap.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/mem.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/net.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/option.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/pdu.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/prng.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/resource.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/str.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/subscribe.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/uri.h \
  $(top_srcdir)/include/coap$(LIBCOAP_API_VERSION)/uthash.h

## Instruct libtool to include API version information in the generated shared
## library file (.so). The library ABI version will later defined in configure.ac,
## so that all version information is kept in one place.
libcoap_@LIBCOAP_NAME_SUFFIX@_la_LDFLAGS =					\
  -version-info $(LT_LIBCOAP_CURRENT):$(LT_LIBCOAP_REVISION):$(LT_LIBCOAP_AGE)	\
  @libcoap_SYMBOLS@ \
  $(DTLS_LIBS) \
  -pie

## Collect symbols here we want to ignore while building the helper files
## libcoap-$(LIBCOAP_API_VERSION).{map,sym} for the linker.
CTAGS_IGNORE=-I " \
coap_dtls_context_check_keys_enabled \
coap_dtls_context_set_pki \
coap_dtls_context_set_pki_root_cas \
coap_dtls_context_set_psk \
coap_dtls_free_context \
coap_dtls_free_session \
coap_dtls_get_context_timeout \
coap_dtls_get_overhead \
coap_dtls_get_timeout \
coap_dtls_handle_timeout \
coap_dtls_hello \
coap_dtls_is_context_timeout \
coap_dtls_new_client_session \
coap_dtls_new_context \
coap_dtls_new_server_session \
coap_dtls_receive \
coap_dtls_send \
coap_dtls_session_update_mtu \
coap_dtls_startup \
coap_mfree_endpoint \
coap_packet_extract_pbuf \
coap_pdu_from_pbuf \
coap_session_mfree \
coap_session_new_dtls_session \
coap_socket_accept_tcp \
coap_socket_bind_tcp \
coap_socket_bind_udp \
coap_socket_close \
coap_socket_connect_tcp1 \
coap_socket_connect_tcp2 \
coap_socket_connect_udp \
coap_socket_read \
coap_socket_send \
coap_socket_send_pdu \
coap_socket_write \
coap_tls_free_session \
coap_tls_new_client_session \
coap_tls_new_server_session \
coap_tls_read \
coap_tls_write \
"

# This helper is called by libcoap-$(LIBCOAP_API_VERSION).{map,sym} to see if
# configure has detected a usable version of the ctags program and aborts if not.
check_ctags:
	@if [ "$(CTAGS_PROG)" = "" ]; then \
		echo ;\
		echo "There was no ctags program found by the configure script!" ;\
		echo "ctags is needed for running this target! Please note the warning about the missed ctags program of the configure script." ;\
		echo ;\
		exit 1;\
	fi

## Helper target to generate the symbol table needed by libtool.
## The .map format is used when ld supports linker scripts, otherwise
## it must fall back to a plain symbol file.
update-map-file: libcoap-$(LIBCOAP_API_VERSION).map libcoap-$(LIBCOAP_API_VERSION).sym

libcoap-$(LIBCOAP_API_VERSION).map: check_ctags $(libcoap_include_HEADERS)
	( echo "VER_$(LIBCOAP_API_VERSION) {" ; \
	echo "global:" ; \
	$(CTAGS_PROG) $(CTAGS_IGNORE) -f - --c-kinds=p $(libcoap_include_HEADERS) | awk '/^coap_/ { print "  " $$1 ";" }' | sort -u ; \
	echo "local:" ; \
	echo "  *;" ; \
	echo "};" ) > $(top_builddir)/$@.new
	mv $(top_builddir)/$@.new $(top_builddir)/$@

libcoap-$(LIBCOAP_API_VERSION).sym: check_ctags $(libcoap_include_HEADERS)
	( $(CTAGS_PROG) $(CTAGS_IGNORE) -f - --c-kinds=p $(libcoap_include_HEADERS) | awk '/^coap_/ { print $$1 }' | sort -u ) \
	> $(top_builddir)/$@.new
	mv $(top_builddir)/$@.new $(top_builddir)/$@

## Install the generated pkg-config file (.pc) into the expected location for
## architecture-dependent package configuration information.  Occasionally,
## pkg-config files are also used for architecture-independent data packages,
## in which case the correct install location would be $(datadir)/pkgconfig.
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libcoap-$(LIBCOAP_NAME_SUFFIX).pc

## Define an independent executable script for inclusion in the distribution
## archive.  However, it will not be installed on an end user's system due to
## the noinst_ prefix.
dist_noinst_SCRIPTS = autogen.sh

## various *-local targets
## Remove the helper files for the linker and the pkg-config file if there
## is 'make distclean' called. NOTE: To re create the *.{map,sym} files you
## need to call the target update-map-file after the configure script was
## running!
clean-local:
	-find \( -name '*.gcda' -o -name '*.gcno' -o -name '*.gcov' \) -delete

distclean-local:
	@rm -f src/*.o src/*.lo
	rm -f libcoap-$(LIBCOAP_API_VERSION).map
	rm -f libcoap-$(LIBCOAP_API_VERSION).sym
	rm -f libcoap-$(LIBCOAP_NAME_SUFFIX).pc
	@echo
	@echo "     ---> Please note the following important advice! <---"
	@echo "     The files libcoap-$(LIBCOAP_API_VERSION).{map,sym} are removed by the distclean target!"
	@echo "     To regenerate this two files you need to call 'make update-map-file' first before any"
	@echo "     other Make target. Otherwise the build of libcoap will fail!"
	@echo

## Ensure we have actual *.{map,sym} files if we create a release tarball.
dist-local: update-map-file

## Finaly some phony targets, just to ensure those targets are always buildable
## no matter if the user has created same called files.
.PHONY: update-map-file check_ctags

