NAME := hdlcclient

$(NAME)_SOURCES := hdlcclient.c

$(NAME)_COMPONENTS += yloop cli atparser netmgr

GLOBAL_DEFINES += DEBUG AOS_ATCMD

GLOBAL_INCLUDES += ./