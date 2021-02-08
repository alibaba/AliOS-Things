NAME := ucloud_ai

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := ai agent

$(NAME)_COMPONENTS := ucloud

$(NAME)_SOURCES := ./src/ucloud_ai.c

$(NAME)_INCLUDES += ./include/
GLOBAL_INCLUDES += ./include/

#face and body AI detection
ifeq ($(UCLOUD_AI_FACEBODY_ENABLE),y)
$(NAME)_SOURCES += ./src/ucloud_ai_facebody.c
endif

ifeq ($(UCLOUD_AI_OBJECTDET_ENABLE),y)
$(NAME)_SOURCES += ./src/ucloud_ai_objectdet.c
endif
