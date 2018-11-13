NAME := qr

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := qr.c decode.c identify.c quirc.c version_db.c qrcodegen.c

GLOBAL_DEFINES += AOS_NO_WIFI DK_CAMERA_SNAPSHOP CAMERA_GRAY_MIRROR

$(NAME)_COMPONENTS := yloop cli

ENABLE_CAMERA_HAL := 1

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./
