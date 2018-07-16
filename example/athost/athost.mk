NAME := athost

$(NAME)_SOURCES := athost.c

$(NAME)_COMPONENTS := yloop atparser cli

GLOBAL_DEFINES += AOS_ATCMD

GLOBAL_INCLUDES += ./
