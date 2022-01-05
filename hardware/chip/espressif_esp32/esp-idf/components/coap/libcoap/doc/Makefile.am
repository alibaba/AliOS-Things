# doc/Makefile.am
#
# Copyright (C) 2015-2018 Carsten Schoenert <c.schoenert@t-online.de>
#                    2018 Jon Shallow <supjps-libcoap@jpshallow.com>
#
# This file is part of the CoAP C library libcoap. Please see README and
# COPYING for terms of use.


# We can only perfom the targets in this directory if doxygen is present.

if HAVE_DOXYGEN

CLEANFILES = \
  doxygen_sqlite3.db

EXTRA_DIST = \
  main.md

man-page-cleanup:
	rm -f $(top_builddir)/doc/DoxygenLayout.xml
	rm -rf $(top_builddir)/doc/man_tmp
	rm -rf $(top_builddir)/doc/man_html

man-page-prepare: man-page-cleanup
	$(MKDIR_P) $(top_builddir)/doc/man_tmp
	$(MKDIR_P) $(top_builddir)/doc/man_html

man-page-start: man-page-prepare
	@echo '    <tab type="usergroup" visible="yes" url="@ref manpage" title="Manual Pages" intro="">' > $(top_builddir)/doc/insert_file
	@echo '/** @page manpage Manual Pages' > $(top_builddir)/doc/man_tmp/manpage.dox
	@echo '  Here is a list of libcoap API manual pages, some of which have code examples:' >> $(top_builddir)/doc/man_tmp/manpage.dox
	@echo '   <table class="directory">' >> $(top_builddir)/doc/man_tmp/manpage.dox

