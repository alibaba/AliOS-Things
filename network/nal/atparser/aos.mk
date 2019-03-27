NAME := atparser

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := component for sending and receiving AT command over uart

GLOBAL_INCLUDES += ./

$(NAME)_SOURCES := atparser.c        \
                   hal_atparser_os.c \
                   hal_atparser_dev.c

GLOBAL_DEFINES += AOS_ATCMD
