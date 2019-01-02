NAME := otaapp
$(NAME)_SOURCES := otaapp.c \

$(NAME)_COMPONENTS += feature.linkkit-mqtt \
                      network/netmgr \
                      middleware/common \
                      middleware/uagent/uota  \
                      utility/cjson \
		      tools/cli  \

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

ifeq ($(loop),1)
$(NAME)_DEFINES      += TEST_LOOP
endif

ifeq ($(print_heap),1)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif

ifeq ($(HOST_MCU_FAMILY),esp8266)
GLOBAL_DEFINES += FOTA_RAM_LIMIT_MODE
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

GLOBAL_INCLUDES += ./