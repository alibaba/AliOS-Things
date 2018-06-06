NAME := ywss

$(NAME)_TYPE := framework
GLOBAL_INCLUDES += .
GLOBAL_DEFINES += CONFIG_YWSS

ifeq ($(COMPILER),armcc)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/keil/ywss.a
else ifeq ($(COMPILER),iar)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/iar/ywss.a
else
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/gcc/ywss.a
endif
