/* don't edit, auto generated by build/tools/gencsiconfig.sh */
#ifndef __CSI_CONFIG_H__
#define __CSI_CONFIG_H__
#define CONFIG_CPU_CK802 1
//#define CONFIG_CHIP_VENDOR_CSKY 1
//#define CONFIG_CHIP_HOBBIT1_2 1
//#define CONFIG_CHIP_VENDOR_STR "csky"
//#define CONFIG_CHIP_NAME_STR "hobbit1_2"
//#define CONFIG_BOARD_HOBBIT1_2_EVB 1
//#define CONFIG_BOARD_NAME_STR "hobbit1_2_evb"
#define CONFIG_KERNEL_RHINO 1
#define CONFIG_SEPARATE_IRQ_SP 1
#define CONFIG_ARCH_INTERRUPTSTACK 600
#define CONFIG_MM_KERNEL_HEAPSIZE 32768
#ifdef WITH_SAL
#define CONFIG_NETIF_WIFI     1
#endif
#ifdef WITH_LWIP
#define CONFIG_NETIF_ETH    1
#define CONFIG_ETH_ENC28J60 1
#define CONFIG_ETH_ENC28J60_USE_PBUF  1
#endif
//#define CONFIG_EXAMPLES_DRIVER 1
//#define CONFIG_EXAMPLES_SPI 1
//#define CONFIG_DEBUG_LEVEL "-g2"
//#define CONFIG_OPTIMIZE_LEVEL "-Os"
#endif
