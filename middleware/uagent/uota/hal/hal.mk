NAME := ota_hal

$(NAME)_TYPE := uota

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_SOURCES := \
    ota_hal_module.c \
    ota_hal_os.c \

ifneq (,$(filter linux,$(HOST_MCU_FAMILY)))
$(NAME)_SOURCES += \
    ota_hal_host.c
else
$(NAME)_SOURCES += \
    ota_hal_plat.c
endif

GLOBAL_INCLUDES += . \
    ../inc \
    ../src/verify/crc \
    ../src/2nd_boot \
