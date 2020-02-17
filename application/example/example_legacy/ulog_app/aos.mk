NAME := ulog_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := demo for ulog
$(NAME)_SOURCES := ulog_app.c

$(NAME)_COMPONENTS  += cli ulog


# optional dependencies
$(NAME)_COMPONENTS-$(ULOGAPP_CONFIG_UDP) += netmgr

$(NAME)_COMPONENTS-$(ULOGAPP_CONFIG_SPIFFS) += vfs spiffs

$(NAME)_COMPONENTS-$(ULOGAPP_CONFIG_RAW_FATFS) += vfs fatfs

$(NAME)_COMPONENTS-$(ULOGAPP_CONFIG_SD_CARD) += vfs fatfs
