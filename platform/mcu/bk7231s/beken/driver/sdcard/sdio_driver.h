#ifndef __SDIO_DRIVER_H__
#define __SDIO_DRIVER_H__

#include "include.h"

#include "uart_pub.h"
#define SDCARD_DEBUG

#ifdef SDCARD_DEBUG
#define SDCARD_PRT      os_printf
#define SDCARD_WARN     warning_prf
#define SDCARD_FATAL    fatal_prf
#else
#define SDCARD_PRT      null_prf
#define SDCARD_WARN     null_prf
#define SDCARD_FATAL    null_prf
#endif

// SDCARD REG
#define SDCARD_BASE_ADDR                    (0x00802D00)

#define REG_SDCARD_CMD_SEND_CTRL            (SDCARD_BASE_ADDR + 0*4)
#define SDCARD_CMD_SEND_CTRL_CMD_START        (1 << 0)
#define SDCARD_CMD_SEND_CTRL_CMD_FLAGS_MASK   (0x7)
#define SDCARD_CMD_SEND_CTRL_CMD_FLAGS_POSI   (1)
#define SDCARD_CMD_SEND_CTRL_CMD_INDEX_MASK   (0x3f)
#define SDCARD_CMD_SEND_CTRL_CMD_INDEX_POSI   (4)

#define REG_SDCARD_CMD_SEND_AGUMENT         (SDCARD_BASE_ADDR + 1*4)
#define REG_SDCARD_CMD_RSP_TIMER            (SDCARD_BASE_ADDR + 2*4)

#define REG_SDCARD_DATA_REC_CTRL            (SDCARD_BASE_ADDR + 3*4)
#define SDCARD_DATA_REC_CTRL_DATA_EN          (1 << 0)
#define SDCARD_DATA_REC_CTRL_DATA_STOP_EN     (1 << 1)
#define SDCARD_DATA_REC_CTRL_DATA_BUS         (1 << 2)
#define SDCARD_DATA_REC_CTRL_DATA_MUL_BLK     (1 << 3)
#define SDCARD_DATA_REC_CTRL_BLK_SIZE_MASK    (0xfff)
#define SDCARD_DATA_REC_CTRL_BLK_SIZE_POSI    (4)
#define SDCARD_DATA_REC_CTRL_DATA_WR_DATA_EN  (1 << 16)
#define SDCARD_DATA_REC_CTRL_DATA_BYTE_SEL    (1 << 17)

#define REG_SDCARD_DATA_REC_TIMER           (SDCARD_BASE_ADDR + 4*4)
#define REG_SDCARD_CMD_RSP_AGUMENT0         (SDCARD_BASE_ADDR + 5*4)
#define REG_SDCARD_CMD_RSP_AGUMENT1         (SDCARD_BASE_ADDR + 6*4)
#define REG_SDCARD_CMD_RSP_AGUMENT2         (SDCARD_BASE_ADDR + 7*4)
#define REG_SDCARD_CMD_RSP_AGUMENT3         (SDCARD_BASE_ADDR + 8*4)

#define REG_SDCARD_CMD_RSP_INT_SEL          (SDCARD_BASE_ADDR + 9*4)
#define SDCARD_CMDRSP_NORSP_END_INT           (1 << 0)
#define SDCARD_CMDRSP_RSP_END_INT             (1 << 1)
#define SDCARD_CMDRSP_TIMEOUT_INT             (1 << 2)
#define SDCARD_CMDRSP_DATA_REC_END_INT        (1 << 3)
#define SDCARD_CMDRSP_DATA_WR_END_INT         (1 << 4)
#define SDCARD_CMDRSP_DATA_TIME_OUT_INT       (1 << 5)
#define SDCARD_CMDRSP_RX_FIFO_NEED_READ       (1 << 6)
#define SDCARD_CMDRSP_TX_FIFO_NEED_WRITE      (1 << 7)
#define SDCARD_CMDRSP_RX_OVERFLOW             (1 << 8)
#define SDCARD_CMDRSP_TX_FIFO_EMPTY           (1 << 9)
#define SDCARD_CMDRSP_CMD_CRC_OK              (1 << 10)
#define SDCARD_CMDRSP_CMD_CRC_FAIL            (1 << 11)
#define SDCARD_CMDRSP_DATA_CRC_OK             (1 << 12)
#define SDCARD_CMDRSP_DATA_CRC_FAIL           (1 << 13)
#define SDCARD_CMDRSP_RSP_INDEX               (0x3f<<14)
#define SDCARD_CMDRSP_WR_STATU                (0x7<<20)
#define SDCARD_CMDRSP_DATA_BUSY               (0x1<<23)

