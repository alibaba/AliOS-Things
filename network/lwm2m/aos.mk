NAME := lwm2m

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := lwm2m component

GLOBAL_DEFINES-y += LWM2M_LITTLE_ENDIAN \
                  LWM2M_CLIENT_MODE

GLOBAL_INCLUDES += ./wakaama-1.0/core \
                   ./wakaama-1.0/examples/shared \
                   ./wakaama-1.0/examples/client

LWM2M_SRC_PATH := ./wakaama-1.0/core

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


$(NAME)_COMPONENTS += libcoap
GLOBAL_DEFINES  += LWM2M_LIBCOAP_ENABLED

#example client
LWM2M_EXAMPLE_PATH := ./wakaama-1.0/examples

$(NAME)_SOURCES += $(LWM2M_EXAMPLE_PATH)/client/object_access_control.c \
                   $(LWM2M_EXAMPLE_PATH)/client/object_device.c \
                   $(LWM2M_EXAMPLE_PATH)/client/object_security.c \
                   $(LWM2M_EXAMPLE_PATH)/client/lwm2mclient.c \
                   $(LWM2M_EXAMPLE_PATH)/client/object_connectivity_moni.c \
                   $(LWM2M_EXAMPLE_PATH)/client/object_firmware.c \
                   $(LWM2M_EXAMPLE_PATH)/client/object_server.c \
                   $(LWM2M_EXAMPLE_PATH)/client/object_connectivity_stat.c \
                   $(LWM2M_EXAMPLE_PATH)/client/object_location.c \
                   $(LWM2M_EXAMPLE_PATH)/client/system_api.c \
                   $(LWM2M_EXAMPLE_PATH)/client/test_object.c \
                   $(LWM2M_EXAMPLE_PATH)/shared/platform.c \
                   $(LWM2M_EXAMPLE_PATH)/shared/commandline.c \
                   $(LWM2M_EXAMPLE_PATH)/shared/memtrace.c

ifeq (y,$(LWM2M_WITH_DTLS))
$(NAME)_COMPONENTS += mbedtls
GLOBAL_DEFINES-y += USE_EXTERNAL_MEBDTLS WITH_MBEDTLS LWM2M_WITH_MBEDTLS
$(NAME)_SOURCES += $(LWM2M_EXAMPLE_PATH)/shared/mbedconnection.c
else
$(NAME)_SOURCES += $(LWM2M_EXAMPLE_PATH)/shared/connection.c
endif

#end example client
