NAME := hdlcserver

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := hdlcserver.c

$(NAME)_COMPONENTS := yloop network.sal.atparser

GLOBAL_DEFINES += AOS_ATCMD

GLOBAL_INCLUDES += ./

GLOBAL_INCLUDES += ../
