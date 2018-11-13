NAME := linkkitapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_SOURCES :=   app_entry.c
$(NAME)_COMPONENTS += feature.linkkit


$(NAME)_COMPONENTS += network/netmgr \
                      middleware/common \
                      utility/cjson

##ifeq ($(COMPILER),iar)
##$(NAME)_COMPONENTS += feature.linkkit-nouota
##else
$(NAME)_INCLUDES   += ../../../middleware/uagent/uota/src/service
$(NAME)_COMPONENTS += middleware/uagent/uota
##endif

ifeq ($(case),sched)
ifneq ($(deprecated),)
$(NAME)_SOURCES += deprecated/sched.c
GLOBAL_DEFINES += DEPRECATED_LINKKIT
else
$(NAME)_SOURCES += linkkit_example_sched.c
endif
else ifeq ($(case),cntdown)
ifneq ($(deprecated),)
$(NAME)_SOURCES += deprecated/cntdown.c
GLOBAL_DEFINES += DEPRECATED_LINKKIT
else
$(NAME)_SOURCES += linkkit_example_cntdown.c
endif
else
ifneq ($(deprecated),)
$(NAME)_SOURCES += deprecated/solo.c
GLOBAL_DEFINES += DEPRECATED_LINKKIT
else
$(NAME)_SOURCES += linkkit_example_solo.c
endif
endif

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
