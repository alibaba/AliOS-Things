NAME := netmgrapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := netmgrapp.c

$(NAME)_COMPONENTS += netmgr yloop cli

GLOBAL_INCLUDES += ./