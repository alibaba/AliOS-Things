# component name
NAME := lcd
# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := Hardware abstract layer for lcd

# source files and the folder of internal include files
$(NAME)_INCLUDES += ./include
$(NAME)_SOURCES +=

GLOBAL_INCLUDES +=  ./include

ifeq ($(AOS_LCD_ILI9341),y)
$(NAME)_SOURCES += drv/ili9341.c
endif

ifeq ($(AOS_LCD_ST7789v),y)
$(NAME)_SOURCES += drv/st7789v.c
endif

GLOBAL_DEFINES += AOS_COMP_LCD


