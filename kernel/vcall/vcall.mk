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

ifneq ($(HOST_MCU_FAMILY),esp32)
$(NAME)_SOURCES := \
    mico/mico_rhino.c
else
$(NAME)_COMPONENTS += vcall.espos
endif

$(NAME)_SOURCES += \
    aos/aos_rhino.c
endif

