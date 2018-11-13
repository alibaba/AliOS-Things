NAME := acapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := acapp.c

$(NAME)_COMPONENTS  += activation yloop cli

GLOBAL_INCLUDES += ./