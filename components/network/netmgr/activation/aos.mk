NAME := activation

ifeq (y,$(AOS_COMP_ACTIVATION_IN_SERVICE))
$(NAME)_MBINS_TYPE := share
else
$(NAME)_MBINS_TYPE := kernel
endif
$(NAME)_VERSION := 1.0.4

$(NAME)_SUMMARY := for device activation

$(NAME)_COMPONENTS +=
GLOBAL_INCLUDES-y += .
$(NAME)_SOURCES := activation.c
