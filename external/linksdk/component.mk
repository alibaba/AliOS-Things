#
# Component Makefile
#
COMPONENT_DIR := linksdk

COMPONENT_SUBMODULES += $(COMPONENT_DIR)

COMPONENT_ADD_INCLUDEDIRS := \
	core \
	core/sysdep \
    core/utils \
    components/bootstrap \
    components/data-model \
    components/subdev \
    components/devinfo \
    components/mqtt-upload \
    components/dynreg \
    components/dynreg-mqtt \
    components/ntp

COMPONENT_SRCDIRS := \
	core \
	core/sysdep \
	core/utils \
	components/bootstrap \
	components/data-model \
	components/subdev \
	external \
	portfiles/aiot_port \
	components/devinfo \
	components/mqtt-upload \
	components/dynreg \
	components/dynreg-mqtt \
	components/ntp


SRCDIRS_HAAS += $(addprefix $(EXTERNAL_DIR)/$(COMPONENT_DIR)/, $(COMPONENT_SRCDIRS))
INC_HAAS += $(addprefix $(EXTERNAL_DIR)/$(COMPONENT_DIR)/, $(COMPONENT_ADD_INCLUDEDIRS))
