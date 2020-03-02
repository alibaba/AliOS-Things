NAME := uai_cifar10_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := uAI CIFAR10 DEMO
$(NAME)_SOURCES := uai_cifar10_demo.c cnn_weights.c img_data.c

$(NAME)_COMPONENTS += osal_aos cli uai

GLOBAL_DEFINES += AOS_NO_WIFI

GLOBAL_INCLUDES += ./

cnn_enable      ?= 1