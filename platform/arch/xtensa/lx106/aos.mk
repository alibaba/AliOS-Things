NAME := arch_xtensa_lx106

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := arch for xtensa_lx106

$(NAME)_SOURCES := port.c         \
                   panic.c        \
                   backtrace.c    \
                   xtensa_panic.S \
                   xtensa_vectors.S

GLOBAL_INCLUDES += include    \
                   include/frxt
