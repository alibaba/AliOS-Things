NAME := netmgrapp

$(NAME)_SOURCES := netmgrapp.c

$(NAME)_COMPONENTS += netmgr yloop cli

GLOBAL_INCLUDES += ./