NAME := knx

$(NAME)_TYPE := framework
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Lightweight KNX framework

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/gcc/knx.a
	
$(NAME)_COMPONENTS :=

GLOBAL_INCLUDES  += . include


GLOBAL_DEFINES  += AOS_KNX


