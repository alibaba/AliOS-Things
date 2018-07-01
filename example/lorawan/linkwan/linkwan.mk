NAME := linkwan

GLOBAL_INCLUDES += .

linkwan := 1

$(NAME)_SOURCES := linkwan.c

$(NAME)_COMPONENTS := protocols.lorawan
