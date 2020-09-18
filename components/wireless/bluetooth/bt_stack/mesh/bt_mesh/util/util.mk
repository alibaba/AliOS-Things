NAME := util

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := BLE Mesh util

$(NAME)_COMPONENTS := bluetooth.bt_mesh.ref_impl

$(NAME)_SOURCES := ./atomic_c.c \
                   ./buf.c \
                   ./log.c \
                   ./work.c \
                   ./queue.c \
                   ./poll.c

$(NAME)_INCLUDES := ./include

GLOBAL_INCLUDES += ./include \
                   ./port/include
