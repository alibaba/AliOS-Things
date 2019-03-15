NAME := libiot_mal
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := mqtt adapter layer

$(NAME)_SOURCES := ./mal.c \
                   ./ica/mdal_ica_at_client.c

malica_test ?= 0
ifeq (1,$(malica_test))
$(NAME)_SOURCES += ./ica/test/mdal_ica_at_client_test.c
GLOBAL_DEFINES += MDAL_MAL_ICA_TEST
endif

$(NAME)_COMPONENTS := libiot_utils libiot_log libiot_system libiot_sdk_impl

$(NAME)_INCLUDES := . \
    middleware/linkkit/sdk-c/src/infra/system \

