NAME := linkkitapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := linkkitapp

$(NAME)_SOURCES :=   app_entry.c
$(NAME)_COMPONENTS += feature.linkkit

$(NAME)_COMPONENTS += netmgr cjson

##ifeq ($(COMPILER),iar)
##$(NAME)_COMPONENTS += feature.linkkit-nouota
##else
$(NAME)_COMPONENTS += ota
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
$(NAME)_COMPONENTS  += lwip
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

en_comboapp := 0
ifeq ($(en_comboapp), 1)
$(NAME)_COMPONENTS += breeze breeze_hal bt_profile
$(NAME)_SOURCES += combo/combo_net.c
GLOBAL_DEFINES += EN_COMBO_NET
bz_en_auth := 1
bz_en_awss := 1
ble := 1
endif


GLOBAL_INCLUDES += ./
