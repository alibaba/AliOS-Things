NAME := xtensa

$(NAME)_SOURCES := port.c \
                   xtensa_panic.S

GLOBAL_INCLUDES += . ./include    \
                   ./include/frxt