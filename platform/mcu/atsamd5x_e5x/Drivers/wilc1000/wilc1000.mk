NAME := device_sal_wilc1000

$(NAME)_SOURCES += bsp/source/nm_bsp.c	\
				   bus_wrapper/source/sdio_same54.c	\
				   bus_wrapper/source/nm_bus_wrapper.c	\
				   common/source/nm_common.c	\
				   driver/source/m2m_hif.c	\
				   driver/source/m2m_periph.c	\
				   driver/source/m2m_wifi.c	\
				   driver/source/nmasic.c	\
				   driver/source/nmbus.c	\
				   driver/source/nmdrv.c	\
				   driver/source/nmi2c.c	\
				   driver/source/nmsdio.c	\
				   driver/source/nmspi.c	\
				   driver/source/nmuart.c	\
				   netif/source/wilc_netif.c 	\
				   os/source/m2m_wifi_ex.c 	\
				   os/source/net_init.c	\
				   os/source/os_hook.c	\
				   wifi_port.c

GLOBAL_DEFINES += DEV_SAL_WILC1000
GLOBAL_DEFINES += __SAME54__

$(NAME)_COMPONENTS += yloop
#ifneq (1, $(at_adapter))
#$(NAME)_COMPONENTS += sal atparser

#$(NAME)_SOURCES += bsp/sources/nm_bsp.c
#endif


GLOBAL_INCLUDES += bsp/include/	\
				   bus_wrapper/include/ \
				   common/include	\
				   driver/include 	\
				   drv_hash/	\
				   netif/include	\
				   os/include/	\
				   /	\
				   ../../../../kernel/protocols/net/include	\
				   ../../../../kernel/protocols/net/include/lwip	\
				   ../../../../kernel/protocols/net/port/include	\
				   ../../../../kernel/protocols/net/core/	\
				   ../../../../platform/mcu/atsamd5x_e5x/Drivers/sd_mmc/

				
