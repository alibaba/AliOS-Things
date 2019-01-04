#ifndef _COMMAND_LINE_H_
#define _COMMAND_LINE_H_

#include "include.h"
#include "command_table.h"

#define CLI_DEBUG

#ifdef CLI_DEBUG
#define CLI_PRT       os_printf
#define CLI_WPRT      warning_prf
#else
#define CLI_PRT       os_null_printf
#define CLI_WPRT      os_null_printf
#endif


#define DEBUG_PARSER		    1
#define CONFIG_SYS_CBSIZE		128
#define CONFIG_SYS_MAXARGS		8

/*
 * Rather than doubling the size of the _ctype lookup table to hold a 'blank'
 * flag, just check for space or tab.
 */
#define isblank(c)	(c == ' ' || c == '\t')
#define isascii(c) (((unsigned char)(c))<=0x7f)
#define toascii(c) (((unsigned char)(c))&0x7f)

/*
 * Error codes that commands return to cmd_process(). We use the standard 0
 * and 1 for success and failure, but add one more case - failure with a
 * request to call cmd_usage(). But the cmd_process() function handles
 * CMD_RET_USAGE itself and after calling cmd_usage() it will return 1.
 * This is just a convenience for commands to avoid them having to call
 * cmd_usage() all over the place.
 */
enum command_ret_t
{
    CMD_RET_SUCCESS,	/* 0 = Success */
    CMD_RET_FAILURE,	/* 1 = Failure */
    CMD_RET_USAGE = -1,	/* Failure, please report 'usage' error */
};

#if CFG_SUPPORT_BKREG
#include "arm_arch.h"

typedef struct
{
    unsigned int addr;
    unsigned int value;
} __attribute__ ((packed)) REGISTER_PARAM;

typedef struct
{
    unsigned char code;             /**< 0x01: HCI Command Packet
                                         0x02: HCI ACL Data Packet
                                         0x03: HCI Synchronous Data Packet
                                         0x04: HCI Event Packet */
    unsigned short opcode;
    unsigned char total;
    unsigned char cmd;              /**< private command */
    unsigned char param[];
} __attribute__ ((packed)) HCI_COMMAND_PACKET;

typedef struct
{
    unsigned char code;             /**< 0x01: HCI Command Packet
                                         0x02: HCI ACL Data Packet
                                         0x03: HCI Synchronous Data Packet
                                         0x04: HCI Event Packet */
    unsigned char event;            /**< 0x00-0xFF: Each event is assigned a 1-Octet event code used to uniquely identify different types of events*/
    unsigned char total;            /**< Parameter Total Length */
    unsigned char param[];
} __attribute__ ((packed)) HCI_EVENT_PACKET;

#include "uart.h"
#define BKREG_TX_FIFO_THRD                      RX_RB_LENGTH //(0x40)

#define HCI_EVENT_HEAD_LENGTH                   (0x03)
#define HCI_COMMAND_HEAD_LENGTH                 (0x04)
#define OTP_CMD_RET_LEN                         (12)
#define OTP_READ_MAX_LEN                        (BKREG_TX_FIFO_THRD - HCI_EVENT_HEAD_LENGTH - OTP_CMD_RET_LEN)
#define OTP_WRITE_MAX_LEN                       (RX_RB_LENGTH - 13) // RX_RB_LENGTH 01E0FCXXA8 ADDR-4 LEN-4

/*****************************
 * HCI Events - Event codes
 ******************************/
