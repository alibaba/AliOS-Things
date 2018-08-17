NAME := hdlcserver

$(NAME)_SOURCES := hdlcserver.c

$(NAME)_COMPONENTS := yloop atparser

GLOBAL_DEFINES += AOS_ATCMD

GLOBAL_INCLUDES += ./
