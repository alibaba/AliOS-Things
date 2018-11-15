NAME := libiot_mal
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := mqtt adapter layer

$(NAME)_SOURCES := ./mal.c \
 
include middleware/linkkit/sdk-c/src/infra/utils/utils.mk
include middleware/linkkit/sdk-c/src/infra/log/log.mk
include middleware/linkkit/sdk-c/src/infra/system/system.mk
include middleware/linkkit/sdk-c/src/sdk-impl/sdk-impl.mk
include middleware/linkkit/sdk-c/src/services/mdal/mal/ica/ica.mk

$(NAME)_INCLUDES := . \
    middleware/linkkit/sdk-c/src/infra/system \

