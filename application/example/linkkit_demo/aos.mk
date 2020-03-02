NAME := linkkit_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := linkkit_demo

$(NAME)_SOURCES :=   app_entry.c maintask.c
$(NAME)_COMPONENTS += libiot_devmodel libiot_awss

$(NAME)_COMPONENTS += netmgr cjson

ifeq ($(AOS_COMP_UND),y)
$(NAME)_COMPONENTS += und
endif

ifeq ($(AOS_COMP_OTA),y)
$(NAME)_COMPONENTS += ota
GLOBAL_DEFINES += ENABLE_AOS_OTA
endif

ifeq ($(LINKKIT_DEMO_CONFIG_CASE_SOLO),y)
$(NAME)_SOURCES += linkkit_example_solo.c
endif

#case cntdown
ifeq ($(LINKKIT_DEMO_CONFIG_CASE_CNTDOWN),y)
$(NAME)_SOURCES += linkkit_example_cntdown.c
endif

#case sched
ifeq ($(LINKKIT_DEMO_CONFIG_CASE_SCHED),y)
$(NAME)_SOURCES += linkkit_example_sched.c
endif

ifeq ($(LINKKIT_DEMO_CONFIG_PRINT_HEAP),y)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif

ifneq ($(HOST_MCU_FAMILY),mcu_esp8266)
$(NAME)_COMPONENTS  += cli
GLOBAL_DEFINES += CLI_CONFIG_STACK_SIZE=3072
else
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

ifeq ($(AWSS_SUPPORT_BLE_PROVISION),y)
ifneq ($(EN_COMBO_NET), y)
$(error need enable AWSS in breeze settings first)
endif

$(NAME)_SOURCES += combo/combo_net.c
$(NAME)_INCLUDES += combo
bz_en_auth := 1
bz_en_awss := 1
endif


$(NAME)_INCLUDES += ./

