NAME := ota_verify

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Verify OTA firmware with Hash or RSA.

$(NAME)_SOURCES := ota_sign.c \
                   ota_hash.c

GLOBAL_INCLUDES += . ../../inc ../../hal ../verify
