NAME := libiot_mal
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := mqtt adapter layer

$(NAME)_SOURCES := ./mal.c \
 
$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
middleware/linkkit/sdk-c/src/infra/log \
middleware/linkkit/sdk-c/src/infra/system \
middleware/linkkit/sdk-c/src/sdk-impl \

include middleware/linkkit/sdk-c/src/services/mdal/mal/ica/ica.mk

$(NAME)_INCLUDES := . \
    middleware/linkkit/sdk-c/src/infra/system \

