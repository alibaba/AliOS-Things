NAME := athost

$(NAME)_SOURCES := athost.c

$(NAME)_COMPONENTS := yloop atparser

GLOBAL_DEFINES += DEBUG AOS_ATCMD

GLOBAL_INCLUDES += ./

ifeq (1,$(athost))
GLOBAL_DEFINES += AT_HOST
endif