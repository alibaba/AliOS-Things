NAME := libiot_log

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := Log printing implementation

$(NAME)_SOURCES := ./iotx_log.c \
 
$(NAME)_INCLUDES := ../../../src/infra/utils \
../../../src/infra/utils/misc \
../../../src/infra/utils/digest \

