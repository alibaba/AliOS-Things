NAME := recovery

$(NAME)_TYPE := uota

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

OTA_RECOVERY_TYPE_DIRECT := 0  #
OTA_RECOVERY_TYPE_ABBACK := 1  #
OTA_RECOVERY_TYPE_ABBOOT := 2  #

ifeq ($(strip $(CONFIG_SYSINFO_DEVICE_NAME)), $(filter $(CONFIG_SYSINFO_DEVICE_NAME), ESP3060))
$(NAME)_SOURCES := rec_common.c
endif

ifeq ($(strip $(CONFIG_SYSINFO_DEVICE_NAME)), $(filter $(CONFIG_SYSINFO_DEVICE_NAME), ESP8266))
$(NAME)_SOURCES += rec_main.c rec_libc.c

$(NAME)_COMPONENTS +=   middleware.uagent.uota.src.recovery.nbpatch \
                        middleware.uagent.uota.src.recovery.xz-embedded
endif

$(NAME)_INCLUDES += nbpatch

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
GLOBAL_DEFINES += IS_ESP8266
GLOBAL_DEFINES += OTA_RECOVERY_TYPE=$(OTA_RECOVERY_TYPE_ABBOOT)
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), MK3060)
GLOBAL_DEFINES += OTA_RECOVERY_TYPE=$(OTA_RECOVERY_TYPE_ABBACK)
endif

GLOBAL_INCLUDES += ./ ../service
