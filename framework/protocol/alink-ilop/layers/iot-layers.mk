#NAME := iot-layers

LAYER_SOURCE_DIR = layers/config
$(NAME)_INCLUDES  += $(LAYER_SOURCE_DIR)/include
$(NAME)_SOURCES   += $(LAYER_SOURCE_DIR)/src/dev_conf.c


                    
