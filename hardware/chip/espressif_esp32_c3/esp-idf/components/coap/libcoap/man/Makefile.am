# man/Makefile.am
#
# Copyright (C) 2018-2021 Jon Shallow <supjps-libcoap@jpshallow.com>
#
# This file is part of the CoAP C library libcoap. Please see README and
# COPYING for terms of use.

# picking up the default warning CFLAGS into AM_CFLAGS
AM_CFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include \
            -I$(top_srcdir)/include/coap$(LIBCOAP_API_VERSION) \
            $(WARNING_CFLAGS) $(DTLS_CFLAGS) -std=c99

# Build, not install
noinst_PROGRAMS = examples-code-check

# build manuals only if 'BUILD_MANPAGES' is defined
if BUILD_MANPAGES

# building the manpages

TXT3 = coap_async.txt \
        coap_attribute.txt \
	coap_block.txt \
	coap_cache.txt \
	coap_context.txt \
	coap_endpoint_server.txt \
	coap_encryption.txt \
	coap_endpoint_client.txt \
	coap_handler.txt \
	coap_io.txt \
	coap_keepalive.txt \
	coap_logging.txt \
	coap_observe.txt \
	coap_pdu_access.txt \
	coap_pdu_setup.txt \
	coap_recovery.txt \
	coap_resource.txt \
	coap_session.txt \
	coap_string.txt \
	coap_tls_library.txt

MAN3 = $(TXT3:%.txt=%.3)

man3_MANS = $(MAN3)

TXT5 = coap-client.txt \
       coap-rd.txt \
       coap-server.txt

MAN5 = $(TXT5:%.txt=%.5)

man5_MANS = $(MAN5)

TXT7 = coap.txt

MAN7 = $(TXT7:%.txt=%.7)

man7_MANS = $(MAN7)

.txt.3:
	$(A2X) --doctype manpage --format manpage $<
	$(A2X) --doctype manpage --format xhtml $<

.txt.5:
	$(A2X) --doctype manpage --format manpage $<
	$(A2X) --doctype manpage --format xhtml $<

.txt.7:
	$(A2X) --doctype manpage --format manpage $<
	$(A2X) --doctype manpage --format xhtml $<

# Man pages built by a2x based on the NAMES section of the .txt file.
# Note - this list includes all the defined entries, but a2x only builds the first 10.
A2X_EXTRA_PAGES_3 = @DOLLAR_SIGN@(shell for fil in $(TXT3) ; do sed -ne '/^NAME/,/^SYNOPSIS/p;/^SYNOPSIS/q' $${fil} | \
	sed -ne '/coap_/{ s/ *, */\n/g; p }' | sed -ne 's/^\(coap_[a-zA-Z_0-9]\+\).*$$/\1.3/p' ; done)
A2X_EXTRA_PAGES_5 = @DOLLAR_SIGN@(shell for fil in $(TXT5) ; do sed -ne '/^NAME/,/^SYNOPSIS/p;/^SYNOPSIS/q' $${fil} | \
	sed -ne '/coap-/{ s/ *, */\n/g; p }' | sed -ne 's/^\(coap-[a-zA-Z0-9-]\+\).*$$/\1.5/p' ; done)

