NAME := bt_profile

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := BLE commonly used profiles.

$(NAME)_SOURCES-y := bas.c cts.c dis.c hrs.c

$(NAME)_INCLUDES-y += .
