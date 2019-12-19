NAME := device_sal_esp8266

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := sal hal implementation for esp8266
GLOBAL_DEFINES += DEV_SAL_ESP8266
GLOBAL_CFLAGS += -D__SAL_DEV_NAME=esp8266

$(NAME)_COMPONENTS += yloop

$(NAME)_SOURCES += esp8266.c
$(NAME)_SOURCES += esp8266_at.c

GLOBAL_INCLUDES += ./
