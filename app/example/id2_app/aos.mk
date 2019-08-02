NAME := id2_client

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 2.0.0
$(NAME)_SUMMARY := ID2 Sample and Local Test.

$(NAME)_SOURCES := app_entry.c
$(NAME)_SOURCES += src/cJSON.c
$(NAME)_SOURCES += src/id2_client_mesg.c
$(NAME)_SOURCES += src/id2_client_test.c

$(NAME)_COMPONENTS  := id2 irot alicrypto

$(NAME)_INCLUDES += ./
$(NAME)_INCLUDES += ./inc
