#
# Component Makefile
#
COMPONENT_DIR = activation
COMPONENT_SUBMODULES += $(COMPONENT_DIR)

COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src

INC_HAAS += $(addprefix $(EXTERNAL_DIR)/$(COMPONENT_DIR)/, $(COMPONENT_ADD_INCLUDEDIRS))
SRC_HAAS += $(addprefix external/$(COMPONENT_DIR)/$(COMPONENT_SRCDIRS)/,\
	activation.c \
	tcp_client.c \
)