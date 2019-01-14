NAME := linkvoice

$(NAME)_TYPE := framework

$(NAME)_COMPONENTS :=  imbedtls alicrypto connectivity/websockets  cjson protocol.alink ywss netmgr

ifeq ($(HOST_ARCH), Cortex-M4)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/linkvoice.a

else ifeq ($(HOST_ARCH), linux)
PLATFORM := linuxhost
$(NAME)_PREBUILT_LIBRARY := lib/$(PLATFORM)/linkvoice.a

else
$(error "do not find correct platform!")
endif


GLOBAL_INCLUDES     +=inc 
#$(NAME)_INCLUDES  :=asr  ca    core

$(NAME)_DEFINES  +=PAL_DEBUG



