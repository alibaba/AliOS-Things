NAME := dk_audio

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := Developer Kit audio example
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),developerkit)
$(NAME)_SOURCES := audioapp.c
endif

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := yloop cli

ENABLE_AUDIO_HAL := 1

$(NAME)_INCLUDES += ./
