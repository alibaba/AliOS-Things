NAME := libiot_utils

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := tool sets, such as json,md5,base64 and so on

 
LINKKIT_MODULE  := middleware/linkkit/sdk-c/src/infra/utils

$(NAME)_SOURCES := $(wildcard $(SOURCE_ROOT)/$(LINKKIT_MODULE)/*.c)
$(NAME)_SOURCES += $(wildcard $(SOURCE_ROOT)/$(LINKKIT_MODULE)/*/*.c)

$(NAME)_SOURCES := $(foreach S,$($(NAME)_SOURCES),$(subst $(SOURCE_ROOT)/$(LINKKIT_MODULE),.,$(S)))

$(NAME)_INCLUDES := ../../../src/infra/log \
./misc \
./digest \

