NAME := 7_fonts

$(NAME)_SOURCES += lv_tutorial_fonts.c
$(NAME)_SOURCES += arial_ascii_20.c
$(NAME)_SOURCES += arial_cyrillic_20.c
$(NAME)_SOURCES += arial_math_20.c

$(NAME)_INCLUDES    += ../../
$(NAME)_INCLUDES    += ../

GLOBAL_INCLUDES     += ../../..
