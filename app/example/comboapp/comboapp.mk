NAME := comboapp
$(NAME)_SOURCES := linkkit_sample_solo.c \
                   comboapp.c

$(NAME)_COMPONENTS += feature.linkkit \
                      network/netmgr \
                      middleware/common \
		      middleware/uagent/uota  \
                      utility/cjson 

$(NAME)_COMPONENTS += bluetooth/breeze \
                      bluetooth/breeze/hal/ble \
                      bluetooth/profile

GLOBAL_CFLAGS += -DCONFIG_DM_DEVTYPE_SINGLE  \
                 -DMQTT_DIRECT                  

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

ifeq ($(print_heap),1)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif

ifneq ($(HOST_MCU_FAMILY),esp8266)
$(NAME)_COMPONENTS  += cli
GLOBAL_DEFINES += CONFIG_AOS_CLI
else
GLOBAL_DEFINES += FOTA_RAM_LIMIT_MODE
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

GLOBAL_DEFINES += CONFIG_COMBO_AWSS
GLOBAL_DEFINES += AWSS_NEED_REBOOT BLE_APP_RECONFIG_AISILOP

ble := 1

GLOBAL_INCLUDES += ./