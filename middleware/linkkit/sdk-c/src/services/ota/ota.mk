NAME := libiot_ota

LINKKIT_MODULE  := middleware/linkkit/sdk-c/src/services/ota

$(NAME)_SOURCES := $(wildcard $(SOURCE_ROOT)/$(LINKKIT_MODULE)/*.c)
$(NAME)_SOURCES += $(wildcard $(SOURCE_ROOT)/$(LINKKIT_MODULE)/impl/*.c)

$(NAME)_SOURCES := $(foreach S,$($(NAME)_SOURCES),$(subst $(SOURCE_ROOT)/$(LINKKIT_MODULE),.,$(S)))

$(NAME)_INCLUDES := .

$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
                      middleware/linkkit/sdk-c/src/infra/log \
                      middleware/linkkit/sdk-c/src/infra/system \
                      middleware/linkkit/sdk-c/src/services/ota \
