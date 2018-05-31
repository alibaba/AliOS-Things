NAME := libywss

$(NAME)_TYPE := framework
GLOBAL_DEFINES += CONFIG_YWSS

ifeq ($(HOST_ARCH), linux)
LIB_PATH := linux
else ifeq ($(HOST_ARCH), ARM968E-S)
LIB_PATH := arm968es
else ifeq ($(HOST_ARCH), xtensa)
ifeq ($(HOST_MCU_FAMILY), esp32)
LIB_PATH := xtensa/esp32
else ifeq ($(HOST_MCU_FAMILY), esp8266)
LIB_PATH := xtensa/esp8266
endif
else ifeq ($(HOST_ARCH), Cortex-M4)
ifeq ($(ENABLE_VFP), 1)
LIB_PATH := cortex-m4/vfp
else
LIB_PATH := cortex-m4
endif
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
