NAME := classb

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
GLOBAL_INCLUDES += .

linkwan := 1

$(NAME)_SOURCES := classb.c

$(NAME)_COMPONENTS := network.lorawan.lorawan_4_4_2

$(NAME)_COMPONENTS += kernel.fs.kv

GLOBAL_INCLUDES += ./
