SHELL=/usr/bin/env bash

.PHONY:all spiffs cli download_app

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

spiffs:
	cd mkspiffs && make 
	./mkspiffs/mkspiffs -c jspack/  -b $(BLOCK_SIZE) -p $(PAGE_SIZE) -s $(IMAGE_SIZE) spiffs.bin

cli:
	sh ./config_cli.sh