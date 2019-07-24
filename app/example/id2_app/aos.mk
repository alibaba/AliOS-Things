NAME := id2_client

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := ID2 Sample and Local Test.

$(NAME)_SOURCES := app_entry.c
$(NAME)_SOURCES += id2_client_demo.c

$(NAME)_COMPONENTS  := id2 irot alicrypto

$(NAME)_INCLUDES += ./
