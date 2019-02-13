NAME := mesh_util

$(NAME)_COMPONENTS := bluetooth.bt_mesh.ref-impl

$(NAME)_SOURCES := ./atomic_c.c \
                   ./buf.c \
                   ./log.c \
                   ./work.c \
                   ./queue.c \
                   ./poll.c

$(NAME)_INCLUDES := ./include

GLOBAL_INCLUDES += ./include \
                   ./port/include \
