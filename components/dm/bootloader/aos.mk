# component name
NAME := bootloader

# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := an common bootloader to support device OTA upgrade, inlcude DIFF/XZ/COPY/Ymoderm upgrade.

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_SOURCES += boot_main.c \
                   boot_hal.c \
		           ymodem/ymodem.c

GLOBAL_INCLUDES += include updater/include

$(NAME)_PREBUILT_LIBRARY := updater/lib/$(HOST_ARCH)/ota_updater.a

CONFIG_SYSINFO_BOOT_VERSION = boot-1.0.0-$(CURRENT_TIME)
$(info boot version:${CONFIG_SYSINFO_BOOT_VERSION})
GLOBAL_CFLAGS += -DSYSINFO_2NDBOOT_VERSION=\"$(CONFIG_SYSINFO_BOOT_VERSION)\"

