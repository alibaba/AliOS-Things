GLOBAL_INCLUDES +=  ./imx6_platform_sdk/board/mx6dq/smart_device  \
                    ./imx6_platform_sdk/sdk/include/mx6dq


#ifeq ($(OS_MUTICORE_NUM), 4)
ifdef OS_MUTICORE_NUM
GLOBAL_INCLUDES += ./imx6dq/smp
$(NAME)_SOURCES  += ./imx6dq/smp/smp_int.c
$(NAME)_SOURCES  += ./imx6dq/smp/smp_load.c
$(NAME)_SOURCES  += ./imx6dq/smp/smp_sync.c
endif


$(NAME)_SOURCES +=  ./imx6_platform_sdk/sdk/drivers/i2c/src/imx_i2c.c   \
                    ./imx6_platform_sdk/sdk/common/ioexpander/src/max7310.c   \
                    ./imx6_platform_sdk/board/common/hardware_modules.c   \
                    ./imx6_platform_sdk/board/common/board_io_expanders.c   \
                    ./imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/uart_iomux_config.c   \
                    ./imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/uart1_iomux_config.c   \
                    ./imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/iomux_config.c   \
                    ./imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/i2c_iomux_config.c   \
                    ./imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/i2c1_iomux_config.c   \
                    ./imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/i2c2_iomux_config.c   \
                    ./imx6_platform_sdk/board/mx6dq/smart_device/rev_a_iomux/i2c3_iomux_config.c
