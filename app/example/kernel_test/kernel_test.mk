NAME := kernel_test

$(NAME)_SOURCES := kernel_test.c

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := cli kernel/rhino/test

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./
