#ifndef _SPI_H_
#define _SPI_H_

#define SPI_DEBUG

#ifdef SPI_DEBUG
#define SPI_PRT      os_printf
#define SPI_WARN     warning_prf
#define SPI_FATAL    fatal_prf
#else
#define SPI_PRT      null_prf
#define SPI_WARN     null_prf
#define SPI_FATAL    null_prf
#endif

#define SPI_BASE                           (0x00802700)

#define SPI_CTRL                           (SPI_BASE + 0 * 4)
#define TXINT_MODE                         (0x01UL << 0)
#define RXINT_MODE                         (0x01UL << 1)
#define TXFIFO_CLR                         (0x01UL << 2)
#define RXFIFO_CLR                         (0x01UL << 3)
#define TXOVR_EN                           (0x01UL << 4)
#define RXOVR_EN                           (0x01UL << 5)
#define TXINT_EN                           (0x01UL << 6)
#define RXINT_EN                           (0x01UL << 7)
#define SPI_CKR_POSI                       (8)
#define SPI_CKR_MASK                       (0xFF)
#define NSSMD_POSI                         (16)
#define NSSMD_MASK                         (0x3)
#define BIT_WDTH                           (0x01UL << 18)
#define CKPOL                              (0x01UL << 20)
#define CKPHA                              (0x01UL << 21)
#define MSTEN                              (0x01UL << 22)
#define SPIEN                              (0x01UL << 23)

#define SPI_STAT                           (SPI_BASE + 1 * 4)
#define TXFIFO_EMPTY                       (0x01UL << 0)
#define TXFIFO_FULL                        (0x01UL << 1)
#define RXFIFO_EMPTY                       (0x01UL << 2)
#define RXFIFO_FULL                        (0x01UL << 3)
#define TXINT                              (0x01UL << 8)
#define RXINT                              (0x01UL << 9)
#define MODF                               (0x01UL << 10)
#define TXOVR                              (0x01UL << 11)
#define RXOVR                              (0x01UL << 12)
#define SLVSEL                             (0x01UL << 14)
#define SPIBUSY                            (0x01UL << 15)

#define SPI_DAT                            (SPI_BASE + 2 * 4)
#define SPI_DAT_POSI                       (0)
#define SPI_DAT_MASK                       (0xFFFF)

typedef struct
{
    UINT8 txint_mode;
    UINT8 rxint_mode;
    UINT8 master;
    UINT8 nssmd;
} spi_message_t;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
UINT32 spi_ctrl(UINT32 cmd, void *param);
#endif //_SPI_H_