man-page-build: man-page-start
	@MAN_FILES=`find $(top_srcdir)/man/ -type f -name "coap*.in"` ;\
	HTML_FILES=`find $(top_builddir)/man/ -type f -name "*.html"` ;\
	COUNT_MAN_FILES=`echo $${MAN_FILES} | wc -w` ;\
	COUNT_HTML_FILES=`echo $${HTML_FILES} | wc -w` ;\
	## We need the HTML files from the Asciidoc source files, check if they around, otherwise build them.
	if [ "$${COUNT_MAN_FILES}" != "$${COUNT_HTML_FILES}" ]; then \
		$(MAKE) -C ../man ;\
	fi ;\
	for FILE in $${MAN_FILES} ; do \
		BASE=`basename $${FILE} | cut -d. -f 1` ;\
		MANUAL=`egrep -B 1 "^====" $${FILE} | head -1` ;\
		SUMMARY=`egrep -B 2 "^SYNOPSIS" $${FILE} | sed 's/coap-//g' | cut -d\- -f2 | cut -c2- | head -1` ;\
		## Build the manual insert page
		echo "/// @page man_$${BASE} $${MANUAL}" > $(top_builddir)/doc/man_tmp/$${MANUAL}.dox ;\
		echo "/// @htmlinclude $${BASE}.html $${MANUAL}" >> $(top_builddir)/doc/man_tmp/$${MANUAL}.dox ;\
		## Update insert_file
		echo "      <tab type=\"user\" visible=\"yes\" url=\"@ref man_$${BASE}\" title=\"$${MANUAL} - $${SUMMARY}\" intro=\"\"/>" >> $(top_builddir)/doc/insert_file ;\
		## Update the summary man page
		echo "   <tr id=\"row_$${ID}_\"$${ROW_EVEN}>" >> $(top_builddir)/doc/man_tmp/manpage.dox ;\
		echo "   <td class=\"entry\" align=\"left\"> @ref man_$${BASE} </td><td class=\"desc\" align=\"left\">$${MANUAL} - $${SUMMARY}</td>" >> $(top_builddir)/doc/man_tmp/manpage.dox ;\
		echo "   </tr>" >> $(top_builddir)/doc/man_tmp/manpage.dox ;\
		if [ -z $${ROW_EVEN} ] ; then \
			ROW_EVEN=" class=\"even\"" ;\
		else \
			ROW_EVEN= ;\
		fi \
	done ;\
	## Close off the man page file
	echo '   </table>' >> $(top_builddir)/doc/man_tmp/manpage.dox ;\
	echo ' */' >> $(top_builddir)/doc/man_tmp/manpage.dox ;\
	## Close off the insert_file
	echo '    </tab>' >> $(top_builddir)/doc/insert_file ;\
	echo '    <tab type="user" visible="yes" url="@ref deprecated" title="Deprecated Items" intro=""/>' >> $(top_builddir)/doc/insert_file ;\
	## Create and Update the DoxygenLayout.xml file
	$(DOXYGEN) -l ;\
	$(SED) -i 's/<tab type="pages" visible="yes" /<tab type="pages" visible="no" /g' $(top_builddir)/doc/DoxygenLayout.xml ;\
	$(SED) -i '/<tab type="examples" visible=.*/r insert_file' $(top_builddir)/doc/DoxygenLayout.xml ;\
	$(RM) $(top_builddir)/doc/insert_file ;\
	## Fix up man html files, fixing links, UC Name and Synopsis
	for FILE in $${MAN_FILES} ; do \
		BASE=`basename $${FILE} | cut -d . -f1` ;\
		if [ -f $(top_builddir)/man/$${BASE}.html ]; then \
			cp -f $(top_builddir)/man/$${BASE}.html $(top_builddir)/doc/man_html/$${BASE}.html ;\
			## Correct case sensitive Name and Synopsis
			$(SED) -i 's^<h2>Name</h2>^<h2>NAME</h2>^g' $(top_builddir)/doc/man_html/$${BASE}.html ;\
			$(SED) -i 's^<h2>Synopsis</h2>^<h2>SYNOPSIS</h2>^g' $(top_builddir)/doc/man_html/$${BASE}.html ;\
		else \
			echo "ERROR: $(top_builddir)/man/$${BASE}.html not found!";\
			exit 1 ;\
		fi ;\
		for ENTRY in $${MAN_FILES} ; do \
			EBASE=`basename $${ENTRY} | cut -d . -f1` ;\
			MANUAL=`egrep -B 1 "^====" $${ENTRY} | head -1` ;\
			SECTION=`echo $${MANUAL} | cut -d\( -f2 | cut -d\) -f1` ;\
			$(SED) -i "s^<span class=\"strong\"><strong>$${EBASE}</strong></span>($${SECTION})^<a href=\"man_$${EBASE}.html\" target=\"_self\"><span class=\"strong\"><strong>$${EBASE}</strong></span>($${SECTION})</a>^g" $(top_builddir)/doc/man_html/$${BASE}.html ;\
		done ;\
		echo "finalized addition $${BASE}.html" ;\
	done

# This target has no check for automated install data! It's simply assumed that
# that the 'all' target is running before, in short: the user has to run first
# 'make' before running 'make install'!
install-data-am:
	@if [ ! -d $(top_builddir)/doc/html ]; then \
		echo ;\
		echo "     No install data in '$(top_builddir)/doc/html'found! Please run 'make all' first." ;\
		echo ;\
		exit 1 ;\
	fi
	$(MKDIR_P) $(DESTDIR)$(htmldir)/html || exit 1
	cp -a -f $(top_builddir)/doc/html $(DESTDIR)$(htmldir)
	find $(DESTDIR)$(htmldir) -type f -name "*.md5" -exec rm {} \;

uninstall-am:
	-rm -rf $(DESTDIR)$(htmldir)/html

if BUILD_MANPAGES
all: man-page-build
	$(DOXYGEN) Doxyfile

else
#
# Need to make sure the man directories are in place, but empty
all: man-page-prepare
	$(DOXYGEN) Doxyfile
endif # BUILD_MANPAGES

clean-local:
	-rm -rf $(top_builddir)/doc/html  $(top_builddir)/doc/man_tmp  $(top_builddir)/doc/man_html  $(top_builddir)/doc/DoxygenLayout.xml

distclean-local: clean-local

endif # HAVE_DOXYGEN

.PHONY: man-page-cleanup man-page-prepare man-page-start man-page-build
