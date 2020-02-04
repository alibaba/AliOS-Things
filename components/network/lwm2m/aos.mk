NAME := lwm2m

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := lwm2m component

GLOBAL_DEFINES-y += LWM2M_LITTLE_ENDIAN \
                    LWM2M_CLIENT_MODE

$(NAME)_INCLUDES += ./include \

LWM2M_SRC_PATH := ./src/wakaama-1.0

LWM2M_CLIENT_SRC_PATH = ./src/ref-impl/client

LWM2M_SHARED_SRC_PATH = ./src/ref-impl/shared

ifeq (y,$(LWM2M_BOOTSTRAP_CLIENT_SUPPORT))
GLOBAL_DEFINES += LWM2M_BOOTSTRAP
endif

$(NAME)_SOURCES := $(LWM2M_SRC_PATH)/block1.c \
                   $(LWM2M_SRC_PATH)/discover.c \
                   $(LWM2M_SRC_PATH)/json.c \
                   $(LWM2M_SRC_PATH)/list.c \
                   $(LWM2M_SRC_PATH)/observe.c \
                   $(LWM2M_SRC_PATH)/tlv.c \
                   $(LWM2M_SRC_PATH)/utils.c \
                   $(LWM2M_SRC_PATH)/bootstrap.c \
                   $(LWM2M_SRC_PATH)/liblwm2m.c \
                   $(LWM2M_SRC_PATH)/management.c \
                   $(LWM2M_SRC_PATH)/packet.c \
                   $(LWM2M_SRC_PATH)/transaction.c \
                   $(LWM2M_SRC_PATH)/data.c \
                   $(LWM2M_SRC_PATH)/objects.c \
                   $(LWM2M_SRC_PATH)/registration.c \
                   $(LWM2M_SRC_PATH)/uri.c


$(NAME)_SOURCES += $(LWM2M_CLIENT_SRC_PATH)/object_server.c   \
                   $(LWM2M_CLIENT_SRC_PATH)/object_security.c \
                   $(LWM2M_CLIENT_SRC_PATH)/object_firmware.c \
                   $(LWM2M_CLIENT_SRC_PATH)/object_location.c \
                   $(LWM2M_CLIENT_SRC_PATH)/object_device.c   \
                   $(LWM2M_CLIENT_SRC_PATH)/lwm2mclient.c


$(NAME)_SOURCES += $(LWM2M_SHARED_SRC_PATH)/platform.c \
                   $(LWM2M_SHARED_SRC_PATH)/mbedconnection.c

GLOBAL_INCLUDES += $(LWM2M_SHARED_SRC_PATH)

$(NAME)_INCLUDES += $(LWM2M_CLIENT_SRC_PATH)


$(NAME)_COMPONENTS += libcoap
GLOBAL_DEFINES  += LWM2M_LIBCOAP_ENABLED
