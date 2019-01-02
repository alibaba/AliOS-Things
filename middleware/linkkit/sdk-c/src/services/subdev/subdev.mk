NAME := libiot_subdev

$(NAME)_SOURCES := ./iotx_subdev_api.c \
./iotx_subdev_common.c \
 
$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
middleware/linkkit/sdk-c/src/infra/log \
middleware/linkkit/sdk-c/src/infra/system \
middleware/linkkit/sdk-c/src/sdk-impl \

$(NAME)_INCLUDES := . \
    ../../../src/infra/system