# a2x builds alternative .3 files up to a limit of 10 names from the
# NAME section, so that 'man' works against the alternative different
# function names.
#
# However, if there are more alternative names, they need to be defined
# as per below
#
# Then all the alternative names as well as the extras defined below need
# to be cleaned up in a 'make unistall'.
install-man: install-man3 install-man5 install-man7
	@echo ".so man3/coap_cache.3" > coap_cache_get_pdu.3
	@echo ".so man3/coap_cache.3" > coap_cache_get_app_data.3
	@echo ".so man3/coap_cache.3" > coap_cache_set_app_data.3
	@echo ".so man3/coap_context.3" > coap_context_get_session_timeout.3
	@echo ".so man3/coap_context.3" > coap_context_set_csm_timeout.3
	@echo ".so man3/coap_context.3" > coap_context_get_csm_timeout.3
	@echo ".so man3/coap_logging.3" > coap_endpoint_str.3
	@echo ".so man3/coap_logging.3" > coap_session_str.3
	@echo ".so man3/coap_pdu_access.3" > coap_option_filter_set.3
	@echo ".so man3/coap_pdu_access.3" > coap_option_filter_unset.3
	@echo ".so man3/coap_pdu_access.3" > coap_option_iterator_init.3
	@echo ".so man3/coap_pdu_access.3" > coap_option_next.3
	@echo ".so man3/coap_pdu_access.3" > coap_pdu_get_code.3
	@echo ".so man3/coap_pdu_access.3" > coap_pdu_get_mid.3
	@echo ".so man3/coap_pdu_access.3" > coap_pdu_get_token.3
	@echo ".so man3/coap_pdu_access.3" > coap_pdu_get_type.3
	@echo ".so man3/coap_pdu_setup.3" > coap_delete_optlist.3
	@echo ".so man3/coap_pdu_setup.3" > coap_encode_var_safe.3
	@echo ".so man3/coap_pdu_setup.3" > coap_encode_var_safe8.3
	@echo ".so man3/coap_pdu_setup.3" > coap_add_optlist_pdu.3
	@echo ".so man3/coap_pdu_setup.3" > coap_add_option.3
	@echo ".so man3/coap_pdu_setup.3" > coap_add_data.3
	@echo ".so man3/coap_pdu_setup.3" > coap_add_data_blocked_response.3
	@echo ".so man3/coap_pdu_setup.3" > coap_send.3
	@echo ".so man3/coap_pdu_setup.3" > coap_split_path.3
	@echo ".so man3/coap_pdu_setup.3" > coap_split_query.3
	@echo ".so man3/coap_pdu_setup.3" > coap_pdu_set_mid.3
	@echo ".so man3/coap_pdu_setup.3" > coap_pdu_set_code.3
	@echo ".so man3/coap_pdu_setup.3" > coap_pdu_set_type.3
	@echo ".so man3/coap_resource.3" > coap_resource_get_userdata.3
	@echo ".so man3/coap_resource.3" > coap_resource_release_userdata_handler.3
	@echo ".so man3/coap_resource.3" > coap_resource_get_uri_path.3
	@echo ".so man3/coap_session.3" > coap_session_get_context.3
	@echo ".so man3/coap_session.3" > coap_session_get_ifindex.3
	@echo ".so man3/coap_session.3" > coap_session_get_proto.3
	@echo ".so man3/coap_session.3" > coap_session_get_psk_hint.3
	@echo ".so man3/coap_session.3" > coap_session_get_psk_key.3
	@echo ".so man3/coap_session.3" > coap_session_get_state.3
	@echo ".so man3/coap_session.3" > coap_session_get_tls.3
	@echo ".so man3/coap_session.3" > coap_session_get_type.3
	@echo ".so man3/coap_string.3" > coap_delete_bin_const.3
	@echo ".so man3/coap_string.3" > coap_make_str_const.3
	@echo ".so man3/coap_string.3" > coap_string_equal.3
	@echo ".so man3/coap_string.3" > coap_binary_equal.3
	$(INSTALL_DATA) $(A2X_EXTRA_PAGES_3) "$(DESTDIR)$(man3dir)"
	$(INSTALL_DATA) $(A2X_EXTRA_PAGES_5) "$(DESTDIR)$(man5dir)"

# As well as removing the base 'man' pages, remove other .3 files built by
# a2x, as well as build by install-man specials.
uninstall-man: uninstall-man3 uninstall-man5 uninstall-man7
	-(cd $(DESTDIR)$(man3dir) ; rm -f $(A2X_EXTRA_PAGES_3) $(A2X_EXTRA_PAGES_5) )

endif # BUILD_MANPAGES

CLEANFILES = *.3 *.5 *.7 *.xml *.html docbook-xsl.css *.o examples-code-check

clean-local:
	-rm -rf tmp
