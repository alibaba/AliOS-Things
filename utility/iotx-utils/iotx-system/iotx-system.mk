NAME := libiotx-system

GLOBAL_INCLUDES += \
    ./ \
# don't modify to L_CFLAGS, because CONFIG_CJSON_WITHOUT_DOUBLE should enable global

$(NAME)_SOURCES := \
    device.c \
    ca.c
