NAME := IOT_AT3080_demo

$(NAME)_SOURCES := app_console.c \
                   app_rgbled.c \
                   app_sht20.c \
                   app_switch.c \
                   main.c

GLOBAL_INCLUDES += .

$(NAME)_COMPONENTS := yloop cli