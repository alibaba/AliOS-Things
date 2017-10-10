NAME := atparser

with_atparser ?= 1

GLOBAL_INCLUDES += ./

$(NAME)_SOURCES := atparser.c

GLOBAL_DEFINES += AOS_ATCMD
