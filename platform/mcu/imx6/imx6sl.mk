GLOBAL_INCLUDES +=  ./imx6_platform_sdk/board/mx6sl/evk  \
                    ./imx6_platform_sdk/sdk/include/mx6sl

$(NAME)_SOURCES +=  ./imx6_platform_sdk/board/mx6sl/evk/hardware_modules.c   \
                    ./imx6_platform_sdk/board/mx6sl/evk/board_io_expanders.c   \
                    ./imx6_platform_sdk/board/mx6sl/evk/iomux/uart_iomux_config.c   \
                    ./imx6_platform_sdk/board/mx6sl/evk/iomux/uart1_iomux_config.c   \
                    ./imx6_platform_sdk/board/mx6sl/evk/iomux/iomux_config.c
