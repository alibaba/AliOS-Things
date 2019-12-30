JSE_SERVICES_DIR := src/services

APP_MGR_ENABLE := 0

${NAME}_INCLUDES += $(JSE_SERVICES_DIR)/app_mgr

# enable debuger?
ifeq ($(JSE_IDE_DEBUG),y)
${NAME}_SOURCES +=  \
										$(JSE_SERVICES_DIR)/debugger/dbg_ssdp.c \
										$(JSE_SERVICES_DIR)/debugger/dbg_ws.c
${NAME}_INCLUDES += $(JSE_SERVICES_DIR)/debugger/
APP_MGR_ENABLE := 1
endif


# enable cli?
ifeq ($(JSE_CLI_DEBUG),y)
${NAME}_SOURCES +=  \
										$(JSE_SERVICES_DIR)/cli/cli_app.c \
										$(JSE_SERVICES_DIR)/cli/cli_ext.c \
										$(JSE_SERVICES_DIR)/cli/cli_js.c \
										$(JSE_SERVICES_DIR)/cli/cli_dev.c \
										$(JSE_SERVICES_DIR)/cli/cli_sub_call.c \
										$(JSE_SERVICES_DIR)/cli/cli_file.c
${NAME}_INCLUDES += $(JSE_SERVICES_DIR)/cli
APP_MGR_ENABLE := 1
endif

# enable app mgr?
ifeq ($(APP_MGR_ENABLE),1)
ifeq ($(JSE_IDE_DEBUG),y)
${NAME}_SOURCES += $(JSE_SERVICES_DIR)/app_mgr/ota_socket.c
endif
${NAME}_SOURCES += $(JSE_SERVICES_DIR)/app_mgr/app_mgr.c
endif


${NAME}_SOURCES +=  \
										$(JSE_SERVICES_DIR)/board_mgr/board_info.c \
										$(JSE_SERVICES_DIR)/board_mgr/board_mgr.c
${NAME}_INCLUDES += \
										$(JSE_SERVICES_DIR)/board_mgr \
										$(JSE_SERVICES_DIR)



