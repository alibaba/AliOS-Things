NAME := id2_client

$(NAME)_SOURCES := app_entry.c
$(NAME)_SOURCES += id2_client_demo.c

$(NAME)_COMPONENTS  := id2 irot alicrypto

GLOBAL_INCLUDES += ./
