NAME := kv

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := lightweight key-value style storage module

$(NAME)_SOURCES := kv.c kv_adapt.c kv_cli.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

$(NAME)_INCLUDES += include
GLOBAL_DEFINES  += AOS_COMP_KV

KV_CONFIG_SECURE_SUPPORT ?= 0

ifeq ($(KV_CONFIG_SECURE_SUPPORT), 1)

KV_CONFIG_SECURE_CRYPT_IMPL ?= 0

$(NAME)_SOURCES += kv_secure.c
$(NAME)_DEFINES += KV_CONFIG_SECURE_SUPPORT=1

ifeq ($(KV_CONFIG_SECURE_CRYPT_IMPL), 1)

$(NAME)_COMPONENTS += mbedtls
$(NAME)_DEFINES += KV_CONFIG_SECURE_CRYPT_IMPL=1

endif

endif

AOS_COMP_KV ?= y
