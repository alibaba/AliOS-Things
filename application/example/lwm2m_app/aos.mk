NAME := lwm2m_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := lwm2m app
$(NAME)_SOURCES := lwm2m_app.c \
                   app_entry.c

$(NAME)_COMPONENTS += osal_aos lwm2m cli netmgr 

ifeq (y,$(LWM2M_WITH_DTLS))
$(NAME)_COMPONENTS += mbedtls
GLOBAL_DEFINES-y += USE_EXTERNAL_MEBDTLS WITH_MBEDTLS LWM2M_WITH_MBEDTLS
endif
