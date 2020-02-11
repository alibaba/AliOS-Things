NAME := websoc

# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := websocket component

# source files and the folder of internal include files
$(NAME)_INCLUDES += src

$(NAME)_SOURCES := src/rws_error.c \
                   src/rws_frame.c \
                   src/rws_list.c \
                   src/rws_memory.c \
                   src/rws_socketpriv.c \
                   src/rws_socketpub.c \
                   src/rws_string.c \
                   src/rws_thread_aos.c

# the folder of API files
GLOBAL_INCLUDES += ../../../include/network/websoc

# mandatory dependencies

# optional dependencies
$(NAME)_COMPONENTS-$(WEBSOC_WITH_TLS) += mbedtls
