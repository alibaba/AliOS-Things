NAME := project

$(NAME)_TYPE := kernel
$(NAME)_SOURCES := common/board/board.c \
                   common/board/board_common.c \
                   common/board/xr871_evb_main/board_config.c \
                   common/framework/sys_ctrl/container.c \
                   common/framework/sys_ctrl/event_queue.c \
                   common/framework/sys_ctrl/observer.c \
                   common/framework/sys_ctrl/publisher.c \
                   common/framework/sys_ctrl/sys_ctrl.c \
                   common/framework/img_ctrl.c \
                   common/framework/net_ctrl.c \
                   common/framework/net_sys.c \
                   common/framework/platform_init.c \
                   common/framework/sysinfo.c \
                   common/startup/gcc/exception.c \
                   common/startup/gcc/retarget.c \
                   common/startup/gcc/startup.S \
                   common/cmd/cmd_wlan.c \
                   common/cmd/cmd_util.c \
                   main/main.c

$(NAME)_ASMFLAGS += -c -x assembler-with-cpp
