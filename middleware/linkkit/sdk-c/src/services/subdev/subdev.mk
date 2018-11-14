NAME := libiot_subdev

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY :=

$(NAME)_SOURCES := ./iotx_subdev_api.c \
./iotx_subdev_common.c \
 
$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
middleware/linkkit/sdk-c/src/infra/log \
middleware/linkkit/sdk-c/src/infra/system \
middleware/linkkit/sdk-c/src/sdk-impl \

$(NAME)_INCLUDES := . \
    ../../../src/infra/system

