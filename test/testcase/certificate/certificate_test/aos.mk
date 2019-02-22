NAME := certificate_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := testcase for certificate
$(NAME)_SOURCES    := aos_test.c cutest/cut.c
$(NAME)_COMPONENTS := rhino
$(NAME)_COMPONENTS += kv

#Prevalidate Mode
ifeq ($(certificate_level), pv)
$(NAME)_DEFINES += TEST_CONFIG_MALLOC_FREE_TIMES=1000
$(NAME)_DEFINES += TEST_CONFIG_CREATE_TASK_TIMES=100
$(NAME)_DEFINES += TEST_CONFIG_SYNC_TIMES=1000
$(NAME)_DEFINES += TEST_CONFIG_KV_ENABLED=0
endif

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-varible
