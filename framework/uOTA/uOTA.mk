NAME := ota

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := An over-the-air update is the wireless delivery of new software. 

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_COMPONENTS += framework.uOTA.hal
$(NAME)_COMPONENTS += framework.uOTA.src.transport
$(NAME)_COMPONENTS += framework.uOTA.src.download
$(NAME)_COMPONENTS += framework.uOTA.src.verify

$(NAME)_SOURCES := ota_service.c

GLOBAL_INCLUDES += inc

GLOBAL_DEFINES += OTA_ALIOS
GLOBAL_DEFINES += OTA_WITH_LINKKIT
GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1
