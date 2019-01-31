NAME := linkkitapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := linkkitapp

$(NAME)_SOURCES :=   app_entry.c
$(NAME)_COMPONENTS += linkkit_sdk_c

$(NAME)_COMPONENTS += netmgr cjson

$(NAME)_COMPONENTS += ota

LINKKITAPP_CONFIG_DEPRECATED ?= 0
LINKKITAPP_CONFIG_PRINT_HEAP ?= 0
LINKKITAPP_CONFIG_CASE ?=
LINKKITAPP_CONFIG_COMBOAPP ?= 0

deprecated ?= $(LINKKITAPP_CONFIG_DEPRECATED)
print_heap ?= $(LINKKITAPP_CONFIG_PRINT_HEAP)
case ?= $(LINKKITAPP_CONFIG_CASE)
en_comboapp ?= $(LINKKITAPP_CONFIG_COMBOAPP)

ifeq ($(case),sched)
ifeq ($(deprecated),1)
$(NAME)_SOURCES += deprecated/sched.c
GLOBAL_DEFINES += DEPRECATED_LINKKIT
else
$(NAME)_SOURCES += linkkit_example_sched.c
endif
else ifeq ($(case),cntdown)
ifeq ($(deprecated),1)
$(NAME)_SOURCES += deprecated/cntdown.c
GLOBAL_DEFINES += DEPRECATED_LINKKIT
else
$(NAME)_SOURCES += linkkit_example_cntdown.c
endif
else
ifeq ($(deprecated),1)
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
else
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

en_comboapp := 0
ifeq ($(en_comboapp), 1)
$(NAME)_COMPONENTS += breeze breeze_hal
$(NAME)_SOURCES += combo/combo_net.c
GLOBAL_DEFINES += EN_COMBO_NET
GLOBAL_INCLUDES += combo
bz_en_auth := 1
bz_en_awss := 1
ble := 1
endif


GLOBAL_INCLUDES += ./

