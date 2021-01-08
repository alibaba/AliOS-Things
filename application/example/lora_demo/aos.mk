NAME := lora_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := lora demo
$(NAME)_SOURCES := demo.c radio.c sx126x.c sx126x-haas.c

$(NAME)_COMPONENTS += osal_aos

$(NAME)_INCLUDES += ./
