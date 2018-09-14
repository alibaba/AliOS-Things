NAME := syscall

$(NAME)_MBINS_TYPE := app

ifeq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := syscall.c
endif

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := yloop cli

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./
