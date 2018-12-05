NAME := ota

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := An over-the-air update is the wireless delivery of new software. 

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall
endif

$(NAME)_COMPONENTS += framework.uOTA.hal
$(NAME)_COMPONENTS += framework.uOTA.src.service
$(NAME)_COMPONENTS += framework.uOTA.src.transport
$(NAME)_COMPONENTS += framework.uOTA.src.download
#$(NAME)_COMPONENTS += framework.uOTA.src.verify

GLOBAL_INCLUDES += inc

ifeq ($(HTTPS_DL),1)
GLOBAL_DEFINES += AOS_OTA_HTTPS
endif
ifeq ($(ITLS_DL),1)
GLOBAL_DEFINES += AOS_OTA_ITLS
endif
ifeq ($(BIN_MD5),1)
GLOBAL_DEFINES += AOS_OTA_MD5
endif

GLOBAL_DEFINES += OTA_ALIOS
GLOBAL_DEFINES += OTA_WITH_LINKKIT
GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1
