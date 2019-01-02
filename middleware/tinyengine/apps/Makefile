.PHONY: all clean install

#public vars
sinclude $(BUILDROOT)/build_rules/globalconf.mk


libname = libtiny_engine_app
libtarget_shared = $(libname).$(shared)
libtarget_static = $(libname).$(static)

libsrc = src/tiny_engine_main.o 

CFLAGS += -I./  -I../inc -I$(HAL_INC) -I$(BE_SERVICE_INC) -I$(COMMON_INC) -I$(3RDLIB_INC) -I$(COMPONET_INC) -I$(BONE_EG_INC) -I$(OSAL_INC) -I$(BE_NATIVE_INC) 

MKSPIFFS=$(BUILDROOT)/output/mkspiffs

ifeq ($(PRODUCT_TYPE), developerkit)
IMAGE_SIZE=262144
BLOCK_SIZE=16384
PAGE_SIZE=2048
else ifeq ($(PRODUCT_TYPE), mk3060)
IMAGE_SIZE=262144
BLOCK_SIZE=65536
PAGE_SIZE=256
else
IMAGE_SIZE=524288
BLOCK_SIZE=65536
PAGE_SIZE=256
endif



all: $(libtarget_static)


$(libtarget_static): $(libsrc)
	$(AR) rcs $@ $^ || exit "$$?"

$(libtarget_shared): $(libsrc)
	$(CC) $(SHARED_LDFLAGS) -o $@ $^ $(CFLAGS) $(LDFLAGS)

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
fsimage:
	$(MKSPIFFS) -c js/  -b $(BLOCK_SIZE) -p $(PAGE_SIZE) -s $(IMAGE_SIZE) spiffs.bin
	cp spiffs.bin $(BUILDROOT)/output/

formatfs:
	rm -rf test
	mkdir -p test
	$(MKSPIFFS) -c test/  -b $(BLOCK_SIZE) -p $(PAGE_SIZE) -s $(IMAGE_SIZE) formatapp.bin
	cp formatapp.bin $(BUILDROOT)/output/
	rm -rf test

-include $(libsrc:.o=.d)

clean:
	@echo "clean $(libname)"
	@$(RM)  $(libsrc) $(libtarget_static) $(libsrc:.o=.d*)
