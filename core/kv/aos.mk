NAME := kv

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := lightweight key-value style storage module

$(NAME)_SOURCES := kv.c kv_adapt.c kv_cli.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

GLOBAL_INCLUDES += include

$(NAME)_SOURCES-$(KV_CONFIG_SECURE_SUPPORT) += kv_secure.c
$(NAME)_COMPONENTS-$(KV_CONFIG_SECURE_CRYPT_IMPL) += mbedtls

# used for certification_test, will removed soon
AOS_COMP_KV ?= y
GLOBAL_DEFINES  += AOS_COMP_KV
RPM_INCLUDE_DIR := fs
