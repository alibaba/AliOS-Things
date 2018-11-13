NAME := prov_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := prov_app.c

$(NAME)_COMPONENTS := prov prov.test
$(NAME)_COMPONENTS += yloop cli

ifeq ($(ERASE), 1)
$(NAME)_DEFINES     += CONFIG_ERASE_KEY
endif

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./