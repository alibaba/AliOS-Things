NAME := otaapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := OTA demo app for developers

OTAAPP_CONFIG_TEST_LOOP ?= 0
loop ?= $(OTAAPP_CONFIG_TEST_LOOP)

$(NAME)_SOURCES := otaapp.c 
$(NAME)_COMPONENTS := linkkit_sdk_c netmgr ota cjson cli
ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif

ifeq ($(loop),1)
$(NAME)_DEFINES     += TEST_LOOP
endif

#ifeq ($(strip $(CONFIG_SYSINFO_DEVICE_NAME)), $(filter $(CONFIG_SYSINFO_DEVICE_NAME), developerkit))
#AOS_DEVELOPERKIT_ENABLE_OTA := 1
#endif

GLOBAL_INCLUDES += ./
