NAME := footprint_test

$(NAME)_SOURCES := footprint_test.c

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := cli

GLOBAL_INCLUDES += ./