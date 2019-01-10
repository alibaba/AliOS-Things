NAME := ota_hal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := ota porting HAL APIs

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_SOURCES := \
    ota_hal_module.c \
    ota_hal_os.c \

ifneq (,$(filter mcu_linuximpl,$(HOST_MCU_FAMILY)))
$(NAME)_SOURCES += \
    ota_hal_host.c
else
$(NAME)_SOURCES += \
    ota_hal_plat.c
endif

GLOBAL_INCLUDES += . \
    ../2nd_boot
