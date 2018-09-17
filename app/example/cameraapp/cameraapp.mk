NAME := cameraapp

$(NAME)_SOURCES := cameraapp.c

GLOBAL_DEFINES += AOS_NO_WIFI CONFIG_AOS_FATFS_SUPPORT_MMC

$(NAME)_COMPONENTS := yloop cli board.developerkit.camera_hal
$(NAME)_COMPONENTS += 3rdparty.experimental.fs.fatfs

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./