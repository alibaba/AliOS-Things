NAME := athostapp

$(NAME)_SOURCES := athostapp.c
$(NAME)_COMPONENTS := network/sal/athost \
                      network/netmgr \
                      tools/cli \
                      yloop

no_atparser ?= 0
ifneq (1,$(no_atparser))
$(NAME)_COMPONENTS += network/sal/atparser
endif

GLOBAL_DEFINES += DEBUG

$(NAME)_INCLUDES := ./

GLOBAL_INCLUDES += ./
