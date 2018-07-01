#NAME := iot-event
EVENT_SOURCE_DIR = base/event

$(NAME)_INCLUDES += $(EVENT_SOURCE_DIR)/

$(NAME)_SOURCES  += $(EVENT_SOURCE_DIR)/event.c

