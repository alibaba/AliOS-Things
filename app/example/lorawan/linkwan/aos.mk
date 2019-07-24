NAME := linkwan

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := linkwan

$(NAME)_SOURCES := main.c

linkwan := 1

# lorawanback defined at board/***/aos.mk
ifeq ($(lorawanback), 0)
$(NAME)_COMPONENTS := lorawan_4_4_2
else ifeq ($(lorawanback), 1)
$(NAME)_COMPONENTS := lorawan_4_4_0
endif

$(NAME)_COMPONENTS += kv

$(NAME)_INCLUDES += ./
