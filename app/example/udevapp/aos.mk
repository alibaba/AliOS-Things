NAME := udevapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := udev sample code
$(NAME)_SOURCES := udevapp.c
$(NAME)_COMPONENTS := netmgr cli udev kv

GLOBAL_INCLUDES += ./

ifeq ($(strip $(CONFIG_SYSINFO_DEVICE_NAME)), $(filter $(CONFIG_SYSINFO_DEVICE_NAME), developerkit))
AOS_DEVELOPERKIT_ENABLE_OTA := 1
endif