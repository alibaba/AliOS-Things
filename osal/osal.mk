NAME := osal

$(NAME)_MBINS_TYPE := share
$(NAME)_COMPONENTS += middleware.common middleware.alink.cloud

GLOBAL_INCLUDES += ./mico/include ./include

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

ifeq ($(HOST_ARCH),ARM968E-S)
$(NAME)_CFLAGS += -marm
endif

osal ?= rhino

ifeq ($(osal),freertos)
GLOBAL_DEFINES += OSAL_FREERTOS

$(NAME)_SOURCES += \
    aos/aos_freertos.c
endif

ifeq ($(osal),posix)
GLOBAL_DEFINES += OSAL_POSIX

$(NAME)_SOURCES += \
    aos/aos_posix.c
endif

ifeq ($(osal),rhino_sxr)
GLOBAL_DEFINES += OSAL_SXR

$(NAME)_SOURCES += \
    aos/aos_sxr.c
endif

ifeq ($(osal),rhino_sxr_mutios)
GLOBAL_DEFINES += OSAL_SXR_MUTIOS

$(NAME)_SOURCES += \
    aos/aos_rhino_rda8955.c
endif

ifeq ($(osal),rhino)
GLOBAL_DEFINES += OSAL_RHINO
$(NAME)_COMPONENTS += rhino

ifeq ($(HOST_MCU_FAMILY),esp32)
$(NAME)_COMPONENTS += osal.espos
else
ifeq ($(HOST_MCU_FAMILY),esp8266)
$(NAME)_COMPONENTS += osal.espos
endif
endif

ifneq (,$(filter $(PLATFORM), linuxhost mk3060 mk3239 mk3166 mk3165))
$(NAME)_SOURCES += mico/mico_rhino.c
endif

$(NAME)_SOURCES += \
    aos/aos_rhino.c
endif

