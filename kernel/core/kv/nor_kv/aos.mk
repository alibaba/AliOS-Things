NAME := nor_kv

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := lightweight key-value style storage module for NOR Flash

$(NAME)_SOURCES := kv.c kv_adapt.c kv_cli.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall
endif

$(NAME)_INCLUDES += include


ifeq ($(KV_CONFIG_SECURE_SUPPORT), y)
$(NAME)_SOURCES += kv_secure.c
ifeq ($(KV_CONFIG_SECURE_CRYPT_IMPL), 1)
$(NAME)_COMPONENTS += mbedtls
endif
endif