#define REG_SDCARD_CMD_RSP_INT_MASK         (SDCARD_BASE_ADDR + 10*4)
#define SDCARD_CMDRSP_NORSP_END_INT_MASK      (1 << 0)
#define SDCARD_CMDRSP_RSP_END_INT_MASK        (1 << 1)
#define SDCARD_CMDRSP_TIMEOUT_INT_MASK        (1 << 2)
#define SDCARD_CMDRSP_DATA_REC_END_INT_MASK   (1 << 3)
#define SDCARD_CMDRSP_DATA_WR_END_INT_MASK    (1 << 4)
#define SDCARD_CMDRSP_DATA_TIME_OUT_INT_MASK  (1 << 5)
#define SDCARD_CMDRSP_RX_FIFO_NEED_READ_MASK  (1 << 6)
#define SDCARD_CMDRSP_TX_FIFO_NEED_WRITE_MASK (1 << 7)
#define SDCARD_CMDRSP_RX_OVERFLOW_MASK        (1 << 8)
#define SDCARD_CMDRSP_TX_FIFO_EMPTY_MASK      (1 << 9)

#define REG_SDCARD_WR_DATA_ADDR             (SDCARD_BASE_ADDR + 11*4)
#define REG_SDCARD_RD_DATA_ADDR             (SDCARD_BASE_ADDR + 12*4)

#define REG_SDCARD_FIFO_THRESHOLD           (SDCARD_BASE_ADDR + 13*4)
#define SDCARD_FIFO_RX_FIFO_THRESHOLD_MASK   (0xff)
#define SDCARD_FIFO_RX_FIFO_THRESHOLD_POSI   (0)
#define SDCARD_FIFO_TX_FIFO_THRESHOLD_MASK   (0xff)
#define SDCARD_FIFO_TX_FIFO_THRESHOLD_POSI   (8)
#define SDCARD_FIFO_RX_FIFO_RST              (1 << 16)
#define SDCARD_FIFO_TX_FIFO_RST              (1 << 17)
#define SDCARD_FIFO_RXFIFO_RD_READY          (1 << 18)
#define SDCARD_FIFO_TXFIFO_WR_READY          (1 << 19)
#define SDCARD_FIFO_SD_STA_RST               (1 << 20)
#define SDCARD_FIFO_SD_RATE_SELECT_POSI      (21)
#define SDCARD_FIFO_SD_RATE_SELECT_MASK      (0x3)

// SDcard defination
/* Exported types ------------------------------------------------------------*/
typedef enum
{
    SD_OK   =   0,
    SD_CMD_CRC_FAIL               = (1), /*!< Command response received (but CRC check failed) */
    SD_DATA_CRC_FAIL              = (2), /*!< Data bock sent/received (CRC check Failed) */
    SD_CMD_RSP_TIMEOUT            = (3), /*!< Command response timeout */
    SD_DATA_TIMEOUT               = (4), /*!< Data time out */

    SD_INVALID_VOLTRANGE,
    SD_R5_ERROR,            /* A general or an unknown error occurred during the operation */
    SD_R5_ERR_FUNC_NUMB,    /* An invalid function number was requested */
    SD_R5_OUT_OF_RANGE,     /*The command's argument was out of the allowed range for this card*/
    SD_ERROR,
    SD_ERR_LONG_TIME_NO_RESPONS,
    SD_ERR_CMD41_CNT = 0xfffe
} SDIO_Error;


#define SD_CMD_NORESP             0
#define SD_CMD_SHORT             (CMD_FLAG_RESPONSE|CMD_FLAG_CRC_CHECK)
#define SD_CMD_LONG              (CMD_FLAG_RESPONSE|CMD_FLAG_LONG_CMD\
                                 |CMD_FLAG_CRC_CHECK)

