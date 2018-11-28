NAME := profile

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := BLE commonly used profiles.

$(NAME)_SOURCES-y := bas.c cts.c cts.h dis.c hrs.c

GLOBAL_INCLUDES-y += .
