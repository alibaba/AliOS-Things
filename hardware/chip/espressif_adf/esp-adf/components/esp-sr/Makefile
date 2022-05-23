PROJECT_NAME := esp_sr_public

MODULE_PATH := $(abspath $(shell pwd))

EXTRA_COMPONENT_DIRS += $(MODULE_PATH)/lib
EXTRA_COMPONENT_DIRS += $(MODULE_PATH)/wake_word_engine
EXTRA_COMPONENT_DIRS += $(MODULE_PATH)/speech_command_recognition
EXTRA_COMPONENT_DIRS += $(MODULE_PATH)/acoustic_algorithm

include $(IDF_PATH)/make/project.mk

