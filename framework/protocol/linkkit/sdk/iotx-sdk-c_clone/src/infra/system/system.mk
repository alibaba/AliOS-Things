NAME := libiot_system

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY :=
# $(NAME)_SOURCES := ./guider.c \
# ./device.c \
# ./ca.c \
# ./report.c \
# ./aos_support.c \

LINKKIT_MODULE  := middleware/linkkit/sdk-c/src/infra/system

$(NAME)_SOURCES := $(wildcard $(SOURCE_ROOT)/$(LINKKIT_MODULE)/*.c)
$(NAME)_SOURCES += $(wildcard $(SOURCE_ROOT)/$(LINKKIT_MODULE)/*/*.c)

$(NAME)_SOURCES := $(foreach S,$($(NAME)_SOURCES),$(subst $(SOURCE_ROOT)/$(LINKKIT_MODULE),.,$(S)))
 
$(NAME)_COMPONENTS := 

$(NAME)_INCLUDES := ../../../src/infra/utils \
../../../src/infra/utils/misc \
../../../src/infra/utils/digest \
../../../src/infra/log \
../../../src/security/pro \
../../../src/security/pro/crypto \
../../../src/security/pro/id2 \
../../../src/security/pro/km \
../../../src/security/pro/itls \

