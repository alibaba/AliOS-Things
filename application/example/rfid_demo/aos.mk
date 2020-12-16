NAME := rfid_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := rfid_demo

$(NAME)_SOURCES :=   app_entry.c maintask.c rfid_app.c
$(NAME)_COMPONENTS += libiot_devmodel libiot_awss libiot_dynreg

$(NAME)_COMPONENTS += netmgr cjson

ifeq ($(AOS_COMP_UND),y)
$(NAME)_COMPONENTS += und
endif

$(NAME)_COMPONENTS-$(ENABLE_AOS_OTA) += ota
$(NAME)_COMPONENTS-$(AOS_COMP_SDK_AWSS) += libiot_awss


#ifeq ($(LINKKIT_DEMO_CONFIG_CASE_SOLO),y)
$(NAME)_SOURCES += linkkit_event.c
#endif

ifeq ($(LINKKIT_DEMO_CONFIG_PRINT_HEAP),y)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif

GLOBAL_DEFINES += CLI_CONFIG_STACK_SIZE=3072

$(NAME)_INCLUDES += ./

