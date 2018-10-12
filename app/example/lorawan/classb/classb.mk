NAME := classb

GLOBAL_INCLUDES += .

linkwan := 1

$(NAME)_SOURCES := classb.c

$(NAME)_COMPONENTS := network.lorawan_new

$(NAME)_COMPONENTS += rhino.fs.kv

GLOBAL_INCLUDES += ./
