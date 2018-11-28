NAME := ota

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := An over-the-air update is the wireless delivery of new software. 

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_COMPONENTS += middleware.uagent.uota.hal
$(NAME)_COMPONENTS += middleware.uagent.uota.src.transport
$(NAME)_COMPONENTS += middleware.uagent.uota.src.download
$(NAME)_COMPONENTS += middleware.uagent.uota.src.verify

$(NAME)_SOURCES := ota_service.c

GLOBAL_INCLUDES += inc

ifeq ($(HTTPS_DL),1)
GLOBAL_DEFINES += AOS_OTA_HTTPS
endif
ifeq ($(ITLS_DL),1)
GLOBAL_DEFINES += AOS_OTA_ITLS
endif

GLOBAL_DEFINES += OTA_ALIOS
GLOBAL_DEFINES += OTA_WITH_LINKKIT
GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1
