NAME := libguider

GLOBAL_INCLUDES += \
    ./ \
    ../iotx-system \
    ../LITE-utils \
    ../LITE-log \
    ../misc \
    ../digest \
    ../sdk-impl \
    ../../security/tfs
# don't modify to L_CFLAGS, because CONFIG_CJSON_WITHOUT_DOUBLE should enable global

$(NAME)_SOURCES := \
    guider.c

$(NAME)_COMPONENTS += iotx-utils.iotx-system
