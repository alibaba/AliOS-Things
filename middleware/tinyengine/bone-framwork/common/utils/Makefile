.PHONY: all clean install

#public vars
sinclude $(BUILDROOT)/build_rules/globalconf.mk


libname = libutils
libtarget_shared = $(libname).$(shared)
libtarget_static = $(libname).$(static)

libsrc := be_log.o be_utils.o 

CFLAGS += -I./ -I../inc/utils 


#libsrc := $(wildcard *.c)
#-include $(libsrc:.o=.d)

all: $(libtarget_static)


$(libtarget_static): $(libsrc)
	$(AR) rcs $@ $^

%.o:%.c
	@set -e; $(CC) -o $@ -c $< $(CFLAGS)

%.d:%.c
	rm -f $@; $(CC) -MM $< $(CFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(libtarget_shared): $(libsrc)
	$(CC) $(SHARED_LDFLAGS) -o $@ $^ $(CFLAGS) $(LDFLAGS)


install:
	@echo " install $(libtarget_static)"
	mkdir -p $(BUILDROOT)/lib/
	$(CP) -Rf $(libtarget_static) $(BUILDROOT)/lib/
	rm $(libsrc:.o=.d)

-include $(libsrc:.o=.d)

clean:
	@echo "clean $(libname)"
	@$(RM)  $(libsrc) $(libtarget_static) $(libsrc:.o=.d*)
