NAME := dk_camera

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := developer kit camera demo
$(NAME)_SOURCES := cameraapp.c

GLOBAL_DEFINES += AOS_NO_WIFI CONFIG_AOS_FATFS_SUPPORT_MMC

$(NAME)_COMPONENTS := yloop cli
$(NAME)_COMPONENTS += 3rdparty.experimental.fs.fatfs

ENABLE_CAMERA_HAL := 1

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./
