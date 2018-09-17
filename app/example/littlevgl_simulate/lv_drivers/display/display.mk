NAME := dispaly

$(NAME)_SOURCES += fbdev.c
$(NAME)_SOURCES += monitor.c
$(NAME)_SOURCES += R61581.c
$(NAME)_SOURCES += SSD1963.c
$(NAME)_SOURCES += ST7565.c

$(NAME)_INCLUDES    += ../../
$(NAME)_INCLUDES    += ../