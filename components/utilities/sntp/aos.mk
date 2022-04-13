NAME := sntp

ifeq (y,$(AOS_COMP_SNTP_IN_SERVICE))
$(NAME)_MBINS_TYPE := share
else
$(NAME)_MBINS_TYPE := kernel
endif

$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := sntp component

$(NAME)_SOURCES := sntp.c
