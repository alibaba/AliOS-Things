NAME := cli

$(NAME)_TYPE := kernel
$(NAME)_MBINS_TYPE := kernel

$(NAME)_SOURCES := cli.c dumpsys.c

ifeq ($(COMPILER),armcc)
else ifeq ($(COMPILER),rvct)
else ifeq ($(COMPILER),iar)
else
$(NAME)_CFLAGS  += -Wall -Werror
endif

$(NAME)_COMPONENTS += hal

GLOBAL_INCLUDES += include
GLOBAL_DEFINES  += HAVE_NOT_ADVANCED_FORMATE CONFIG_AOS_CLI
