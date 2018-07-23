NAME := linkkitapp_sched
$(NAME)_SOURCES := linkkit_example_sched.c \
                   linkkit_entry.c

$(NAME)_COMPONENTS += framework/protocol/linkkit/sdk \
                      framework/protocol/linkkit/hal \
                      framework/netmgr \
                      framework/common \
                      framework/ywss4linkkit \
                      framework/fota  \
                      utility/cjson 
                      

GLOBAL_DEFINES += MQTT_DIRECT \
                  ALIOT_DEBUG \
                  IOTX_DEBUG \
                  USE_LPTHREAD \
                  CONFIG_DM_DEVTYPE_SINGLE 
                   

                 

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

