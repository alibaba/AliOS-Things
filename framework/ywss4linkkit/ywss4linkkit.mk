NAME := ywss4linkkit

$(NAME)_TYPE := framework
GLOBAL_INCLUDES += .
GLOBAL_DEFINES += CONFIG_YWSS

ifeq ($(COMPILER),armcc)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/keil/ywss4linkkit.a
else ifeq ($(COMPILER),iar)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/iar/ywss4linkkit.a
else
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/gcc/ywss4linkkit.a
endif
