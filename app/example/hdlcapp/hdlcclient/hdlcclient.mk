NAME := hdlcclient

$(NAME)_SOURCES := hdlcclient.c

$(NAME)_COMPONENTS += yloop cli network.sal.atparser netmgr

GLOBAL_DEFINES += AOS_ATCMD

GLOBAL_INCLUDES += ./

GLOBAL_INCLUDES += ../
