#ifndef _SPI_PUB_H_
#define _SPI_PUB_H_

#define SPI_FAILURE                (1)
#define SPI_SUCCESS                (0)

#define SPI_DEV_NAME                "spi"

#define SPI_CMD_MAGIC              (0xe250000)
enum
{
    CMD_SPI_UNIT_ENABLE = SPI_CMD_MAGIC + 1,
    CMD_SPI_SET_MSTEN,
    CMD_SPI_SET_CKPHA,
    CMD_SPI_SET_CKPOL,
    CMD_SPI_SET_BITWIDTH,
    CMD_SPI_SET_NSSID,
    CMD_SPI_SET_CKR,
    CMD_SPI_RXINT_EN,
    CMD_SPI_TXINT_EN,
    CMD_SPI_RXOVR_EN,
    CMD_SPI_TXOVR_EN,
    CMD_SPI_RXFIFO_CLR,
    CMD_SPI_TXFIFO_CLR,
    CMD_SPI_RXINT_MODE,
    CMD_SPI_TXINT_MODE,
    CMD_SPI_START_TRANS,
    CMD_SPI_INIT_MSTEN,
    CMD_SPI_GET_BUSY
};

typedef struct
{
    UINT8 tx_remain_data_cnt;
    UINT8 rx_remain_data_cnt;
    UINT8 *p_tx_buf;
    UINT8 *p_rx_buf;
    UINT8 trans_done;
} spi_trans_t;

typedef enum
{
    slave,
    master
} spi_msten;

extern volatile spi_trans_t spi_trans;
/*******************************************************************************
* Function Declarations
*******************************************************************************/
void spi_init(void);
void spi_exit(void);
void spi_isr(void);
#endif //_SPI_PUB_H_
