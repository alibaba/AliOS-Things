NAME := hdlcserver

$(NAME)_SOURCES := hdlcserver.c

$(NAME)_COMPONENTS := yloop network.sal.atparser

GLOBAL_DEFINES += AOS_ATCMD

GLOBAL_INCLUDES += ./

GLOBAL_INCLUDES += ../
