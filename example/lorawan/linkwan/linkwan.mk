NAME := linkwan

GLOBAL_INCLUDES += .

linkwan := 1
ifeq ($(linkwanat), 1)
GLOBAL_DEFINES += LOW_POWER_DISABLE
endif

$(NAME)_SOURCES := linkwan.c

$(NAME)_COMPONENTS := protocols.lorawan
