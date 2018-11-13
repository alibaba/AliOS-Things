NAME := hdlcclient

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := hdlcclient.c

$(NAME)_COMPONENTS += yloop cli network.sal.atparser netmgr

GLOBAL_DEFINES += AOS_ATCMD

GLOBAL_INCLUDES += ./

GLOBAL_INCLUDES += ../
