NAME := libsdk-impl

$(NAME)_SOURCES     := sdk-impl.c

$(NAME)_CFLAGS += \
    -Wno-unused-function \
    -Wno-implicit-function-declaration \
    -Wno-unused-function \
#    -Werror

$(NAME)_INCLUDES    := \
    ./ \
    ./imports \
    ./exports \
    ../LITE-utils \
    ../LITE-log \
    ../guider \
    ../device

$(NAME)_COMPONENTS  += iotx-utils.guider
