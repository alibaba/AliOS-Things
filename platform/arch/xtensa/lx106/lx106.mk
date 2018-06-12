NAME := xtensa

$(NAME)_SOURCES := port.c \
                   panic.c \
                   xtensa_panic.S \
                   xtensa_vectors.S

GLOBAL_INCLUDES += . ./include    \
                   ./include/frxt