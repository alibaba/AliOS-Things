NAME := mal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.1
$(NAME)_SUMMARY := mqtt adapter layer

$(NAME)_SOURCES := ./mal.c     \
                   ./mqtt_api.c

ifneq (y,$(strip $(AOS_COMP_SDK_WRAPPERS)))
$(NAME)_SOURCES +=  ./mal_aos.c
endif

GLOBAL_DEFINES += WITH_MAL CONFIG_NO_TCPIP

$(NAME)_INCLUDES += ./
