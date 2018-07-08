NAME := networkapp

$(NAME)_SOURCES := networkapp.c

$(NAME)_COMPONENTS := yloop cli netmgr

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

