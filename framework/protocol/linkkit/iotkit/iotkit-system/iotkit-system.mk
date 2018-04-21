#NAME := iotkit-system

SYSTEM_SOURCE_DIR = iotkit-system/src
$(NAME)_SOURCES  += $(SYSTEM_SOURCE_DIR)/sdk-impl.c
$(NAME)_SOURCES  += $(SYSTEM_SOURCE_DIR)/ca.c
$(NAME)_SOURCES  += $(SYSTEM_SOURCE_DIR)/device.c
$(NAME)_SOURCES  += $(SYSTEM_SOURCE_DIR)/guider.c
$(NAME)_SOURCES  += $(SYSTEM_SOURCE_DIR)/id2_guider.c
$(NAME)_SOURCES  += $(SYSTEM_SOURCE_DIR)/report.c
$(NAME)_SOURCES  += $(SYSTEM_SOURCE_DIR)/class_interface.c

$(NAME)_INCLUDES  += ./
                 
$(NAME)_CFLAGS    += -DCONFIG_GUIDER_AUTH_TIMEOUT=500 -DMQTT_COMM_ENABLED