NAME := hdlcserver

$(NAME)_SOURCES := hdlcserver.c

$(NAME)_COMPONENTS := yloop atparser

GLOBAL_DEFINES += DEBUG AOS_ATCMD

GLOBAL_INCLUDES += ./
