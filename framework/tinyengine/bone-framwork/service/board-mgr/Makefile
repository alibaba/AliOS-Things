.PHONY: all clean install

#public vars
sinclude $(BUILDROOT)/build_rules/globalconf.mk


libname = libboard_mgr
libtarget_shared = $(libname).$(shared)
libtarget_static = $(libname).$(static)

libsrc := board_mgr.o

CFLAGS += -I./  -I$(COMMON_INC) -I$(OSAL_INC) -I$(HAL_INC) -DCONFIG_CJSON_WITHOUT_DOUBLE



all: $(libtarget_static)


$(libtarget_static): $(libsrc)
	$(AR) rcs $@ $^

%.o:%.c
	@set -e; $(CC) -o $@ -c $< $(CFLAGS)

%.d:%.c
	@if [ -f $(BUILDROOT)/build_rules/globalconf.mk ]; then echo "generate header auto rely"; else exit 1; fi;
	rm -f $@; $(CC) -MM $< $(CFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


install:
	@echo " install $(libtarget_static)"
	mkdir -p $(BUILDROOT)/lib/
	$(CP) -Rf $(libtarget_static) $(BUILDROOT)/lib/
	rm -f $(libsrc:.o=.d)

-include $(libsrc:.o=.d)

clean:
	@echo "clean $(libname)"
	@$(RM)  $(libsrc) $(libtarget_static) $(libsrc:.o=.d*)
