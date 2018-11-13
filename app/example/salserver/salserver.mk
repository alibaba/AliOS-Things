NAME := salserver

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
osal ?= posix

$(NAME)_SOURCES := salserver.c

$(NAME)_COMPONENTS += netmgr sal cli yloop
sal := 1

GLOBAL_DEFINES += DEBUG SAL_SERVER

GLOBAL_INCLUDES += ./