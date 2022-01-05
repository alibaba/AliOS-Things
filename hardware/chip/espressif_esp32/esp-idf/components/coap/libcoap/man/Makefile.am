# man/Makefile.am
#
# Copyright (C)      2018 Jon Shallow <supjps-libcoap@jpshallow.com>
#
# This file is part of the CoAP C library libcoap. Please see README and
# COPYING for terms of use.

# build manuals only if 'BUILD_MANPAGES' is defined
if BUILD_MANPAGES

# building the manpages

TXT3 = coap_attribute.txt \
	coap_context.txt \
	coap_encryption.txt \
	coap_handler.txt \
	coap_logging.txt \
	coap_observe.txt \
	coap_pdu_setup.txt \
	coap_recovery.txt \
	coap_resource.txt \
	coap_session.txt \
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

# Man pages built byt a2x based on the NAMES section of the .txt file
A2X_EXTRA_PAGES = @DOLLAR_SIGN@(shell for fil in $(TXT3) ; do sed -ne '/^NAME/,/^SYNOPSIS/p;/^SYNOPSIS/q' $${fil} | \
	sed -ne '/coap_/{ s/ *, */\n/g; p }' | sed -ne 's/^\(coap_[a-zA-Z_]\+\).*$$/\1.3/p' ; done)

# Extra man pages that need to be installed due to limit of 10
# names built by a2x
# These files are created in install-man
EXTRA_PAGES = \
	coap_context_set_pki_root_cas.3 \
	coap_add_data_blocked_response.3 \
	coap_encode_var_bytes.3 \
	coap_split_path.3 \
	coap_split_query.3 \
	coap_session_get_app_data.3 \
	coap_session_set_app_data.3 \
	coap_endpoint_str.3

# a2x builds alternative .3 files up to a limit of 10 names from the
# NAME section, so that 'man' works against the alternative different
# function names.
# However, if there are more alternative names, they need to be defined
# as per below.
# Then all the alternative names as well as the extras defined below need
# to be cleaned up in a 'make unistall'.
install-man: install-man3 install-man5 install-man7
	@echo ".so man3/coap_context.3" > coap_context_set_pki_root_cas.3
	@echo ".so man3/coap_pdu_setup.3" > coap_add_data_blocked_response.3
	@echo ".so man3/coap_pdu_setup.3" > coap_encode_var_bytes.3
	@echo ".so man3/coap_pdu_setup.3" > coap_split_path.3
	@echo ".so man3/coap_pdu_setup.3" > coap_split_query.3
	@echo ".so man3/coap_session.3" > coap_session_get_app_data.3
	@echo ".so man3/coap_session.3" > coap_session_set_app_data.3
	@echo ".so man3/coap_logging.3" > coap_endpoint_str.3
	@echo ".so man3/coap_logging.3" > coap_session_str.3
	$(INSTALL_DATA) $(EXTRA_PAGES) $(A2X_EXTRA_PAGES) "$(DESTDIR)$(man3dir)"

# As well as removing the base 'man' pages, remove other .3 files built by
# a2x, as well as build by install-man specials.
uninstall-man: uninstall-man3 uninstall-man5 uninstall-man7
	-(cd $(DESTDIR)$(man3dir) ; rm -f $(EXTRA_PAGES) $(A2X_EXTRA_PAGES) )

CLEANFILES = *.3 *.5 *.7 *.xml *.html docbook-xsl.css

endif # BUILD_MANPAGES
