NAME := ulog_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := demo for ulog
$(NAME)_SOURCES := ulog_app.c

$(NAME)_COMPONENTS  += cli osal_aos
ifeq ($(ULOGAPP_CONFIG_UDP),y)

$(info >>> switch on test case of ulog: pop out via udp)
$(NAME)_COMPONENTS  += netmgr
endif

ifeq ($(ULOGAPP_CONFIG_SPIFFS),y)
ifeq ($(CSP_LINUXHOST), y)
else
$(info >>> switch on test case of ulog: pop out to file system-spiffs)
$(NAME)_COMPONENTS  += vfs spiffs
endif
endif

ifeq ($(ULOGAPP_CONFIG_FATFS),y)

$(NAME)_COMPONENTS  += vfs
ifneq ($(ULOG_APP_CONFIG_SD_CARD),y)
$(info >>> switch on test case of ulog: pop out to file system fatfs)
ifneq ($(findstring esp32, $(BUILD_STRING)), esp32)
$(info >>> not esp32 device, select fatfs)
$(NAME)_COMPONENTS  += fatfs
endif
GLOBAL_DEFINES += CONFIG_AOS_FATFS_SUPPORT_RAW_FLASH
else
$(info >>> switch on test case of ulog: pop out log files from SD card)
GLOBAL_DEFINES += CONFIG_AOS_FATFS_SUPPORT_MMC
$(NAME)_COMPONENTS  += fatfs
endif
endif
$(NAME)_INCLUDES += ./


