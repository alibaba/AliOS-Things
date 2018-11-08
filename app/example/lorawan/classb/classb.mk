NAME := classb

GLOBAL_INCLUDES += .

linkwan := 1

$(NAME)_SOURCES := classb.c

$(NAME)_COMPONENTS := network.lorawan_4_4_2

$(NAME)_COMPONENTS += kernel.fs.kv

GLOBAL_INCLUDES += ./
