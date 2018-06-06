NAME := salserver

vcall ?= posix

$(NAME)_SOURCES := salserver.c

$(NAME)_COMPONENTS += netmgr sal cli yloop
sal := 1

GLOBAL_DEFINES += DEBUG SAL_SERVER
