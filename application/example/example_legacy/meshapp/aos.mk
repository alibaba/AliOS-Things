NAME := meshapp
no_with_lwip := 0
$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := uMesh sample application

$(NAME)_SOURCES     := main.c

$(NAME)_COMPONENTS  += umesh cli netmgr
GLOBAL_DEFINES      += TAPIF_DEFAULT_OFF DEBUG

ifeq ($(MESHAPP_CONFIG_MESHAUTH), y)
$(NAME)_COMPONENTS  += alicrypto id2
endif

MESHAPP_CONFIG_USE_LWIP ?=y
ifeq ($(MESHAPP_CONFIG_USE_LWIP), y)
$(NAME)_COMPONENTS  += lwip
endif

ifeq ($(MESHAPP_CONFIG_IPV6),n)
GLOBAL_DEFINES += LWIP_IPV6=0
endif

$(NAME)_INCLUDES += ./
