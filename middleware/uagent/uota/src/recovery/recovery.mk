NAME := recovery

$(NAME)_TYPE := uota

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

ifeq ($(strip $(CONFIG_SYSINFO_DEVICE_NAME)), $(filter $(CONFIG_SYSINFO_DEVICE_NAME), MK3060 ESP8266))
$(NAME)_SOURCES := \
    rec_main.c \
    rec_libc.c

$(NAME)_COMPONENTS += middleware.uagent.uota.src.recovery.nbpatch \
                      middleware.uagent.uota.src.recovery.xz-embedded
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
GLOBAL_DEFINES += IS_ESP8266
GLOBAL_DEFINES += RECOVERY_FLASH_COPY
endif

GLOBAL_INCLUDES += .
