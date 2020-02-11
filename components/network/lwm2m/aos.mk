NAME := lwm2m

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := lwm2m component

# source files and the folder of internal include files
LWM2M_SRC_PATH := ./src/wakaama-1.0
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

LWM2M_CLIENT_SRC_PATH = ./src/ref-impl/client
$(NAME)_SOURCES += $(LWM2M_CLIENT_SRC_PATH)/object_server.c   \
                   $(LWM2M_CLIENT_SRC_PATH)/object_security.c \
                   $(LWM2M_CLIENT_SRC_PATH)/object_firmware.c \
                   $(LWM2M_CLIENT_SRC_PATH)/object_location.c \
                   $(LWM2M_CLIENT_SRC_PATH)/object_device.c   \
                   $(LWM2M_CLIENT_SRC_PATH)/lwm2mclient.c

LWM2M_SHARED_SRC_PATH = ./src/ref-impl/shared
$(NAME)_SOURCES += $(LWM2M_SHARED_SRC_PATH)/platform.c \
                   $(LWM2M_SHARED_SRC_PATH)/mbedconnection.c

$(NAME)_INCLUDES += ./include \
                    $(LWM2M_CLIENT_SRC_PATH)

# the folder of API files
GLOBAL_INCLUDES += ../../../include/network/lwm2m \
                   $(LWM2M_SHARED_SRC_PATH)

# mandatory dependencies
$(NAME)_COMPONENTS += libcoap

