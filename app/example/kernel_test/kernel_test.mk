NAME := kernel_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := kernel_test.c

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := cli log test/testcase/certificate_test

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./
