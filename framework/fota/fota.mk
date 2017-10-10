NAME := fota

$(NAME)_CFLAGS += \
	-Wall \
	-Werror \
	-Wno-unused-function

$(NAME)_SOURCES += \
    ota_service.c \
    ota_util.c \
    ota_update_manifest.c \
    ota_download.c \
    ota_version.c

$(NAME)_COMPONENTS += fota.platform digest_algorithm fota.socket

$(NAME)_INCLUDES := \
    ./platform \
    ./socket \
    ../../include/hal \

GLOBAL_INCLUDES += .

GLOBAL_DEFINES += AOS_FOTA
