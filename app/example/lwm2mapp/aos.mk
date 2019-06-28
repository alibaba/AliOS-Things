NAME := lwm2mapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := lwm2m app
$(NAME)_SOURCES := lwm2m_example.c

#example client

LWM2M_EXAMPLE_PATH := .

GLOBAL_INCLUDES += $(LWM2M_EXAMPLE_PATH)/shared \
                   $(LWM2M_EXAMPLE_PATH)/client

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

$(NAME)_COMPONENTS += osal_aos lwm2m cli netmgr 

GLOBAL_DEFINES += AOS_NO_WIFI

GLOBAL_INCLUDES += ./
