NAME := linkkitapp
$(NAME)_SOURCES :=   app_entry.c

$(NAME)_COMPONENTS += feature.linkkit


$(NAME)_COMPONENTS += network/netmgr \
                      middleware/common \
                      middleware/uagent/uota  \
                      utility/cjson    
          
                                        
$(NAME)_INCLUDES += \
    ../../../middleware/uagent/uota/src/service

ifeq ($(case),sched)
$(NAME)_SOURCES += linkkit_example_sched.c 
GLOBAL_DEFINES += DEPRECATED_LINKKIT 
else ifeq ($(case),cntdown)
ifneq ($(newapi),)
$(NAME)_SOURCES += newapi/cntdown.c 
else
$(NAME)_SOURCES += linkkit_example_cntdown.c 
GLOBAL_DEFINES += DEPRECATED_LINKKIT 
endif
else
ifneq ($(newapi),)
$(NAME)_SOURCES += newapi/solo.c 
else
$(NAME)_SOURCES += linkkit_example_solo.c 
GLOBAL_DEFINES += DEPRECATED_LINKKIT 
endif
endif

GLOBAL_DEFINES += CONFIG_DM_DEVTYPE_SINGLE

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
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

GLOBAL_INCLUDES += ./
