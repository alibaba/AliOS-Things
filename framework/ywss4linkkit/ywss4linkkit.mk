NAME := libywss

$(NAME)_TYPE := framework

GLOBAL_INCLUDES += ./

ifeq ($(HOST_ARCH), linux)
LIB_PATH := linux
else ifeq ($(HOST_ARCH), ARM968E-S)
LIB_PATH := arm968es
else ifeq ($(HOST_ARCH), xtensa)
LIB_PATH := xtensa
else ifeq ($(HOST_ARCH), Cortex-M4)
LIB_PATH := cortex-m4
else
$(error "not find correct platform!")
endif

ifeq ($(COMPILER),armcc)
$(NAME)_PREBUILT_LIBRARY := lib/$(LIB_PATH)/KEIL/libywss.a
else ifeq ($(COMPILER),iar)
$(NAME)_PREBUILT_LIBRARY := lib/$(LIB_PATH)/IAR/libywss.a
else
$(NAME)_PREBUILT_LIBRARY := lib/$(LIB_PATH)/libywss.a
endif
