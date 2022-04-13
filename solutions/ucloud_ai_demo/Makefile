CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

MK_GENERATED_IMGS_PATH:=generated
PRODUCT_BIN:=product

.PHONY:startup
startup: all

all:
	@echo "Build Solution by $(BOARD) "
	$(CPRE) scons $(VERB) --board=$(BOARD) -j4
	@echo ucloud ai demo build Done

	@echo [INFO] Create bin files
#	$(CPRE) $(PRODUCT_BIN) image $(MK_GENERATED_IMGS_PATH)/images.zip -i $(MK_GENERATED_IMGS_PATH)/data -l -p
#	$(CPRE) $(PRODUCT_BIN) image $(MK_GENERATED_IMGS_PATH)/images.zip -e $(MK_GENERATED_IMGS_PATH) -x

.PHONY:flash
flash:
	$(CPRE) $(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -w prim

.PHONY:flashall
flashall:
	$(CPRE) $(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -a

sdk:
	$(CPRE) haas sdk

.PHONY:clean
clean:
	$(CPRE) scons -c
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm yoc_sdk yoc.* generated out -rf
