NAME := ota_download

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Download firmware form OTA server.

ifeq ($(DOWNLOAD),coap)
$(NAME)_SOURCES := ota_download_coap.c
else
$(NAME)_SOURCES := ota_download_http.c 
endif

ifeq ($(TLS_DL),1)
GLOBAL_DEFINES += AOS_OTA_TLS
endif
ifeq ($(ITLS_DL),1)
GLOBAL_DEFINES += AOS_OTA_ITLS
endif
                   
GLOBAL_INCLUDES += . ../../inc ../../hal ../verify


