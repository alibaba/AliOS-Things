# Variables:
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
includedir = $(prefix)/include
pkgconfigdir = $(libdir)/pkgconfig

CFLAGS = -Wall -Wextra
LDFLAGS_GCSECTIONS = -Wl,--gc-sections
LDFLAGS += $(if $(gc_sections-pass),$(LDFLAGS_GCSECTIONS))
LDLIBS = -lm

GIT_ARCHIVE = git archive --prefix="$(PACKAGE)/" -9
INSTALL = install
INSTALL_DATA = $(INSTALL) -m 644
INSTALL_PROGRAM = $(INSTALL) -m 755
QMAKE = qmake
MKDIR = mkdir -p
RMDIR = rmdir
SED = sed

# Our sources
TINYCBOR_HEADERS = src/cbor.h src/cborjson.h src/tinycbor-version.h
TINYCBOR_FREESTANDING_SOURCES = \
	src/cborerrorstrings.c \
	src/cborencoder.c \
	src/cborencoder_close_container_checked.c \
	src/cborparser.c \
	src/cborpretty.c \
#
CBORDUMP_SOURCES = tools/cbordump/cbordump.c

BUILD_SHARED = $(shell file -L /bin/sh 2>/dev/null | grep -q ELF && echo 1)
BUILD_STATIC = 1

ifneq ($(BUILD_STATIC),1)
ifneq ($(BUILD_SHARED),1)
  $(error error: BUILD_STATIC and BUILD_SHARED can not be both disabled)
endif
endif

INSTALL_TARGETS += $(bindir)/cbordump
ifeq ($(BUILD_SHARED),1)
BINLIBRARY=lib/libtinycbor.so
INSTALL_TARGETS += $(libdir)/libtinycbor.so
INSTALL_TARGETS += $(libdir)/libtinycbor.so.$(SOVERSION)
INSTALL_TARGETS += $(libdir)/libtinycbor.so.$(VERSION)
endif
ifeq ($(BUILD_STATIC),1)
BINLIBRARY=lib/libtinycbor.a
INSTALL_TARGETS += $(libdir)/libtinycbor.a
endif
INSTALL_TARGETS += $(pkgconfigdir)/tinycbor.pc
INSTALL_TARGETS += $(TINYCBOR_HEADERS:src/%=$(includedir)/tinycbor/%)

# setup VPATH
MAKEFILE := $(lastword $(MAKEFILE_LIST))
SRCDIR := $(dir $(MAKEFILE))
VPATH = $(SRCDIR):$(SRCDIR)/src

# Our version
GIT_DIR := $(strip $(shell git -C $(SRCDIR). rev-parse --git-dir 2> /dev/null))
VERSION = $(shell cat $(SRCDIR)VERSION)
SOVERSION = $(shell cut -f1-2 -d. $(SRCDIR)VERSION)
PACKAGE = tinycbor-$(VERSION)

# Check that QMAKE is Qt 5
ifeq ($(origin QMAKE),file)
  check_qmake = $(strip $(shell $(1) -query QT_VERSION 2>/dev/null | cut -b1))
  ifneq ($(call check_qmake,$(QMAKE)),5)
    QMAKE := qmake -qt5
    ifneq ($(call check_qmake,$(QMAKE)),5)
      QMAKE := qmake-qt5
      ifneq ($(call check_qmake,$(QMAKE)),5)
        QMAKE := @echo >&2 $(MAKEFILE): Cannot find a Qt 5 qmake; false
      endif
    endif
  endif
endif

-include .config

ifeq ($(wildcard .config),)
    $(info .config file not yet created)
endif

ifeq ($(freestanding-pass),1)
TINYCBOR_SOURCES = $(TINYCBOR_FREESTANDING_SOURCES)
else
TINYCBOR_SOURCES = \
	$(TINYCBOR_FREESTANDING_SOURCES) \
	src/cborparser_dup_string.c \
	src/cborpretty_stdio.c \
	src/cbortojson.c \
	src/cborvalidation.c \
#
# if open_memstream is unavailable on the system, try to implement our own
# version using funopen or fopencookie
ifeq ($(open_memstream-pass),)
  ifeq ($(funopen-pass)$(fopencookie-pass),)
    CFLAGS += -DWITHOUT_OPEN_MEMSTREAM
    ifeq ($(wildcard .config),.config)
        $(warning warning: funopen and fopencookie unavailable, open_memstream can not be implemented and conversion to JSON will not work properly!)
    endif
  else
    TINYCBOR_SOURCES += src/open_memstream.c
  endif
endif
endif

# json2cbor depends on an external library (cjson)
ifneq ($(cjson-pass)$(system-cjson-pass),)
  JSON2CBOR_SOURCES = tools/json2cbor/json2cbor.c
  INSTALL_TARGETS += $(bindir)/json2cbor
  ifeq ($(system-cjson-pass),1)
    LDFLAGS_CJSON = -lcjson
  else
    JSON2CBOR_SOURCES += src/cjson/cJSON.c
    json2cbor_CCFLAGS = -I$(SRCDIR)src/cjson
  endif
endif

# Rules
all: .config \
	$(if $(subst 0,,$(BUILD_STATIC)),lib/libtinycbor.a) \
	$(if $(subst 0,,$(BUILD_SHARED)),lib/libtinycbor.so) \
	$(if $(freestanding-pass),,bin/cbordump) \
	tinycbor.pc
all: $(if $(JSON2CBOR_SOURCES),bin/json2cbor)
check: tests/Makefile | $(BINLIBRARY)
	$(MAKE) -C tests check
