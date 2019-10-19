NAME := websoc

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := websocket component

$(NAME)_INCLUDES += src

ifeq (y,$(WEBSOCKET_WITH_TLS))
 GLOBAL_DEFINES += WEBSOCKET_SSL_ENABLE
$(NAME)_COMPONENTS += mbedtls
endif

$(NAME)_SOURCES := src/rws_common.c \
                   src/rws_error.c \
                   src/rws_frame.c \
                   src/librws.c \
                   src/rws_list.c \
                   src/rws_memory.c \
                   src/rws_socketpriv.c \
                   src/rws_socketpub.c \
                   src/rws_string.c \
                   src/rws_thread.c
