NAME := linkkitapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := linkkitapp

$(NAME)_SOURCES :=   app_entry.c
$(NAME)_COMPONENTS += linkkit_sdk_c

$(NAME)_COMPONENTS += netmgr cjson

$(NAME)_COMPONENTS += ota

# LINKKITAPP_CONFIG_DEPRECATED ?= n
# LINKKITAPP_CONFIG_PRINT_HEAP ?= n
# LINKKITAPP_CONFIG_COMBOAPP ?= n

# case solo
ifeq ($(LINKKITAPP_CONFIG_CASE_SOLO),y)
ifeq ($(LINKKITAPP_CONFIG_DEPRECATED),y)
$(NAME)_SOURCES += deprecated/solo.c
GLOBAL_DEFINES += DEPRECATED_LINKKIT
else
$(NAME)_SOURCES += linkkit_example_solo.c
endif
endif

#case cntdown
ifeq ($(LINKKITAPP_CONFIG_CASE_CNTDOWN),y)
ifeq ($(LINKKITAPP_CONFIG_DEPRECATED),y)
$(NAME)_SOURCES += deprecated/cntdown.c
GLOBAL_DEFINES += DEPRECATED_LINKKIT
else
$(NAME)_SOURCES += linkkit_example_cntdown.c
endif
endif

#case sched
ifeq ($(LINKKITAPP_CONFIG_CASE_SCHED),y)
ifeq ($(LINKKITAPP_CONFIG_DEPRECATED),y)
$(NAME)_SOURCES += deprecated/sched.c
GLOBAL_DEFINES += DEPRECATED_LINKKIT
else
$(NAME)_SOURCES += linkkit_example_sched.c
endif
endif

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif

ifeq ($(LINKKITAPP_CONFIG_PRINT_HEAP),y)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif

ifneq ($(HOST_MCU_FAMILY),esp8266)
$(NAME)_COMPONENTS  += cli
else
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

ifeq ($(LINKKITAPP_CONFIG_COMBOAPP),y)
$(NAME)_COMPONENTS += breeze breeze_hal
$(NAME)_SOURCES += combo/combo_net.c
GLOBAL_DEFINES += EN_COMBO_NET
GLOBAL_INCLUDES += combo
bz_en_auth := 1
bz_en_awss := 1
ble := 1
endif


GLOBAL_INCLUDES += ./

