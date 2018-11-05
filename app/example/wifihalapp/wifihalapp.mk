NAME := wifihalapp

$(NAME)_SOURCES := wifihalapp.c

$(NAME)_COMPONENTS += yloop cli halwifi

GLOBAL_INCLUDES += ./
