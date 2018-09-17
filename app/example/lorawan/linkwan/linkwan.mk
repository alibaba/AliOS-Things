NAME := linkwan

GLOBAL_INCLUDES += .

linkwan := 1

$(NAME)_SOURCES := linkwan.c

$(NAME)_COMPONENTS := network.lorawan

$(NAME)_COMPONENTS += rhino.fs.kv

GLOBAL_INCLUDES += ./