NAME := cli

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := Command Line Interface

$(NAME)_SOURCES := cli.c         \
                   cli_adapt.c   \
                   cli_console.c \
                   uart_console.c

$(NAME)_SOURCES-$(AOS_COMP_LWIP) += telnet_console.c

GLOBAL_INCLUDES += include
GLOBAL_INCLUDES += ../../../component/include
