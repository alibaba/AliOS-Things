JSE_COMPONENTS_DIR := src/components
# including MIIO source code
ifeq ($(JSE_NET_ADDON_MIIO),y)
${NAME}_SOURCES  += \
										${JSE_COMPONENTS_DIR}/miio/miio_device.c \
										${JSE_COMPONENTS_DIR}/miio/miio_discover.c
${NAME}_INCLUDES += \
										${JSE_COMPONENTS_DIR}/ \
										${JSE_COMPONENTS_DIR}/miio/
endif