NAME := meshapp
no_with_lwip := 0
$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := uMesh sample application

$(NAME)_SOURCES     := main.c

$(NAME)_COMPONENTS  += umesh cli netmgr
GLOBAL_DEFINES      += TAPIF_DEFAULT_OFF DEBUG

ifdef $(MESHAPP_CONFIG_MESHAUTH)
MESHAUTH ?= $(MESHAPP_CONFIG_MESHAUTH)
else
MESHAUTH ?= 0
endif

ifdef $(MESHAPP_CONFIG_IPV6)
ipv6 ?= $(MESHAPP_CONFIG_IPV6)
else
ipv6 ?= 0
endif

ifdef $(MESHAPP_CONFIG_HAL_TEST)
mesh_hal_test ?= $(MESHAPP_CONFIG_HAL_TEST)
else
mesh_hal_test ?= 0
endif

ifeq ($(MESHAUTH), 1)
$(NAME)_COMPONENTS  += base64 alicrypto id2
endif

LWIP ?=1
ifeq ($(LWIP), 1)
$(NAME)_COMPONENTS  += lwip
endif


ifeq ($(ipv6),0)
GLOBAL_DEFINES += LWIP_IPV6=0
endif

ifneq (,$(filter linuxhost,$(COMPONENTS)))
DDA ?= 1
endif

ifneq (,$(filter armhflinux,$(COMPONENTS)))
DDA ?= 1
endif

ifeq ($(DDA),1)
GLOBAL_LDFLAGS += -lreadline -lncurses
$(NAME)_COMPONENTS  += dda
endif

ifneq (,$(filter EMW3060,$(MODULE)))
GLOBAL_DEFINES += MESHAPP_LIGHT_ENABLED
endif

ifeq ($(mesh_hal_test),1)
GLOBAL_DEFINES += MESH_HAL_TEST_ENABLED
endif

GLOBAL_INCLUDES += ./
