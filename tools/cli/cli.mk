NAME := cli

$(NAME)_TYPE := kernel

$(NAME)_SOURCES := cli.c dumpsys.c
$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration

GLOBAL_INCLUDES += include
GLOBAL_DEFINES  += HAVE_NOT_ADVANCED_FORMATE CONFIG_AOS_CLI
