NAME := blink

$(NAME)_SOURCES := blink.c

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := yloop cli
