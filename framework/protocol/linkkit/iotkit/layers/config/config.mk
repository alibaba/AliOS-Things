#NAME := iot-config

SOURCE_DIR = layers/config

GLOBAL_INCLUDES += ./layers/config／include
$(NAME)_SOURCES := $(SOURCE_DIR)/src/dev_conf.c

                    
