NAME := libiot_dm

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := Device Management


LINKKIT_MODULE  := middleware/linkkit/sdk-c/src/services/linkkit/dm

$(NAME)_SOURCES := $(wildcard $(SOURCE_ROOT)/$(LINKKIT_MODULE)/*.c)
$(NAME)_SOURCES += $(wildcard $(SOURCE_ROOT)/$(LINKKIT_MODULE)/*/*.c)

$(NAME)_SOURCES := $(foreach S,$($(NAME)_SOURCES),$(subst $(SOURCE_ROOT)/$(LINKKIT_MODULE),.,$(S)))

$(NAME)_COMPONENTS := libiot_utils libiot_log libiot_system

$(NAME)_INCLUDES := ./server \
./client \
../../../../src/infra \
../../../../src/infra/log \
../../../../src/infra/utils \
../../../../src/infra/utils/misc \
../../../../src/infra/utils/digest \
../../../../src/infra/system \
../../../../src/services/linkkit/cm \
../../../../src/services/linkkit/cm/include \
./example \

