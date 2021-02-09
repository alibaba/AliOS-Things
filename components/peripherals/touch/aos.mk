# component name
NAME := touch
# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Hardware abstract layer for touchpad

# source files and the folder of internal include files
$(NAME)_INCLUDES +=  ./include
$(NAME)_SOURCES +=

ifeq ($(AOS_TOUCH_FT6336),y)
$(NAME)_SOURCES += drv/ft6336.c
endif