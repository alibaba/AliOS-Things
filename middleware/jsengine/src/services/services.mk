JSE_SERVICES_DIR := src/services

APP_MGR_ENABLE := 0

${NAME}_INCLUDES += $(JSE_SERVICES_DIR)/app-mgr

# 判断是否需要编译 be-debuger
ifeq ($(JSE_IDE_DEBUG),y)
${NAME}_SOURCES +=  \
										$(JSE_SERVICES_DIR)/be-debugger/be-ssdp/be_ssdp.c \
										$(JSE_SERVICES_DIR)/be-debugger/be-websocket/websocket.c
${NAME}_INCLUDES += \
										$(JSE_SERVICES_DIR)/be-debugger/be-ssdp \
										$(JSE_SERVICES_DIR)/be-debugger/be-websocket
APP_MGR_ENABLE := 1
endif


# 判断是否需要编译 cli
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

# 判断是否需要编译 app-mgr
ifeq ($(APP_MGR_ENABLE),1)
ifeq ($(JSE_IDE_DEBUG),y)
${NAME}_SOURCES += $(JSE_SERVICES_DIR)/app-mgr/ota_socket.c
endif
${NAME}_SOURCES += $(JSE_SERVICES_DIR)/app-mgr/app_mgr.c
endif


${NAME}_SOURCES +=  \
										$(JSE_SERVICES_DIR)/board-mgr/board_info.c \
										$(JSE_SERVICES_DIR)/board-mgr/board_mgr.c
${NAME}_INCLUDES += \
										$(JSE_SERVICES_DIR)/board-mgr \
										$(JSE_SERVICES_DIR)



