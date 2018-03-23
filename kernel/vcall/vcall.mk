NAME := vcall

$(NAME)_TYPE := kernel
GLOBAL_INCLUDES += ./mico/include

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

ifeq ($(HOST_ARCH),ARM968E-S)
$(NAME)_CFLAGS += -marm
endif

vcall ?= rhino

ifeq ($(vcall),freertos)
GLOBAL_DEFINES += VCALL_FREERTOS

$(NAME)_SOURCES += \
    aos/aos_freertos.c
endif

ifeq ($(vcall),posix)
GLOBAL_DEFINES += VCALL_POSIX

$(NAME)_SOURCES += \
    aos/aos_posix.c
endif

ifeq ($(vcall),rhino)
GLOBAL_DEFINES += VCALL_RHINO
$(NAME)_COMPONENTS += rhino

ifeq ($(HOST_MCU_FAMILY),esp32)
$(NAME)_COMPONENTS += vcall.espos
else
ifeq ($(HOST_MCU_FAMILY),esp8266)
$(NAME)_COMPONENTS += vcall.espos
endif
endif

ifneq (,$(filter $(PLATFORM), linuxhost mk3060 mk3239 mk3166))
$(NAME)_SOURCES += mico/mico_rhino.c
endif

$(NAME)_SOURCES += \
    aos/aos_rhino.c
endif

