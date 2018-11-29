NAME := hashtable

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := The hashtable provides friendly APIs to create, add, find hash table.

$(NAME)_SOURCES := hashtable.c
$(NAME)_INCLUDES := ../../middleware/alink/protocol/os/
GLOBAL_INCLUDES += .
