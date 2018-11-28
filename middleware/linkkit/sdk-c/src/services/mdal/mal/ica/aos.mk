NAME := libiot_malica

$(NAME)_SOURCES := mdal_ica_at_client.c 

malica_test ?= 0
ifeq (1,$(malica_test))
$(NAME)_SOURCES += test/mdal_ica_at_client_test.c
GLOBAL_DEFINES += MDAL_MAL_ICA_TEST
endif

GLOBAL_INCLUDES += ./
