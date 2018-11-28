NAME := libiot_mal
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := mqtt adapter layer

$(NAME)_SOURCES := ./mal.c \
 
include middleware/linkkit/sdk-c/src/infra/utils/aos.mk
include middleware/linkkit/sdk-c/src/infra/log/aos.mk
include middleware/linkkit/sdk-c/src/infra/system/aos.mk
include middleware/linkkit/sdk-c/src/sdk-impl/aos.mk
include middleware/linkkit/sdk-c/src/services/mdal/mal/ica/aos.mk

$(NAME)_INCLUDES := . \
    middleware/linkkit/sdk-c/src/infra/system \

