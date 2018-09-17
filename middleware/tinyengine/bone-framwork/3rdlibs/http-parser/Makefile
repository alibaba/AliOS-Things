.PHONY: all clean install

#public vars
sinclude $(BUILDROOT)/build_rules/globalconf.mk


libname = libhttp-parser
libtarget_shared = $(libname).$(shared)
libtarget_static = $(libname).$(static)

libsrc = http_parser.o

CFLAGS += -I./

#libsrc := $(wildcard *.c)

all: $(libtarget_static)


$(libtarget_static): $(libsrc)
	$(AR) rcs $@ $^

$(libtarget_shared): $(libsrc)
	$(CC) $(SHARED_LDFLAGS) -o $@ $^ $(CFLAGS) $(LDFLAGS)


install:
	@echo " install $(libtarget_static)"
	mkdir -p $(BUILDROOT)/lib/
	$(CP) -Rf $(libtarget_static) $(BUILDROOT)/lib/

clean:
	@echo "clean $(libname)"
	@$(RM) -f *.o $(libtarget_static)