silentcheck: | $(BINLIBRARY)
	TESTARGS=-silent $(MAKE) -f $(MAKEFILE) -s check
configure: .config
.config: Makefile.configure
	$(MAKE) -f $(SRCDIR)Makefile.configure OUT='>&9' configure 9> $@

lib/libtinycbor-freestanding.a: $(TINYCBOR_FREESTANDING_SOURCES:.c=.o)
	@$(MKDIR) -p lib
	$(AR) cqs $@ $^

lib/libtinycbor.a: $(TINYCBOR_SOURCES:.c=.o)
	@$(MKDIR) -p lib
	$(AR) cqs $@ $^

lib/libtinycbor.so: $(TINYCBOR_SOURCES:.c=.pic.o)
	@$(MKDIR) -p lib
	$(CC) -shared -Wl,-soname,libtinycbor.so.$(SOVERSION) -o lib/libtinycbor.so.$(VERSION) $(LDFLAGS) $^ $(LDLIBS)
	cd lib ; ln -sf libtinycbor.so.$(VERSION) libtinycbor.so ; ln -sf libtinycbor.so.$(VERSION) libtinycbor.so.$(SOVERSION)

bin/cbordump: $(CBORDUMP_SOURCES:.c=.o) $(BINLIBRARY)
	@$(MKDIR) -p bin
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

bin/json2cbor: $(JSON2CBOR_SOURCES:.c=.o) $(BINLIBRARY)
	@$(MKDIR) -p bin
	$(CC) -o $@ $(LDFLAGS) $^ $(LDFLAGS_CJSON) $(LDLIBS)

tinycbor.pc: tinycbor.pc.in
	$(SED) > $@ < $< \
		-e 's,@prefix@,$(prefix),' \
		-e 's,@exec_prefix@,$(exec_prefix),' \
		-e 's,@libdir@,$(libdir),' \
		-e 's,@includedir@,$(includedir),' \
		-e 's,@version@,$(VERSION),'

tests/Makefile: tests/tests.pro
	$(QMAKE) $(QMAKEFLAGS) -o $@ $<

$(PACKAGE).tar.gz: | .git
	GIT_DIR=$(SRCDIR).git $(GIT_ARCHIVE) --format=tar.gz -o "$(PACKAGE).tar.gz" HEAD
$(PACKAGE).zip: | .git
	GIT_DIR=$(SRCDIR).git $(GIT_ARCHIVE) --format=zip -o "$(PACKAGE).zip" HEAD

$(DESTDIR)$(libdir)/%: lib/%
	$(INSTALL) -d $(@D)
	$(INSTALL_DATA) $< $@
$(DESTDIR)$(bindir)/%: bin/%
	$(INSTALL) -d $(@D)
	$(INSTALL_PROGRAM) $< $@
$(DESTDIR)$(pkgconfigdir)/%: %
	$(INSTALL) -d $(@D)
	$(INSTALL_DATA) $< $@
$(DESTDIR)$(includedir)/tinycbor/%: src/%
	$(INSTALL) -d $(@D)
	$(INSTALL_DATA) $< $@

install-strip:
	$(MAKE) -f $(MAKEFILE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' install

install: $(INSTALL_TARGETS:%=$(DESTDIR)%)
uninstall:
	$(RM) $(INSTALL_TARGETS:%=$(DESTDIR)%)

mostlyclean:
	$(RM) $(TINYCBOR_SOURCES:.c=.o)
	$(RM) $(TINYCBOR_SOURCES:.c=.pic.o)
	$(RM) $(CBORDUMP_SOURCES:.c=.o)

clean: mostlyclean
	$(RM) bin/cbordump
	$(RM) bin/json2cbor
	$(RM) lib/libtinycbor.a
	$(RM) lib/libtinycbor-freestanding.a
	$(RM) tinycbor.pc
	$(RM) lib/libtinycbor.so*
	test -e tests/Makefile && $(MAKE) -C tests clean || :

distclean: clean
	test -e tests/Makefile && $(MAKE) -C tests distclean || :

docs:
	cd $(SRCDIR)src && VERSION=$(VERSION) doxygen $(SRCDIR)/../Doxyfile

dist: $(PACKAGE).tar.gz $(PACKAGE).zip
distcheck: .git
	-$(RM) -r $${TMPDIR-/tmp}/tinycbor-distcheck
	GIT_DIR=$(SRCDIR).git git archive --prefix=tinycbor-distcheck/ --format=tar HEAD | tar -xf - -C $${TMPDIR-/tmp}
	cd $${TMPDIR-/tmp}/tinycbor-distcheck && $(MAKE) silentcheck
	$(RM) -r $${TMPDIR-/tmp}/tinycbor-distcheck

tag: distcheck
	@cd $(SRCDIR). && perl scripts/maketag.pl

.PHONY: all check silentcheck configure install uninstall
.PHONY: mostlyclean clean distclean
.PHONY: docs dist distcheck release
.SECONDARY:

cflags := $(CPPFLAGS) -I$(SRCDIR)src
cflags += -std=gnu99 $(CFLAGS)
%.o: %.c
	@test -d $(@D) || $(MKDIR) $(@D)
	$(CC) $(cflags) $($(basename $(notdir $@))_CCFLAGS) -c -o $@ $<
%.pic.o: %.c
	@test -d $(@D) || $(MKDIR) $(@D)
	$(CC) $(cflags) -fPIC $($(basename $(notdir $@))_CCFLAGS) -c -o $@ $<

-include src/*.d
