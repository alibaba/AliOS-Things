# component name
NAME := oled
# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := oled device

# source files and the folder of internal include files
$(NAME)_INCLUDES +=  ./include
$(NAME)_SOURCES +=

GLOBAL_INCLUDES +=  ./include

ifeq ($(AOS_OLED_SH1106),y)
$(NAME)_SOURCES += drv/sh1106.c
endif


