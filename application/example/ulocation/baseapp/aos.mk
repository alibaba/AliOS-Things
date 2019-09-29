NAME := baseapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := ulocation base app

$(NAME)_SOURCES :=   app_entry.c
$(NAME)_SOURCES += linkkit_example_solo.c

$(NAME)_COMPONENTS += linkkit_sdk_c

$(NAME)_COMPONENTS += netmgr cjson
$(NAME)_COMPONENTS += ulocation

$(NAME)_COMPONENTS += ota

# GLOBAL_DEFINES += ON_PRE

ifeq ($(print_heap),1)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif

ifneq ($(HOST_MCU_FAMILY),mcu_esp8266)
$(NAME)_COMPONENTS  += cli
else
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

$(NAME)_INCLUDES += ./

