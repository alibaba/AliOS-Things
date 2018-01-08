NAME := libsdk-impl

$(NAME)_SOURCES     := sdk-impl.c

ifeq ($(COMPILER),armcc)
else ifeq ($(COMPILER),iar)
else
$(NAME)_CFLAGS += \
    -Wno-unused-function \
    -Wno-implicit-function-declaration \
    -Wno-unused-function 
endif

$(NAME)_INCLUDES    := \
    ./ \
    ./imports \
    ./exports \
    ../LITE-utils \
    ../LITE-log \
    ../guider \
    ../iotx-system

$(NAME)_COMPONENTS  += iotx-utils.guider
