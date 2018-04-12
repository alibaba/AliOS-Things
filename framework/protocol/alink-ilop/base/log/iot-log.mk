#NAME := iot-log
LOG_SOURCE_DIR = base/log/LITE-log

$(NAME)_INCLUDES += $(LOG_SOURCE_DIR)/

$(NAME)_SOURCES  += $(LOG_SOURCE_DIR)/lite-log.c

