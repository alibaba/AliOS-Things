NAME := mesh_util

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := BLE Mesh util

$(NAME)_COMPONENTS := mesh_hal_ref_impl

$(NAME)_SOURCES := ./atomic_c.c \
                   ./buf.c \
                   ./log.c \
                   ./work.c \
                   ./queue.c \
                   ./poll.c

$(NAME)_INCLUDES += ./include \
                    ./port/include
