NAME := ota_2nd_boot

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_COMPONENTS += middleware.uagent.uota.src.2nd_boot.nbpatch
$(NAME)_COMPONENTS += middleware.uagent.uota.src.2nd_boot.xz

$(NAME)_SOURCES += rec_libc.c rec_main.c rec_2boot.c rec_ymodem.c

$(NAME)_INCLUDES += nbpatch
GLOBAL_INCLUDES += ./ ../service

CONFIG_SYSINFO_2BOOT_VERSION = aos-2nd_boot-1.0.0
$(info 2boot_version:${CONFIG_SYSINFO_2BOOT_VERSION})
GLOBAL_CFLAGS += -DSYSINFO_2BOOT_VERSION=\"$(CONFIG_SYSINFO_2BOOT_VERSION)\"
