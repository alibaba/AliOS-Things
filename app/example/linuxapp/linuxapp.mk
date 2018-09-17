NAME := linuxapp

$(NAME)_SOURCES    := main.c

$(NAME)_COMPONENTS := cli yloop

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS += network.lwip
endif

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS += benchmarks
GLOBAL_DEFINES     += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./