#define HCI_INQUIRY_COMPLETE_EVENT                         0x01
#define HCI_INQUIRY_RESULT_EVENT                           0x02
#define HCI_CONNECTION_COMPLETE_EVENT                      0x03
#define HCI_CONNECTION_REQUEST_EVENT                       0x04
#define HCI_DISCONNECTION_COMPLETE_EVENT                   0x05
#define HCI_AUTHENTICATION_COMPLETE_EVENT                  0x06
#define HCI_READ_REMOTE_NAME_REQUEST_COMPLETE_EVENT        0x07
#define HCI_ENCRYPTION_CHANGE_EVENT                        0x08
#define HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT      0x09
#define HCI_MASTER_LINK_KEY_COMPLETE_EVENT                 0x0A
#define HCI_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT  0x0B
#define HCI_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT 0x0C
#define HCI_QoS_SETUP_COMPLETE_EVENT                       0x0D
#define HCI_COMMAND_COMPLETE_EVENT                         0x0E
#define HCI_COMMAND_STATUS_EVENT                           0x0F
#define HCI_HARDWARE_ERROR_EVENT                           0x10
#define HCI_FLUSH_OCCURED_EVENT                            0x11
#define HCI_ROLE_CHANGE_EVENT                              0x12
#define HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT              0x13
#define HCI_MODE_CHANGE_EVENT                              0x14
#define HCI_RETURN_LINK_KEYS_EVENT                         0x15
#define HCI_PIN_CODE_REQUEST_EVENT                         0x16
#define HCI_LINK_KEY_REQUEST_EVENT                         0x17
#define HCI_LINK_KEY_NOTIFICATION_EVENT                    0x18
#define HCI_LOOPBACK_COMMAND_EVENT                         0x19
#define HCI_DATA_BUFFER_OVERFLOW_EVENT                     0x1A
#define HCI_MAX_SLOTS_CHANGE_EVENT                         0x1B
#define HCI_READ_CLOCK_OFFSET_EVENT                        0x1C
#define HCI_CONNECTION_PACKET_TYPE_CHANGED_EVENT           0x1D
#define HCI_CONNECTION_PACKET_TYPE_CHANGED_EVENT           0x1D
#define HCI_QOS_VIOLATION_EVENT                            0x1E
#define HCI_PAGE_SCAN_MODE_CHANGE_EVENT                    0x1F
#define HCI_PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT         0x20


/*
 * HCI transport type bytes
 */

enum
{
    TRA_HCIT_COMMAND = 1,
    TRA_HCIT_ACLDATA = 2,
    TRA_HCIT_SCODATA = 3,
    TRA_HCIT_EVENT   = 4
};

enum
{
    BEKEN_UART_LINK_CHECK                       = 0x00, /*return 0x04 0x0e 0x04 0x01 0xe0 0xfc 0x00*/
    BEKEN_UART_REGISTER_WRITE_CMD               = 0x01, /*Write certain reg*/
    BEKEN_UART_REGISTER_CONTINUOUS_WRITE_CMD    = 0x02, /*write reg continuous*/
    BEKEN_UART_REGISTER_READ_CMD                = 0x03, /*read certain reg value*/

    BEKEN_UART_BT_START_CMD                     = 0x04, /*useless*/
    BEKEN_UART_BT_STOP_CMD                      = 0x05, /*useless*/
    BEKEN_UART_PATCH_CMD                        = 0x06, /*useless*/

    BEKEN_UART_SET_UART_PROTOCOL                = 0x07, /*set uart protocol, H4:00  BCSP:01*/

    /*
    01 e0 fc 09 08 aa aa aa aa bb cc dd ee
    aa: baudrate
    bb: byte size 5,6,7,8
    cc: stop len  1 or 2
    dd: par_en    00:no par 01:par
    ee: par       00:odd 01:even
    */
    BEKEN_UART_SET_UART_CONFIG                  = 0x08, /*set uart config*/
    BEKEN_ENABLE_AFC                            = 0x09, /*enable afc*/

    /*
    01 e0 fc 06 0A aa bb cc dd ee
    aa: data_len        0:16bit, N(8,13,14,15): N bit
    bb: speed;          0:200K,1: 1M,2:64K,3:external,4:256K,5:512K,6:128K
    cc: is_msb          0:msb,1:lsb
    dd: role;           0:slave 1:master
    ee: sync_type;      0:short_sync, N(1-7): long_sync with length N
    */
    BEKEN_CONFIG_PCM                            = 0x0A, /*config PCM*/

    //0:normal mode; 1:only stop CPU when acl number = 0; 2:not stop CPU
    BEKEN_CPU_HALT_MODE                         = 0x0B, /*set CPU sleep mode*/
    BEKEN_ENABLE_32K_SLEEP                      = 0x0C, /*enable cpu enter 32K sleep*/
    BEKEN_ENABLE_ANALOG_POWERDOWN               = 0x0D, /*enable close CEVA clock*/

    BEKEN_ENABLE_GPIO_EINT_WAKEUP               = 0x0E, /*enable GPIO wake up CPU*/
    BEKEN_ENABLE_UART_RX_WAKEUP                 = 0x0F, /*enable uart RX wake up BT chip*/
    BEKEN_SET_UART_RX_WAKEUP_COUNT              = 0x10, /*set UART RX wake up count*/

