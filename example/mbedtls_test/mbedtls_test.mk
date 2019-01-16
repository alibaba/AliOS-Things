NAME := mbedtls_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := test and demonstration program for mbedtls.

ifeq ($(HOST_ARCH),linux)
CONFIG_MBEDTLS_TEST_TLS := y
else
ifeq ($(with_lwip),1)
CONFIG_MBEDTLS_TEST_TLS := y
endif
endif

$(NAME)_SOURCES := mbedtls_test.c

ifeq ($(CONFIG_MBEDTLS_TEST_TLS),y)
$(NAME)_DEFINES += CONFIG_MBEDTLS_TEST_TLS
$(NAME)_SOURCES += tls_client.c
endif

GLOBAL_DEFINES += CONFIG_MBEDTLS_TEST

$(NAME)_COMPONENTS += cli mbedtls yloop netmgr

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./
