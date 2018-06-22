NAME := athost

$(NAME)_SOURCES := athost.c

$(NAME)_COMPONENTS := yloop atparser cli

GLOBAL_DEFINES += DEBUG AOS_ATCMD AT_HOST

GLOBAL_INCLUDES += ./
