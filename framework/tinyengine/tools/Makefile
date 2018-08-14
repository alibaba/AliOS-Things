SHELL=/usr/bin/env bash

.PHONY:all spiffs cli download_app

#modify your hardware board by define PRODUCT_TYPE

PRODUCT_TYPE = developerkit
#PRODUCT_TYPE = mk3060
#PRODUCT_TYPE = esp32devkitc

ifeq ($(PRODUCT_TYPE), developerkit)
IMAGE_SIZE=131072
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

spiffs:
	cd mkspiffs && make 
	./mkspiffs/mkspiffs -c jspack/  -b $(BLOCK_SIZE) -p $(PAGE_SIZE) -s $(IMAGE_SIZE) spiffs.bin

cli:
	sh ./script/config_cli.sh

download_js:
ifeq ($(PRODUCT_TYPE), developerkit)
	sh ./script/developerkit_download_js.sh
else ifeq ($(PRODUCT_TYPE), esp32devkitc)
	sh ./script/esp32_download_js.sh
else ifeq ($(PRODUCT_TYPE), mk3060)
	echo "not support,please use be cli tool to push js"
else
	echo "not support,please use be cli tool to push js"
endif