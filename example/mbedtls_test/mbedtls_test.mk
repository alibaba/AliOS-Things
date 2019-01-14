NAME := mbedtls_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := test and demonstration program for utility/mbedtls.

$(NAME)_SOURCES := mbedtls_test.c
$(NAME)_SOURCES += tls_client.c

GLOBAL_DEFINES += CONFIG_MBEDTLS_TEST

$(NAME)_COMPONENTS := yloop cli utility/mbedtls netmgr

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./
