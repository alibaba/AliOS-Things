NAME := dk_gui

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := Developer Kit GUI example
$(NAME)_SOURCES := developerkitgui.c sensor_display.c freetype_display.c AliOS_Things_logo.c

$(NAME)_COMPONENTS := yloop cli freetype253 fatfs

ENABLE_IRDA_HAL := 1

ENABLE_CAMERA_HAL := 1

$(NAME)_INCLUDES    += .

$(NAME)_COMPONENTS += littlevGL

GLOBAL_DEFINES += LITTLEVGL_DEVELOPERKIT CONFIG_AOS_FATFS_SUPPORT_MMC
