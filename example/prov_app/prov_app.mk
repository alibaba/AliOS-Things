NAME := prov_app

$(NAME)_SOURCES := prov_app.c

$(NAME)_COMPONENTS := prov prov.test

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

