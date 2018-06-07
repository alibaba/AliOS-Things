.PHONY: all clean install

#public vars
rule_exist = $(shell if [ -f $(BUILDROOT)/build_rules/globalconf.mk ]; then echo "exist"; else echo "notexist"; fi;)
sinclude $(BUILDROOT)/build_rules/globalconf.mk


libname = libbone_engine
libtarget_shared = $(libname).$(shared)
libtarget_static = $(libname).$(static)

libsrc = jse/be_jse_executor.o \
			jse/be_jse_lex.o \
			jse/be_jse_module.o \
			jse/be_jse_symbol.o \
			jse/be_jse_utils.o \
			core/bone_engine.o \

CFLAGS += -I./ -I./include
CFLAGS += -I$(OS_ROOT)/include
CFLAGS += -I$(COMMON_INC) -I$(BONE_EG_INC) -I$(OSAL_INC) -I$(HAL_INC) 

all: $(libtarget_static) 


$(libtarget_static): $(libsrc) 
	$(AR) rcs $@ $^ || exit "$$?"

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

clean:
	@echo "clean $(libname)"
	rm -f $(libsrc) $(libtarget_static) $(libsrc:.o=.d*)


-include $(libsrc:.o=.d)



