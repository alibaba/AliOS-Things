NAME := fota

$(NAME)_TYPE := kernel
$(NAME)_CFLAGS += \
	-Wall \
	-Werror \
	-Wno-unused-function

$(NAME)_SOURCES += \
    ota_service.c \
    ota_util.c \
    ota_update_manifest.c \
    ota_version.c

$(NAME)_COMPONENTS += fota.platform fota.download  digest_algorithm 

$(NAME)_INCLUDES := \
    ./platform \
    ./download \
    ../../include/hal \

GLOBAL_INCLUDES += .

GLOBAL_DEFINES += AOS_FOTA
