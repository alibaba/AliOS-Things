NAME := indev

$(NAME)_SOURCES += FT5406EE8.c
$(NAME)_SOURCES += keyboard.c
$(NAME)_SOURCES += mouse.c
$(NAME)_SOURCES += XPT2046.c

$(NAME)_INCLUDES    += ../../
$(NAME)_INCLUDES    += ../