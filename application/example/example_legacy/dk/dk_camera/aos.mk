NAME := dk_camera

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := developer kit camera demo
$(NAME)_SOURCES := cameraapp.c

GLOBAL_DEFINES += AOS_NO_WIFI CONFIG_AOS_FATFS_SUPPORT_MMC

$(NAME)_COMPONENTS := yloop cli
$(NAME)_COMPONENTS += fatfs

ENABLE_CAMERA_HAL := 1

$(NAME)_INCLUDES += ./
