#ifndef _SOC_CONFIG_H_
#define _SOC_CONFIG_H_

#include <soc_types.h>

#define CONFIG_VENDOR_STR               "South Silicon Valley"
#define CONFIG_VENDOR_ID
#define CONFIG_PRODUCT_STR              "SSV6006"
#define CONFIG_PRODUCT_ID
#define CONFIG_PRODUCT_VERSION
#define CONFIG_FW_NAME_STR
#define CONFIG_FW_VERSION

#define CLI_ENABLE                      1   // Enable/Disable CLI
#define CLI_BUFFER_SIZE                 80  // CLI Buffer size
#define CLI_ARG_SIZE                    6   // The number of arguments
#define CLI_PROMPT                      "RB-Turismo> "

//#define TCPIP_ENABLE                    0
//#define AP_MODE_ENABLE                  0
#define ENABLE_DEBUG_TIMER              1   // Debug timer for profiling. (Using MTX timer)
//#define ENABLE_BACKGROUND_PMK_CALC      1   // Let WPA2/PSK PMK calculation run at background.
#define ENABLE_PSK_CACHE                1   // Cache PSK.
#define CONSOLE_SERIAL_PORT             serCOM1

#define CONFIG_LOG_ENABLE               0

#define PACKED                              __attribute__((packed))

#define HZ                                   1000             // 100Hz => 100 ticks per second

#define MAX_MAIN_IRQ_ENTRY                   10
#define MAX_PERI_IRQ_ENTRY                   10

/* Message Queue Length: */
#define MBOX_MAX_MSG_EVENT                   74              //(64+10) 64:tx id, 10 for rx // Max message event in the system

#define WLAN_MAX_CONN                        8

//#define __REG_PRINT__    //direct printf to register to speed up for simulate

#define CLI_HISTORY_ENABLE			1               // Enable/Disable CLI history log. only work in SIM platofrm for now.
#define CLI_HISTORY_NUM				5

#define MAILBOX_DBG                      0

#define USE_SERIAL_DRV                   1        // Use interrupt-driven serial driver instead of direct UART register access

//AP mode send deauth information
#define SEND_DEAUTH_COUNT 3
#define NO_NEED_TO_SEND_DEAUTH_COUNT 3


#define HIGH_TEMPERATURE    32 //60 degree
#define LOW_TEMPERATURE     12 //0 degree

#define PACKET_TRACE        0

//
//#define ENABLE_CLI_CMD_DUMP_REG

//
#define ENABLE_CLI_CMD_MIB

//#define _INTERNAL_MCU_SUPPLICANT_SUPPORT_

#define ENABLE_RATE_CONTROL 0


#endif /* _SOC_CONFIG_H_ */