    BEKEN_ENABLE_UART_TX_WAKEUP                 = 0x11, /*enable uart TX wake up HOST*/
    BEKEN_SET_UART_AFTER_WAKEUP_SIG_WAIT_COUNT  = 0x12, /*set UART TX wake up HOST count*/

    BEKEN_MAX_ACL_BUFF_SIZE                     = 0x13, /*useless*/
    BEKEN_ACL_ACTIVE_CHECK_WHEN_SLEEP           = 0x14,
    BEKEN_DISABLE_SNIFFER_WHEN_OTHER_LINK_ACTIVE = 0x15,
    BEKEN_LM_CONFIG_SLEEP_IN_STANDBY_MONITOR_PERIOD = 0x16,
    BEKEN_LM_CONFIG_AWAKE_IN_STANDBY_MONITOR_PERIOD = 0x17,
    BEKEN_DISABLE_ACL_ACCEPT_WHEN_ACL_EXIST     = 0x18,
    BEKEN_ENABLE_TX_POWER_CONTROL               = 0x19,
    BEKEN_LM_CONFIG_AWAKE_KEEP_WHEN_UART_WAKEUP = 0x1A,
    BEKEN_DISABLE_INQUIRY_WHEN_ACL_EXIST        = 0x1B,
    BEKEN_DELAY_BETWEEN_EVERY_PACKET_UART_TX    = 0x1C,
    BEKEN_SCATTER_LENGTH_FOR_PACKET_UART_TX     = 0x1D,
    BEKEN_ENABLE_ACK_SEQ_CHECK                  = 0x1E,
    BEKEN_RESET_ACK_SEQ_AFTER_TX                = 0x1F,
    BEKEN_GET_FW_VERSION                        = 0x20,
    BEKEN_UART_CLOCK_CONFIG_BEFORE_TX_LOW_LEVEL = 0x21,
    BEKEN_UART_BAUD_RATE_FOR_TX_LOW_LEVEL       = 0x22,
    BEKEN_DELAY1_AFTER_TX_LOW_LEVEL             = 0x23,
    BEKEN_DELAY2_AFTER_TX_LOW_LEVEL             = 0x24,
    BEKEN_DELAY_FOR_OBEX_PACKET_FINAL           = 0x25,
    BEKEN_DISABLE_SPREADTRUM_HCI                = 0x26,
    BEKEN_MAX_ACL_BUFF_NUMBER                   = 0x27,
    BEKEN_WRITE_ADDR_AFTER_RESET                = 0x28,

    BEKEN_ENABLE_MASTER_AFC                     = 0x2C,
    BEKEN_ENABLE_VIMicro_ENCRYPTION_ISSUE       = 0x2D,
    BEKEN_ENABLE_CPU_SPEED_FOR_ECC              = 0x2E,
    BEKEN_CHANGE_CPU_CLK                        = 0x2F,
    BEKEN_ENABLE_CSR_TX_CRC                     = 0x30,

