#
# Component Makefile
#
COMPONENT_DIR = utility
COMPONENT_SUBMODULES += $(COMPONENT_DIR)

COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src

INC_HAAS += $(addprefix $(MODULES_DIR)/$(COMPONENT_DIR)/, $(COMPONENT_ADD_INCLUDEDIRS))
SRC_HAAS += $(addprefix modules/$(COMPONENT_DIR)/$(COMPONENT_SRCDIRS)/,\
	modutility.c \
	crc16_ibm.c \
	crc16_modbus.c \
)

CFLAGS += -DMICROPY_PY_UTILITY=1