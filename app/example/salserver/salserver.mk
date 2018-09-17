NAME := salserver

osal ?= posix

$(NAME)_SOURCES := salserver.c

$(NAME)_COMPONENTS += netmgr sal cli yloop
sal := 1

GLOBAL_DEFINES += DEBUG SAL_SERVER

GLOBAL_INCLUDES += ./