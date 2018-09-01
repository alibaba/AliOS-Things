NAME := linkkitapp
$(NAME)_SOURCES :=   linkkit_entry.c

$(NAME)_COMPONENTS += feature.linkkit \
                      network/netmgr \
                      middleware/common \
                      middleware/uagent/uota  \
                      utility/cjson    

$(NAME)_INCLUDES += \
    ../../../middleware/uagent/uota/src/service

ifeq ($(case),sched)
$(NAME)_SOURCES += linkkit_example_sched.c 
else ifeq ($(case),cntdown)
ifneq ($(newapi),)
$(NAME)_SOURCES += newapi/cntdown.c 
else
$(NAME)_SOURCES += linkkit_example_cntdown.c 
endif
else
ifneq ($(newapi),)
$(NAME)_SOURCES += newapi/solo.c 
else
$(NAME)_SOURCES += linkkit_example_solo.c 
endif
endif

GLOBAL_CFLAGS += -DCONFIG_DM_DEVTYPE_SINGLE   -DDEPRECATED_LINKKIT

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

