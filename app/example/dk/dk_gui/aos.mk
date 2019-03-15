NAME := dk_gui

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Developer Kit GUI example
$(NAME)_SOURCES := developerkitgui.c AliOS_Things_logo.c sensor_display.c house.c weather.c

$(NAME)_COMPONENTS := yloop cli

ENABLE_IRDA_HAL := 1

ENABLE_CAMERA_HAL := 1

$(NAME)_INCLUDES    += .

$(NAME)_COMPONENTS += littlevGL

GLOBAL_DEFINES += LITTLEVGL_DEVELOPERKIT

GLOBAL_INCLUDES += ./
