NAME := uai_kws_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := uAI KWS DEMO
$(NAME)_SOURCES := uai_kws_demo.c \
					dnn_weights.c \
					wav_data.c    \
					mfcc.c

$(NAME)_COMPONENTS += osal_aos cli uai 

GLOBAL_DEFINES += AOS_NO_WIFI

GLOBAL_INCLUDES += ./

cmsis_nn_enable ?= 1
dnn_enable      ?= 1