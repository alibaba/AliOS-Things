NAME := ota_2ndboot

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := an common 2ndboot to support device OTA upgrade, inlcude DIFF/XZ/COPY/Ymoderm upgrade.

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_COMPONENTS += ota_updater

$(NAME)_SOURCES += 2ndboot.c \
                   ymodem.c

GLOBAL_INCLUDES += include

CONFIG_SYSINFO_2NDBOOT_VERSION = 2ndboot-1.0.0-$(CURRENT_TIME)
$(info 2ndboot version:${CONFIG_SYSINFO_2NDBOOT_VERSION})
GLOBAL_CFLAGS += -DSYSINFO_2NDBOOT_VERSION=\"$(CONFIG_SYSINFO_2NDBOOT_VERSION)\"
