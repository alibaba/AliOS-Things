.PHONY: all clean install

#public vars
sinclude $(BUILDROOT)/build_rules/globalconf.mk


#native_module = gpio js-lite mqtt zigbee
#todo mk3060 don't have implement rtc hal 
ifeq ($(PRODUCT_TYPE), mk3060)
native_module = js-lite mqtt  wifi uart gpio  http fs hw net 
else ifeq ($(PRODUCT_TYPE), developerkit)
#native_module = js-lite mqtt wifi uart gpio i2c http rtc fs net hw
native_module = js-lite mqtt wifi uart gpio i2c http fs net hw lcd
else
native_module = js-lite mqtt  dac adc pwm wifi uart gpio i2c http rtc fs net  wdg hw
endif

libname = libjse_natvie
libtarget_static = $(libname).$(static)

#libsrc = $(shell foreach native_module)

libcsrc = $(foreach n,$(native_module),$(wildcard $(n)/*.c))
libsrc = $(patsubst %.c,%.o,$(libcsrc))


CFLAGS += -I./
CFLAGS += -I$(COMMON_INC) -I$(BONE_EG_INC) -I$(OSAL_INC) -I$(HAL_INC) -I$(BE_SERVICE_INC)


all: clean $(libtarget_static)


$(libtarget_static): $(libsrc)
	echo $(OS_TYPE)
	@set -e; $(AR) rcs $@ $^


install:
	@echo " install $(libtarget_static)"
	mkdir -p $(BUILDROOT)/lib/
	$(CP) -Rf $(libtarget_static) $(BUILDROOT)/lib/

clean:
	@echo "clean $(libname)"
	@$(RM)  $(libsrc) $(libtarget_static)