#define SD_CMD_RSP               (SDCARD_CMDRSP_NORSP_END_INT\
                                 |SDCARD_CMDRSP_RSP_END_INT\
                                 |SDCARD_CMDRSP_TIMEOUT_INT\
                                 |SDCARD_CMDRSP_CMD_CRC_FAIL)

#define SD_DATA_RSP              (SDCARD_CMDRSP_DATA_REC_END_INT\
                                 |SDCARD_CMDRSP_DATA_CRC_FAIL\
                                 |SDCARD_CMDRSP_DATA_WR_END_INT\
                                 |SDCARD_CMDRSP_DATA_TIME_OUT_INT)


#define SD_DATA_DIR_RD           0
#define SD_DATA_DIR_WR           1

#define OCR_MSK_BUSY             0x80000000 // Busy flag
#define OCR_MSK_HC               0x40000000 // High Capacity flag
#define OCR_MSK_VOLTAGE_3_2V_3_3V           0x00100000 // Voltage 3.2V to 3.3V flag
#define OCR_MSK_VOLTAGE_ALL      0x00FF8000 // All Voltage flag

#define SD_DEFAULT_OCR           (OCR_MSK_VOLTAGE_ALL|OCR_MSK_HC)

#define SD_MAX_VOLT_TRIAL        (0xFF)

#define SD_DEFAULT_BLOCK_SIZE    512
#define SDCARD_TX_FIFO_THRD      (0x01) // 16byte
#define SDCARD_RX_FIFO_THRD      (0x01)

#define	CLK_26M                  0
#define	CLK_13M                  1
#define	CLK_6_5M                 2
#define	CLK_200K                 3

#define CMD_FLAG_RESPONSE        0x01
#define CMD_FLAG_LONG_CMD        0x02
#define CMD_FLAG_CRC_CHECK       0x04
#define CMD_FLAG_MASK            0x07

#define DEF_LOW_SPEED_CMD_TIMEOUT 100000 //200k -- about 5us (500ms)
#define DEF_LOW_SPEED_DATA_TIMEOUT 100000

#define DEF_HIGH_SPEED_CMD_TIMEOUT 20000000//26M - about 40ns (800ms)
#define DEF_HIGH_SPEED_DATA_TIMEOUT 10000000//about 400ms

#define DEF_CMD_TIME_OUT         0x3000000
#define DEF_DATA_TIME_OUT        0x3000000

#define SDIO_RD_DATA             0
#define SDIO_WR_DATA             1
#define SDIO_RD_AF_WR            2

#define SDIO_DEF_LINE_MODE       4
#define SDIO_DEF_WORK_CLK        13


#define	SD_CLK_PIN_TIMEOUT1				0x1000
#define	SD_CLK_PIN_TIMEOUT2				0x8000
#define SD_CARD_OFFLINE				    0
#define SD_CARD_ONLINE				    1
#define SD_DETECT_DEFAULT_GPIO                  31
//extern  uint8 SD_det_gpio_flag;

// interface function
void sdio_set_low_clk(void);
void sdio_set_high_clk(void);
void sdio_gpio_config(void);
void sdio_clk_config(UINT8 enable);
void sdio_register_reset(void);
void sdio_sendcmd_function( UINT8 cmd_index, UINT32 flag,
                            UINT32 timeout, VOID *arg );
SDIO_Error sdio_wait_cmd_response(UINT32 cmd);
void sdio_get_cmdresponse_argument(UINT8 num, UINT32 *resp);
void sdio_setup_data(UINT32 data_dir, UINT32 byte_len);
void sdio_set_data_timeout(UINT32 timeout);
void sdcard_set_host_buswidth_4line(void);
void sdcard_set_host_buswidth_1line(void);

SDIO_Error sdcard_wait_receive_data(UINT8 *receive_buf);
SDIO_Error sdcard_wait_write_end(void);
SDIO_Error sdcard_write_data(UINT8 *writebuff, UINT32 block);
void driver_sdcard_recv_data_start(int timeout );
//uint8 sd_clk_is_attached(void);
uint8 sd_is_attached(void);
void sdio_register_reenable(void);
int wait_Receive_Data(void);

#endif

