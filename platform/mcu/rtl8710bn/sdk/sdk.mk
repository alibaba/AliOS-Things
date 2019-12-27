#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

$(NAME)_INCLUDES += ../../../../components/network/lwip/include
$(NAME)_INCLUDES += ./

$(NAME)_INCLUDES += sdk/project/realtek_amebaz_va0_example/inc
$(NAME)_INCLUDES += sdk/component/os/os_dep/include
$(NAME)_INCLUDES += sdk/component/common/api/network/include
$(NAME)_INCLUDES += sdk/component/common/api
$(NAME)_INCLUDES += sdk/component/common/api/at_cmd
$(NAME)_INCLUDES += sdk/component/common/api/platform
$(NAME)_INCLUDES += sdk/component/common/api/wifi
$(NAME)_INCLUDES += sdk/component/common/api/wifi/rtw_wpa_supplicant/src
$(NAME)_INCLUDES += sdk/component/common/api/wifi/rtw_wowlan
$(NAME)_INCLUDES += sdk/component/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant
$(NAME)_INCLUDES += sdk/component/common/drivers/modules
$(NAME)_INCLUDES += sdk/component/common/drivers/sdio/realtek/sdio_host/inc
$(NAME)_INCLUDES += sdk/component/common/drivers/inic/rtl8711b
$(NAME)_INCLUDES += sdk/component/common/drivers/usb_class/device
$(NAME)_INCLUDES += sdk/component/common/drivers/usb_class/device/class
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/include
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/src/osdep
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/src/hci
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/src/hal
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/src/hal/rtl8711b
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/src/hal/OUTSRC
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/wlan_ram_map/rom
$(NAME)_INCLUDES += sdk/component/common/network
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/app/monitor/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/cmsis
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/cmsis/device
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/fwlib
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/fwlib/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/fwlib/ram_lib/crypto
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/fwlib/rom_lib
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/os_dep/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/std_lib/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/std_lib/libc/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/std_lib/libc/rom/string
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/std_lib/libgcc/rtl8195a/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/rtl_lib
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/misc
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/misc/os
$(NAME)_INCLUDES += sdk/component/common/mbed/api
$(NAME)_INCLUDES += sdk/component/common/mbed/hal
$(NAME)_INCLUDES += sdk/component/common/mbed/hal_ext
$(NAME)_INCLUDES += sdk/component/common/mbed/targets/cmsis/rtl8711b
$(NAME)_INCLUDES += sdk/component/common/mbed/targets/hal/rtl8711b

#$(NAME)_SOURCES := startup.c
#$(NAME)_SOURCES += app_start.c
#$(NAME)_SOURCES += system_8195a.c
#$(NAME)_SOURCES += peripheral/serial_api.c
#$(NAME)_SOURCES += peripheral/pinmap.c
#$(NAME)_SOURCES += peripheral/pinmap_common.c

#$(NAME)_SOURCES := soc_impl.c

ifneq ($(AOS_2NDBOOT_SUPPORT), yes)
#cmsis
$(NAME)_SOURCES += sdk/component/soc/realtek/8711b/cmsis/device/app_start.c
$(NAME)_SOURCES += sdk/component/soc/realtek/8711b/fwlib/ram_lib/startup.c
$(NAME)_SOURCES += sdk/component/soc/realtek/8711b/cmsis/device/system_8195a.c

#console
$(NAME)_SOURCES += sdk/component/common/api/at_cmd/atcmd_wifi.c
$(NAME)_SOURCES += sdk/component/common/api/at_cmd/atcmd_sys.c
$(NAME)_SOURCES += sdk/component/common/api/at_cmd/log_service.c
$(NAME)_SOURCES += sdk/component/soc/realtek/8711b/app/monitor/ram/low_level_io.c
$(NAME)_SOURCES += sdk/component/soc/realtek/8711b/app/monitor/ram/monitor.c
$(NAME)_SOURCES += sdk/component/soc/realtek/8711b/app/monitor/ram/rtl_consol.c
$(NAME)_SOURCES += sdk/component/soc/realtek/8711b/app/monitor/ram/rtl_trace.c

#network api wifi
$(NAME)_SOURCES += sdk/component/common/api/wifi/wifi_conf.c
$(NAME)_SOURCES += sdk/component/common/api/wifi/wifi_ind.c
$(NAME)_SOURCES += sdk/component/common/api/wifi/wifi_promisc.c
$(NAME)_SOURCES += sdk/component/common/api/wifi/wifi_simple_config.c
$(NAME)_SOURCES += sdk/component/common/api/wifi/wifi_util.c

#network api wifi rtw_wpa_supplican
$(NAME)_SOURCES += sdk/component/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_eap_config.c
$(NAME)_SOURCES += sdk/component/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_wps_config.c
$(NAME)_SOURCES += sdk/component/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_p2p_config.c

#network app
$(NAME)_SOURCES += sdk/component/common/api/network/src/ping_test.c
#$(NAME)_SOURCES += /component/common/utilities/ssl_client.c
$(NAME)_SOURCES += sdk/component/common/utilities/tcptest.c
$(NAME)_SOURCES += sdk/component/common/api/network/src/wlan_network.c

#network api
$(NAME)_SOURCES += sdk/component/common/api/lwip_netconf.c

#network lwip
$(NAME)_SOURCES += sdk/component/common/network/dhcp/dhcps.c

#lwip port
$(NAME)_SOURCES += sdk/component/common/drivers/wlan/realtek/src/osdep/lwip_intf.c

#os osdep
$(NAME)_SOURCES += sdk/component/os/os_dep/device_lock.c
$(NAME)_SOURCES += sdk/component/os/os_dep/osdep_service.c

#peripheral api
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/analogin_api.c
#$(NAME)_SOURCES += /component/common/mbed/targets/hal/rtl8711b/dma_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/efuse_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/flash_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/gpio_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/gpio_irq_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/i2c_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/i2s_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/nfc_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/pinmap.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/pinmap_common.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/port_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/pwmout_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/rtc_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/serial_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/sleep.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/spi_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/sys_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/timer_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/us_ticker.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/us_ticker_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/wait_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/wdt_api.c

#peripheral rtl8710b
$(NAME)_SOURCES += sdk/component/soc/realtek/8711b/fwlib/ram_lib/rtl8710b_dsleepcfg.c
$(NAME)_SOURCES += sdk/component/soc/realtek/8711b/fwlib/ram_lib/rtl8710b_dstandbycfg.c
$(NAME)_SOURCES += sdk/component/soc/realtek/8711b/fwlib/ram_lib/rtl8710b_intfcfg.c
$(NAME)_SOURCES += sdk/component/soc/realtek/8711b/misc/rtl8710b_ota.c
$(NAME)_SOURCES += sdk/component/soc/realtek/8711b/fwlib/ram_lib/rtl8710b_pinmapcfg.c
$(NAME)_SOURCES += sdk/component/soc/realtek/8711b/fwlib/ram_lib/rtl8710b_sleepcfg.c

else
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/flash_api.c
$(NAME)_SOURCES += sdk/component/common/mbed/targets/hal/rtl8711b/wdt_api.c
endif
