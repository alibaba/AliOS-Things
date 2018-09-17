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
    ota_hal_os.c

GLOBAL_INCLUDES += . \
                 ../inc \
                 ../src/utility/crc