    SLEEP_FOR_ATE_POWER_TEST                    = 0x32,
    BEKEN_DISALBE_HAREWARE_ERROR_LOG            = 0x33,
    BEKEN_ENABLE_ROLE_SWITCH                    = 0x34,
    BEKEN_SET_LMP_FEATURES                      = 0x35,
    BEKEN_SET_LMP_EXT_FEATURES                  = 0x36,
    BEKEN_DISALBE_EDR3                          = 0x37,
    BEKEN_DISALBE_2DH1_WHEN_AUTORATE            = 0x38,
    BEKEN_FORCE_DM1_WHEN_LITTLE_PACKET          = 0x39,
    BEKEN_ENABLE_QOS                            = 0x3A,
    //    BEKEN_DISABLE_ESCO                          =0x3A,
    BEKEN_DELAY_PTT_SET                         = 0x3B,
    BEKEN_SET_32K_WAKUP_TIME                    = 0x3C,
    BEKEN_CFG_MIN_SLOTS_FOR_SLEEP_PROCEDURE     = 0x3D,
    BEKEN_SET_HOST_WAKEUP_TIME                  = 0x3E,
    BEKEN_BT_ACTIVE_PIN_SEL                     = 0x3F,
    BEKEN_BT_PRIORITY_PIN_SEL                   = 0x40,
    BEKEN_WLAN_ACTIVE_PIN_SEL                   = 0x41,
    BEKEN_WLAN_ACTIVE_PIN_POL                   = 0x42,
    BEKEN_ENABLE_SOFTIRQ_FOR_UART               = 0x43,
    BEKEN_SET_T_SNIFF_MIN                       = 0x44,
    BEKEN_SET_T_SNIFF_MAX                       = 0x45,
    BEKEN_CHANGE_SNIFF_ATTEMP_WHEN_SNIFF_MIN_IS_SMALL   = 0x46,
    BEKEN_CHANGE_SNIFF_TIMEOUT_WHEN_SNIFF_MIN_IS_SMALL  = 0x47,
    BEKEN_PTA_TX_DELAY_AFTER_PRIORITY           = 0x48,
    BEKEN_PTA_TX_DELAY_AFTER_FREQ_OVERLAP       = 0x49,
    BEKEN_PTA_RX_DELAY_AFTER_PRIORITY           = 0x4A,
    BEKEN_ENABLE_PTA                            = 0x4B,
    BEKEN_ENABLE_CPU_SPEED_FOR_ECC2             = 0x4C,
    BEKEN_SPEED_UART_CRC                        = 0x4D,
    BEKEN_UART_MODULE_TEST_CMD                  = 0x50,
    BEKEN_UART_MODULE_SUB_TEST_CMD              = 0x51,
    BEKEN_UART_MODULE_GENERAL_CMD               = 0x52,
    BEKEN_ENABLE_AUTO_CHANGE_CPU_CLK            = 0x53,
    BEKEN_SET_SCO_USE_HCI                       = 0x82,
#if (DEBUG_BASEBAND_MONITORS == 1)
    BEKEN_READ_BASEBAND_MONITORS                = 0x90,
    BEKEN_RESET_BASEBAND_MONITORS               = 0x91,
#endif
#if (DEBUG_AGC_MODE_CHANNEL_ASSESSMENT == 1)
    BEKEN_AGC_MODE_PARAM                        = 0x92,
#endif
    BEKEN_FLASH_READ_CMD                        = 0xA0,
    BEKEN_FLASH_WRITE_CMD                       = 0xA1,
    BEKEN_FLASH_ERASE_CMD                       = 0xA2,
    BEKEN_SHOW_STACK_CMD                        = 0XAA,
    BEKEN_DUMP_ENV_CMD                          = 0xAB,
    BEKEN_SHOW_BT_STATUS                        = 0xAC,
    BEKEN_SHOW_BT_DEBUG                         = 0xAD,
    BEKEN_PRINT_LINK_KEY                        = 0XAE,
    BEKEN_ENTRY_DUT_MODE                        = 0XAF,
    BEKEN_READ_OTP_CMD                          = 0XA7,
    BEKEN_WRITE_OTP_CMD                         = 0XA8,    
    LOOP_MODE_CMD                               = 0XCC,
    BEKEN_TEMP_CMD                              = 0XDD,
    BEKEN_TEMP_TCP                              = 0XEE,
    BEKEN_TEMP_DETECT_CONFIG_CMD                = 0XEC,    
    BEKEN_TEST_UDP                              = 0XDE,
    BEKEN_SD_CLOSE                              = 0XDC,
};


#endif

/**
 * Process a command with arguments. We look up the command and execute it
 * if valid. Otherwise we print a usage message.
 *
 * @param flag		Some flags normally 0 (see CMD_FLAG_.. above)
 * @param argc		Number of arguments (arg 0 must be the command text)
 * @param argv		Arguments
 * @param repeatable	This function sets this to 0 if the command is not
 *			repeatable. If the command is repeatable, the value
 *			is left unchanged.
 * @param ticks		If ticks is not null, this function set it to the
 *			number of ticks the command took to complete.
 * @return 0 if the command succeeded, 1 if it failed
 */
int cmd_process(int flag, int argc, char *const argv[],
                int *repeatable, unsigned int *ticks);
int run_command(const char *cmd, int flag);
cmd_tbl_t *cmd_find_tbl(const char *cmd, cmd_tbl_t *table, int table_len);
int cmd_usage(const cmd_tbl_t *cmdtp);

#if CFG_SUPPORT_BKREG
extern int bkreg_run_command(const char *cmd, int flag);
#endif // CFG_SUPPORT_BKREG

#endif // _COMMAND_LINE_H_
