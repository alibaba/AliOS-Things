
NAME := recovery_2boot

$(NAME)_TYPE := uota

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

ifeq ($(strip $(CONFIG_SYSINFO_DEVICE_NAME)), $(filter $(CONFIG_SYSINFO_DEVICE_NAME), MK3060))
$(NAME)_SOURCES := \
    rec_main.c \
    rec_libc.c \
    2boot/rec_2boot.c

$(NAME)_COMPONENTS +=   middleware.uagent.uota.src.recovery.nbpatch \
                        middleware.uagent.uota.src.recovery.xz-embedded
endif

$(NAME)_INCLUDES += nbpatch

ifeq ($(strip $(CONFIG_SYSINFO_DEVICE_NAME)), $(filter $(CONFIG_SYSINFO_DEVICE_NAME), MK3060 MK3080))
GLOBAL_DEFINES += OTA_RECOVERY_TYPE=1 REC_2BOOT_WITH_CMDLINE
endif

GLOBAL_INCLUDES += ./ ../service

CONFIG_SYSINFO_2BOOT_VERSION = aos-2boot-1.0.0
$(info 2boot_version:${CONFIG_SYSINFO_2BOOT_VERSION})
GLOBAL_CFLAGS += -DSYSINFO_2BOOT_VERSION=\"$(CONFIG_SYSINFO_2BOOT_VERSION)\"