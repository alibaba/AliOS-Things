NAME := lwm2m_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := lwm2m app
$(NAME)_SOURCES := lwm2m_app.c \
                   app_entry.c \
                   maintask.c

$(NAME)_COMPONENTS-y += osal_aos lwm2m cli netmgr 
$(NAME)_COMPONENTS-$(LWM2M_WITH_DTLS) += mbedtls
