NAME := ulocationapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := ulocationapp

$(NAME)_SOURCES :=   app_entry.c
$(NAME)_SOURCES += linkkit_example_solo.c

$(NAME)_COMPONENTS += linkkit_sdk_c

$(NAME)_COMPONENTS += netmgr cjson 
$(NAME)_COMPONENTS += middleware/ulocation 

$(NAME)_COMPONENTS += ota

# GLOBAL_DEFINES += ON_PRE

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif

ifeq ($(print_heap),1)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif

ifneq ($(HOST_MCU_FAMILY),mcu_esp8266)
$(NAME)_COMPONENTS  += cli
else
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

GLOBAL_INCLUDES += ./

