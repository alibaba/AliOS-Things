NAME := xtensa

$(NAME)_SOURCES := port.c \
                   panic.c \
                   backtrace.c \
                   xtensa_panic.S \
                   xtensa_vectors.S

GLOBAL_INCLUDES += . ./include    \
                   ./include/frxt