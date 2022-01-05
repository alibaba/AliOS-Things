#
# Component Makefile
#

# DuerOS LightDuer Module
LIGHTDUER_PATH := dueros/lightduer

COMPONENT_ADD_INCLUDEDIRS += $(LIGHTDUER_PATH)/include

COMPONENT_SRCDIRS :=

ifdef IDF_VERSION_MAJOR
ifneq ($(IDF_VERSION_MAJOR),3)
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/$(LIGHTDUER_PATH) -lduer-device-v4x
else
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/$(LIGHTDUER_PATH) -lduer-device
endif
else
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/$(LIGHTDUER_PATH) -lduer-device
endif