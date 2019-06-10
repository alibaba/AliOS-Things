NAME := ota_hal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := ota porting HAL APIs

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_COMPONENTS += base64

$(NAME)_SOURCES := \
    ota_hal_module.c \
    ota_hal_os.c \

ifneq (,$(filter linux,$(HOST_MCU_FAMILY)))
$(NAME)_SOURCES += ota_hal_host.c
else
ifneq (,$(filter esp32,$(HOST_MCU_FAMILY)))
GLOBAL_DEFINES += AOS_OTA_BANK_DUAL
GLOBAL_DEFINES += AOS_OTA_DISABLE_MD5
$(NAME)_SOURCES += ota_hal_$(HOST_MCU_FAMILY).c
else
ifneq (,$(filter bk7231u,$(HOST_MCU_FAMILY)))
GLOBAL_DEFINES += AOS_OTA_BANK_DUAL
GLOBAL_DEFINES += AOS_OTA_DISABLE_MD5
$(NAME)_SOURCES += ota_hal_beken.c
else
ifneq (,$(filter bk7231s,$(HOST_MCU_FAMILY)))
$(NAME)_SOURCES += ota_hal_beken.c
else
ifneq (,$(filter rtl8710bn,$(HOST_MCU_FAMILY)))
else
ifneq (,$(filter asr5501,$(HOST_MCU_FAMILY)))
GLOBAL_DEFINES += AOS_OTA_DISABLE_MD5
GLOBAL_DEFINES += OTA_BOOT_PAPA_COMPATIBAL
else
$(NAME)_SOURCES += ota_hal_plat.c
endif
endif
endif
endif
endif
endif

ifneq (,$(filter rtl8710bn esp8266,$(HOST_MCU_FAMILY)))
GLOBAL_DEFINES += AOS_OTA_BANK_DUAL
GLOBAL_DEFINES += AOS_OTA_DISABLE_MD5
endif

ifeq ($(FEATURE_SUPPORT_ITLS),y)
GLOBAL_DEFINES += AOS_OTA_ITLS
$(NAME)_COMPONENTS += itls
else
$(NAME)_COMPONENTS += imbedtls
endif

ifeq ($(MD5_CHECK),0)
GLOBAL_DEFINES += AOS_OTA_DISABLE_MD5
endif

GLOBAL_INCLUDES += . \
    ../inc \
    ../src/verify/crc \
    ../src/2nd_boot
