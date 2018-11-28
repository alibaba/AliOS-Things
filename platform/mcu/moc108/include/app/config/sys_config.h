#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

//#define KEIL_SIMULATOR

/*SUMMARY: macro--1: OPEN; --0:CLOSE*/
#define CFG_READ_FLASH                             0
#define CFG_MODE_SWITCH                            1

/*section 0-----app macro config-----*/
#define CFG_IEEE80211N                             0

#if CFG_MODE_SWITCH
#define CFG_WIFI_AP_MODE                           1
#define CFG_WIFI_STATION_MODE                      1

#define CFG_MICO_DEMO                              1
#endif

#define CFG_WIFI_WEP                               0
#define CFG_WEP_KEY                    "1111117231"

#define CFG_WIFI_WPA                               1
#define CFG_WPA_TYPE                               1 /*0: TKIP 1: CCMP*/
#define CFG_WPA_KEY                    "12345678"

/* macro of ap mode*/
#define CFG_SSID_AP                    "beken7231"
#define CFG_CHANNEL_AP                            11

#define CFG_OOB_CONNECT_SSID           "flashtest"

/*section 1-----OS macro config-----*/

#define THD_APPLICATION_PRIORITY                   20
#define THD_CORE_PRIORITY                          2
#define THD_LWIP_PRIORITY                          4
#define THD_INIT_PRIORITY                          4
#define THD_RECONNECT_PRIORITY                     4
#define THD_WPAS_PRIORITY                          6
#define THD_HOSTAPD_PRIORITY                       6

/*section 2-----function macro config-----*/
#define CFG_TX_EVM_TEST                            1
#define CFG_RX_SENSITIVITY_TEST                    1


/*section 3-----driver macro config-----*/
#define CFG_MAC_PHY_BAPASS                         1

#define CFG_SDIO                                   0
#define CFG_SDIO_TRANS                             0
#define CFG_REAL_SDIO                              0
#if CFG_REAL_SDIO
#define FOR_SDIO_BLK_512                           0
#endif

#if CFG_MODE_SWITCH
#define CFG_MSDU_RESV_HEAD_LEN                    96
#else
#if CFG_WIFI_WPA
#define CFG_MSDU_RESV_HEAD_LEN                    96
#else
#define CFG_MSDU_RESV_HEAD_LEN                    64
#endif
#endif

#define CFG_MSDU_RESV_TAIL_LEN                    16

#define CFG_USB                                    0
#if CFG_USB
#define CFG_SUPPORT_MSD                            0
#define CFG_SUPPORT_HID                            0
#define CFG_SUPPORT_CCD                            0
#define CFG_SUPPORT_UVC                            1
#endif


/*section 4-----DEBUG macro config-----*/
#define CFG_UART_DEBUG                             0
#define CFG_UART_DEBUG_COMMAND_LINE                1
#define CFG_BACKGROUND_PRINT                       0
#define CFG_SUPPORT_BKREG                          1


/*section 5-----PRODUCT macro config-----*/
#define CFG_RELEASE_FIRMWARE                       0

/*section 6-----for platform*/
#define SOC_PLATFORM                               1
#define FPGA_PLATFORM                              0
#define CFG_RUNNING_PLATFORM                       SOC_PLATFORM

/*section 7-----calibration*/
#if (CFG_RUNNING_PLATFORM == FPGA_PLATFORM)
#define CFG_SUPPORT_CALIBRATION                    0
#define CFG_SUPPORT_MANUAL_CALI                    0
#else
#define CFG_SUPPORT_CALIBRATION                    1
#define CFG_SUPPORT_MANUAL_CALI                    1
#endif

/*section 8-----for netstack*/
#define CFG_USE_LWIP_NETSTACK                      1

/*section 9-----for DHCP servicers and client*/
#define CFG_USE_DHCP                               1

/*section 10-----patch*/
#define CFG_BK7221_MDM_WATCHDOG_PATCH              0

/*section 11-----temperature detect*/
#define CFG_USE_TEMPERATURE_DETECT                 0

/*section 12-----for SPIDMA interface*/
#define CFG_USE_SPIDMA                             0

/*section 13-----for GENERRAL DMA */
#define CFG_GENERAL_DMA                            0

/*section 14-----for SDCARD HOST*/
#define CFG_USE_SDCARD_HOST                        0

/*section 15-----for FTPD UPGRADE*/
#define CFG_USE_FTPD_UPGRADE                       0

/*section 16-----support customer macro*/
#define CFG_SUPPORT_TIANZHIHENG_DRONE              0
#define CFG_MXCHIP                                 1

#endif // _SYS_CONFIG_H_
