NAME := benchmark

$(NAME)_SOURCES += benchmark.c
$(NAME)_SOURCES += img_benchmark_bg.c

$(NAME)_INCLUDES    += ../../
$(NAME)_INCLUDES    += ../

GLOBAL_INCLUDES     += ../../..