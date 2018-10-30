/*!
    \file  gd32f30x_usart.h
    \brief definitions for GD32F30x
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F30x
*/

#ifndef GD32F30X_USART_H
#define GD32F30X_USART_H

#include "gd32f30x.h"

/* USARTx(x=0,1,2)/UARTx(x=3,4) definitions */
#define USART1                        USART_BASE                     /*!< USART1 base address */
#define USART2                        (USART_BASE+0x00000400U)       /*!< USART2 base address */
#define UART3                         (USART_BASE+0x00000800U)       /*!< UART3 base address */
#define UART4                         (USART_BASE+0x00000C00U)       /*!< UART4 base address */
#define USART0                        (USART_BASE+0x0000F400U)       /*!< USART0 base address */

/* registers definitions */
#define USART_STAT0(usartx)           REG32((usartx) + 0x00U)        /*!< USART status register 0 */
#define USART_DATA(usartx)            REG32((usartx) + 0x04U)        /*!< USART data register */
#define USART_BAUD(usartx)            REG32((usartx) + 0x08U)        /*!< USART baud rate register */
#define USART_CTL0(usartx)            REG32((usartx) + 0x0CU)        /*!< USART control register 0 */
#define USART_CTL1(usartx)            REG32((usartx) + 0x10U)        /*!< USART control register 1 */
#define USART_CTL2(usartx)            REG32((usartx) + 0x14U)        /*!< USART control register 2 */
#define USART_GP(usartx)              REG32((usartx) + 0x18U)        /*!< USART guard time and prescaler register */
#define USART_CTL3(usartx)            REG32((usartx) + 0x80U)        /*!< USART control register 3 */
#define USART_RT(usartx)              REG32((usartx) + 0x84U)        /*!< USART receiver timeout register */
#define USART_STAT1(usartx)           REG32((usartx) + 0x88U)        /*!< USART status register 1 */

/* bits definitions */
/* USARTx_STAT0 */
#define USART_STAT0_PERR              BIT(0)       /*!< parity error flag */
#define USART_STAT0_FERR              BIT(1)       /*!< frame error flag */
#define USART_STAT0_NERR              BIT(2)       /*!< noise error flag */
#define USART_STAT0_ORERR             BIT(3)       /*!< overrun error */
#define USART_STAT0_IDLEF             BIT(4)       /*!< IDLE frame detected flag */
#define USART_STAT0_RBNE              BIT(5)       /*!< read data buffer not empty */
#define USART_STAT0_TC                BIT(6)       /*!< transmission complete */
#define USART_STAT0_TBE               BIT(7)       /*!< transmit data buffer empty */
#define USART_STAT0_LBDF              BIT(8)       /*!< LIN break detected flag */
#define USART_STAT0_CTSF              BIT(9)       /*!< CTS change flag */

/* USARTx_DATA */
#define USART_DATA_DATA               BITS(0,8)    /*!< transmit or read data value */

/* USARTx_BAUD */
#define USART_BAUD_FRADIV             BITS(0,3)    /*!< fraction part of baud-rate divider */
#define USART_BAUD_INTDIV             BITS(4,15)   /*!< integer part of baud-rate divider */

/* USARTx_CTL0 */
#define USART_CTL0_SBKCMD             BIT(0)       /*!< send break command */
#define USART_CTL0_RWU                BIT(1)       /*!< receiver wakeup from mute mode */
#define USART_CTL0_REN                BIT(2)       /*!< receiver enable */
#define USART_CTL0_TEN                BIT(3)       /*!< transmitter enable */
#define USART_CTL0_IDLEIE             BIT(4)       /*!< idle line detected interrupt enable */
#define USART_CTL0_RBNEIE             BIT(5)       /*!< read data buffer not empty interrupt and overrun error interrupt enable */
#define USART_CTL0_TCIE               BIT(6)       /*!< transmission complete interrupt enable */
#define USART_CTL0_TBEIE              BIT(7)       /*!< transmitter buffer empty interrupt enable */
#define USART_CTL0_PERRIE             BIT(8)       /*!< parity error interrupt enable */
#define USART_CTL0_PM                 BIT(9)       /*!< parity mode */
#define USART_CTL0_PCEN               BIT(10)      /*!< parity check function enable */
#define USART_CTL0_WM                 BIT(11)      /*!< wakeup method in mute mode */
#define USART_CTL0_WL                 BIT(12)      /*!< word length */
#define USART_CTL0_UEN                BIT(13)      /*!< USART enable */

/* USARTx_CTL1 */
#define USART_CTL1_ADDR               BITS(0,3)    /*!< address of USART */
#define USART_CTL1_LBLEN              BIT(5)       /*!< LIN break frame length */
#define USART_CTL1_LBDIE              BIT(6)       /*!< LIN break detected interrupt eanble */
#define USART_CTL1_CLEN               BIT(8)       /*!< CK length */
#define USART_CTL1_CPH                BIT(9)       /*!< CK phase */
#define USART_CTL1_CPL                BIT(10)      /*!< CK polarity */
#define USART_CTL1_CKEN               BIT(11)      /*!< CK pin enable */
#define USART_CTL1_STB                BITS(12,13)  /*!< STOP bits length */
#define USART_CTL1_LMEN               BIT(14)      /*!< LIN mode enable */

/* USARTx_CTL2 */
#define USART_CTL2_ERRIE              BIT(0)       /*!< error interrupt enable */
#define USART_CTL2_IREN               BIT(1)       /*!< IrDA mode enable */
#define USART_CTL2_IRLP               BIT(2)       /*!< IrDA low-power */
#define USART_CTL2_HDEN               BIT(3)       /*!< half-duplex enable */
#define USART_CTL2_NKEN               BIT(4)       /*!< NACK enable in smartcard mode */
#define USART_CTL2_SCEN               BIT(5)       /*!< smartcard mode enable */
#define USART_CTL2_DENR               BIT(6)       /*!< DMA request enable for reception */
#define USART_CTL2_DENT               BIT(7)       /*!< DMA request enable for transmission */
#define USART_CTL2_RTSEN              BIT(8)       /*!< RTS enable */
#define USART_CTL2_CTSEN              BIT(9)       /*!< CTS enable */
#define USART_CTL2_CTSIE              BIT(10)      /*!< CTS interrupt enable */

/* USARTx_GP */
#define USART_GP_PSC                  BITS(0,7)    /*!< prescaler value for dividing the system clock */
#define USART_GP_GUAT                 BITS(8,15)   /*!< guard time value in smartcard mode */
 
/* USARTx_CTL3 */
#define USART_CTL3_RTEN               BIT(0)       /*!< receiver timeout enable */
#define USART_CTL3_SCRTNUM            BITS(1,3)    /*!< smartcard auto-retry number */
#define USART_CTL3_RTIE               BIT(4)       /*!< interrupt enable bit of receive timeout event */
#define USART_CTL3_EBIE               BIT(5)       /*!< interrupt enable bit of end of block event */
#define USART_CTL3_RINV               BIT(8)       /*!< RX pin level inversion */
#define USART_CTL3_TINV               BIT(9)       /*!< TX pin level inversion */
#define USART_CTL3_DINV               BIT(10)      /*!< data bit level inversion */
#define USART_CTL3_MSBF               BIT(11)      /*!< most significant bit first */

/* USARTx_RT */
#define USART_RT_RT                   BITS(0,23)   /*!< receiver timeout threshold */
#define USART_RT_BL                   BITS(24,31)  /*!< block length */

/* USARTx_STAT1 */
#define USART_STAT1_RTF               BIT(11)      /*!< receiver timeout flag */
#define USART_STAT1_EBF               BIT(12)      /*!< end of block flag */
#define USART_STAT1_BSY               BIT(16)      /*!< busy flag */

/* constants definitions */
/* define the USART bit position and its register index offset */
#define USART_REGIDX_BIT(regidx, bitpos)    (((uint32_t)(regidx) << 6) | (uint32_t)(bitpos))
#define USART_REG_VAL(usartx, offset)       (REG32((usartx) + (((uint32_t)(offset) & 0xFFFFU) >> 6)))
#define USART_BIT_POS(val)                  ((uint32_t)(val) & 0x1FU)
#define USART_REGIDX_BIT2(regidx, bitpos, regidx2, bitpos2)   (((uint32_t)(regidx2) << 22) | (uint32_t)((bitpos2) << 16)\
                                                              | (((uint32_t)(regidx) << 6) | (uint32_t)(bitpos)))
#define USART_REG_VAL2(usartx, offset)       (REG32((usartx) + ((uint32_t)(offset) >> 22)))
#define USART_BIT_POS2(val)                  (((uint32_t)(val) & 0x1F0000U) >> 16)

/* register offset */
#define USART_STAT0_REG_OFFSET              0x00U        /*!< STAT0 register offset */
#define USART_STAT1_REG_OFFSET              0x88U        /*!< STAT1 register offset */
#define USART_CTL0_REG_OFFSET               0x0CU        /*!< CTL0 register offset */
#define USART_CTL1_REG_OFFSET               0x10U        /*!< CTL1 register offset */
#define USART_CTL2_REG_OFFSET               0x14U        /*!< CTL2 register offset */
#define USART_CTL3_REG_OFFSET               0x80U        /*!< CTL3 register offset */

/* USART flags */
typedef enum
{
    /* flags in STAT0 register */
    USART_FLAG_CTS = USART_REGIDX_BIT(USART_STAT0_REG_OFFSET, 9U),      /*!< CTS change flag */
    USART_FLAG_LBD = USART_REGIDX_BIT(USART_STAT0_REG_OFFSET, 8U),      /*!< LIN break detected flag */
    USART_FLAG_TBE = USART_REGIDX_BIT(USART_STAT0_REG_OFFSET, 7U),      /*!< transmit data buffer empty */
    USART_FLAG_TC = USART_REGIDX_BIT(USART_STAT0_REG_OFFSET, 6U),       /*!< transmission complete */
    USART_FLAG_RBNE = USART_REGIDX_BIT(USART_STAT0_REG_OFFSET, 5U),     /*!< read data buffer not empty */
    USART_FLAG_IDLE = USART_REGIDX_BIT(USART_STAT0_REG_OFFSET, 4U),     /*!< IDLE frame detected flag */
    USART_FLAG_ORERR = USART_REGIDX_BIT(USART_STAT0_REG_OFFSET, 3U),    /*!< overrun error */
    USART_FLAG_NERR = USART_REGIDX_BIT(USART_STAT0_REG_OFFSET, 2U),     /*!< noise error flag */
    USART_FLAG_FERR = USART_REGIDX_BIT(USART_STAT0_REG_OFFSET, 1U),     /*!< frame error flag */
    USART_FLAG_PERR = USART_REGIDX_BIT(USART_STAT0_REG_OFFSET, 0U),     /*!< parity error flag */
    /* flags in STAT1 register */
    USART_FLAG_BSY = USART_REGIDX_BIT(USART_STAT1_REG_OFFSET, 16U),     /*!< busy flag */
    USART_FLAG_EB = USART_REGIDX_BIT(USART_STAT1_REG_OFFSET, 12U),      /*!< end of block flag */
    USART_FLAG_RT = USART_REGIDX_BIT(USART_STAT1_REG_OFFSET, 11U),      /*!< receiver timeout flag */
}usart_flag_enum;

/* USART interrupt flags */
typedef enum
{
    /* interrupt flags in CTL0 register */
    USART_INT_FLAG_PERR = USART_REGIDX_BIT2(USART_CTL0_REG_OFFSET, 8U, USART_STAT0_REG_OFFSET, 0U),       /*!< parity error interrupt and flag */
    USART_INT_FLAG_TBE = USART_REGIDX_BIT2(USART_CTL0_REG_OFFSET, 7U, USART_STAT0_REG_OFFSET, 7U),        /*!< transmitter buffer empty interrupt and flag */
    USART_INT_FLAG_TC = USART_REGIDX_BIT2(USART_CTL0_REG_OFFSET, 6U, USART_STAT0_REG_OFFSET, 6U),         /*!< transmission complete interrupt and flag */
    USART_INT_FLAG_RBNE = USART_REGIDX_BIT2(USART_CTL0_REG_OFFSET, 5U, USART_STAT0_REG_OFFSET, 5U),       /*!< read data buffer not empty interrupt and flag */
    USART_INT_FLAG_RBNE_ORERR = USART_REGIDX_BIT2(USART_CTL0_REG_OFFSET, 5U, USART_STAT0_REG_OFFSET, 3U), /*!< read data buffer not empty interrupt and overrun error flag */
    USART_INT_FLAG_IDLE = USART_REGIDX_BIT2(USART_CTL0_REG_OFFSET, 4U, USART_STAT0_REG_OFFSET, 4U),       /*!< IDLE line detected interrupt and flag */
    /* interrupt flags in CTL1 register */
    USART_INT_FLAG_LBD = USART_REGIDX_BIT2(USART_CTL1_REG_OFFSET, 6U, USART_STAT0_REG_OFFSET, 8U),        /*!< LIN break detected interrupt and flag */
    /* interrupt flags in CTL2 register */
    USART_INT_FLAG_CTS = USART_REGIDX_BIT2(USART_CTL2_REG_OFFSET, 10U, USART_STAT0_REG_OFFSET, 9U),       /*!< CTS interrupt and flag */
    USART_INT_FLAG_ERR_ORERR = USART_REGIDX_BIT2(USART_CTL2_REG_OFFSET, 0U, USART_STAT0_REG_OFFSET, 3U),  /*!< error interrupt and overrun error */
    USART_INT_FLAG_ERR_NERR = USART_REGIDX_BIT2(USART_CTL2_REG_OFFSET, 0U, USART_STAT0_REG_OFFSET, 2U),   /*!< error interrupt and noise error flag */
    USART_INT_FLAG_ERR_FERR = USART_REGIDX_BIT2(USART_CTL2_REG_OFFSET, 0U, USART_STAT0_REG_OFFSET, 1U),   /*!< error interrupt and frame error flag */
    /* interrupt flags in CTL3 register */
    USART_INT_FLAG_EB = USART_REGIDX_BIT2(USART_CTL3_REG_OFFSET, 5U, USART_STAT1_REG_OFFSET, 12U),        /*!< interrupt enable bit of end of block event and flag */
    USART_INT_FLAG_RT = USART_REGIDX_BIT2(USART_CTL3_REG_OFFSET, 4U, USART_STAT1_REG_OFFSET, 11U),        /*!< interrupt enable bit of receive timeout event and flag */
}usart_interrupt_flag_enum;

/* USART interrupt enable or disable */
typedef enum
{
    /* interrupt in CTL0 register */
    USART_INT_PERR = USART_REGIDX_BIT(USART_CTL0_REG_OFFSET, 8U),        /*!< parity error interrupt */
    USART_INT_TBE = USART_REGIDX_BIT(USART_CTL0_REG_OFFSET, 7U),         /*!< transmitter buffer empty interrupt */
    USART_INT_TC = USART_REGIDX_BIT(USART_CTL0_REG_OFFSET, 6U),          /*!< transmission complete interrupt */
    USART_INT_RBNE = USART_REGIDX_BIT(USART_CTL0_REG_OFFSET, 5U),        /*!< read data buffer not empty interrupt and overrun error interrupt */
    USART_INT_IDLE = USART_REGIDX_BIT(USART_CTL0_REG_OFFSET, 4U),        /*!< IDLE line detected interrupt */
    /* interrupt in CTL1 register */
    USART_INT_LBD = USART_REGIDX_BIT(USART_CTL1_REG_OFFSET, 6U),         /*!< LIN break detected interrupt */
    /* interrupt in CTL2 register */
    USART_INT_CTS = USART_REGIDX_BIT(USART_CTL2_REG_OFFSET, 10U),        /*!< CTS interrupt */
    USART_INT_ERR = USART_REGIDX_BIT(USART_CTL2_REG_OFFSET, 0U),         /*!< error interrupt */
    /* interrupt in CTL3 register */
    USART_INT_EB = USART_REGIDX_BIT(USART_CTL3_REG_OFFSET, 5U),          /*!< end of block interrupt */
    USART_INT_RT = USART_REGIDX_BIT(USART_CTL3_REG_OFFSET, 4U),          /*!< receive timeout interrupt */
}usart_interrupt_enum;

/* USART invert configure */
typedef enum
{
    /* data bit level inversion */
    USART_DINV_ENABLE,                             /*!< data bit level inversion */
    USART_DINV_DISABLE,                            /*!< data bit level not inversion */
    /* TX pin level inversion */
    USART_TXPIN_ENABLE,                            /*!< TX pin level inversion */
    USART_TXPIN_DISABLE,                           /*!< TX pin level not inversion */
    /* RX pin level inversion */
    USART_RXPIN_ENABLE,                            /*!< RX pin level inversion */
    USART_RXPIN_DISABLE,                           /*!< RX pin level not inversion */
}usart_invert_enum;

/* USART receiver configure */
#define CTL0_REN(regval)              (BIT(2) & ((uint32_t)(regval) << 2))
#define USART_RECEIVE_ENABLE          CTL0_REN(1)                      /*!< enable receiver */
#define USART_RECEIVE_DISABLE         CTL0_REN(0)                      /*!< disable receiver */

/* USART transmitter configure */
#define CTL0_TEN(regval)              (BIT(3) & ((uint32_t)(regval) << 3))
#define USART_TRANSMIT_ENABLE         CTL0_TEN(1)                      /*!< enable transmitter */
#define USART_TRANSMIT_DISABLE        CTL0_TEN(0)                      /*!< disable transmitter */

/* USART parity bits definitions */
#define CTL0_PM(regval)               (BITS(9,10) & ((uint32_t)(regval) << 9))
#define USART_PM_NONE                 CTL0_PM(0)                       /*!< no parity */
#define USART_PM_EVEN                 CTL0_PM(2)                       /*!< even parity */
#define USART_PM_ODD                  CTL0_PM(3)                       /*!< odd parity */

/* USART wakeup method in mute mode */
#define CTL0_WM(regval)               (BIT(11) & ((uint32_t)(regval) << 11))
#define USART_WM_IDLE                 CTL0_WM(0)                       /*!< idle line */
#define USART_WM_ADDR                 CTL0_WM(1)                       /*!< address match */

/* USART word length definitions */
#define CTL0_WL(regval)               (BIT(12) & ((uint32_t)(regval) << 12))
#define USART_WL_8BIT                 CTL0_WL(0)                       /*!< 8 bits */
#define USART_WL_9BIT                 CTL0_WL(1)                       /*!< 9 bits */

/* USART stop bits definitions */
#define CTL1_STB(regval)              (BITS(12,13) & ((uint32_t)(regval) << 12))
#define USART_STB_1BIT                CTL1_STB(0)                      /*!< 1 bit */
#define USART_STB_0_5BIT              CTL1_STB(1)                      /*!< 0.5 bit */
#define USART_STB_2BIT                CTL1_STB(2)                      /*!< 2 bits */
#define USART_STB_1_5BIT              CTL1_STB(3)                      /*!< 1.5 bits */

/* USART LIN break frame length */
#define CTL1_LBLEN(regval)            (BIT(5) & ((uint32_t)(regval) << 5))
#define USART_LBLEN_10B               CTL1_LBLEN(0)                    /*!< 10 bits */
#define USART_LBLEN_11B               CTL1_LBLEN(1)                    /*!< 11 bits */

/* USART CK length */
#define CTL1_CLEN(regval)             (BIT(8) & ((uint32_t)(regval) << 8))
#define USART_CLEN_NONE               CTL1_CLEN(0)                     /*!< there are 7 CK pulses for an 8 bit frame and 8 CK pulses for a 9 bit frame */
#define USART_CLEN_EN                 CTL1_CLEN(1)                     /*!< there are 8 CK pulses for an 8 bit frame and 9 CK pulses for a 9 bit frame */

/* USART clock phase */
#define CTL1_CPH(regval)              (BIT(9) & ((uint32_t)(regval) << 9))
#define USART_CPH_1CK                 CTL1_CPH(0)                      /*!< first clock transition is the first data capture edge */
#define USART_CPH_2CK                 CTL1_CPH(1)                      /*!< second clock transition is the first data capture edge */

/* USART clock polarity */
#define CTL1_CPL(regval)              (BIT(10) & ((uint32_t)(regval) << 10))
#define USART_CPL_LOW                 CTL1_CPL(0)                      /*!< steady low value on CK pin */
#define USART_CPL_HIGH                CTL1_CPL(1)                      /*!< steady high value on CK pin */

/* USART DMA request for receive configure */
#define CLT2_DENR(regval)             (BIT(6) & ((uint32_t)(regval) << 6))
#define USART_DENR_ENABLE             CLT2_DENR(1)                     /*!< DMA request enable for reception */
#define USART_DENR_DISABLE            CLT2_DENR(0)                     /*!< DMA request disable for reception */

/* USART DMA request for transmission configure */
#define CLT2_DENT(regval)             (BIT(7) & ((uint32_t)(regval) << 7))
#define USART_DENT_ENABLE             CLT2_DENT(1)                     /*!< DMA request enable for transmission */
#define USART_DENT_DISABLE            CLT2_DENT(0)                     /*!< DMA request disable for transmission */

/* USART RTS configure */
#define CLT2_RTSEN(regval)            (BIT(8) & ((uint32_t)(regval) << 8))
#define USART_RTS_ENABLE              CLT2_RTSEN(1)                    /*!< RTS enable */
#define USART_RTS_DISABLE             CLT2_RTSEN(0)                    /*!< RTS disable */

/* USART CTS configure */
#define CLT2_CTSEN(regval)            (BIT(9) & ((uint32_t)(regval) << 9))
#define USART_CTS_ENABLE              CLT2_CTSEN(1)                    /*!< CTS enable */
#define USART_CTS_DISABLE             CLT2_CTSEN(0)                    /*!< CTS disable */

/* USART IrDA low-power enable */
#define CTL2_IRLP(regval)             (BIT(2) & ((uint32_t)(regval) << 2))
#define USART_IRLP_LOW                CTL2_IRLP(1)                     /*!< low-power */
#define USART_IRLP_NORMAL             CTL2_IRLP(0)                     /*!< normal */

/* USART data is transmitted/received with the LSB/MSB first */
#define CTL3_MSBF(regval)             (BIT(11) & ((uint32_t)(regval) << 11))
#define USART_MSBF_LSB                CTL3_MSBF(0)                     /*!< LSB first */
#define USART_MSBF_MSB                CTL3_MSBF(1)                     /*!< MSB first */

/* function declarations */
/* initialization functions */
/* reset USART */
void usart_deinit(uint32_t usart_periph);
/* configure USART baud rate value */
void usart_baudrate_set(uint32_t usart_periph, uint32_t baudval);
/* configure USART parity function */
void usart_parity_config(uint32_t usart_periph, uint32_t paritycfg);
/* configure USART word length */
void usart_word_length_set(uint32_t usart_periph, uint32_t wlen);
/* configure USART stop bit length */
void usart_stop_bit_set(uint32_t usart_periph, uint32_t stblen);

/* USART normal mode communication */
/* enable USART */
void usart_enable(uint32_t usart_periph);
/* disable USART */
void usart_disable(uint32_t usart_periph);
/* configure USART transmitter */
void usart_transmit_config(uint32_t usart_periph, uint32_t txconfig);
/* configure USART receiver */
void usart_receive_config(uint32_t usart_periph, uint32_t rxconfig);
/* data is transmitted/received with the LSB/MSB first */
void usart_data_first_config(uint32_t usart_periph, uint32_t msbf);
/* configure USART inverted */
void usart_invert_config(uint32_t usart_periph, usart_invert_enum invertpara);
/* enable receiver timeout */
void usart_receiver_timeout_enable(uint32_t usart_periph);
/* disable receiver timeout */
void usart_receiver_timeout_disable(uint32_t usart_periph);
/* configure receiver timeout threshold */
void usart_receiver_timeout_threshold_config(uint32_t usart_periph, uint32_t rtimeout);
/* USART transmit data function */
void usart_data_transmit(uint32_t usart_periph, uint32_t data);
/* USART receive data function */
uint16_t usart_data_receive(uint32_t usart_periph);

/* multi-processor communication */
/* configure address of the USART */
void usart_address_config(uint32_t usart_periph, uint8_t addr);
/* enable mute mode */
void usart_mute_mode_enable(uint32_t usart_periph);
/* disable mute mode */
void usart_mute_mode_disable(uint32_t usart_periph);
/* configure wakeup method in mute mode */
void usart_mute_mode_wakeup_config(uint32_t usart_periph, uint32_t wmethod);

/* LIN mode communication */
/* LIN mode enable */
void usart_lin_mode_enable(uint32_t usart_periph);
/* LIN mode disable */
void usart_lin_mode_disable(uint32_t usart_periph);
/* LIN break detection length */
void usart_lin_break_dection_length_config(uint32_t usart_periph, uint32_t lblen);
/* send break frame */
void usart_send_break(uint32_t usart_periph);

/* half-duplex communication */
/* half-duplex enable */
void usart_halfduplex_enable(uint32_t usart_periph);
/* half-duplex disable */
void usart_halfduplex_disable(uint32_t usart_periph);

/* synchronous communication */
/* clock enable */
void usart_synchronous_clock_enable(uint32_t usart_periph);
/* clock disable */
void usart_synchronous_clock_disable(uint32_t usart_periph);
/* configure usart synchronous mode parameters */
void usart_synchronous_clock_config(uint32_t usart_periph, uint32_t clen, uint32_t cph, uint32_t cpl);

/* smartcard communication */
/* guard time value configure in smartcard mode */
void usart_guard_time_config(uint32_t usart_periph,uint32_t guat);
/* smartcard mode enable */
void usart_smartcard_mode_enable(uint32_t usart_periph);
/* smartcard mode disable */
void usart_smartcard_mode_disable(uint32_t usart_periph);
/* NACK enable in smartcard mode */
void usart_smartcard_mode_nack_enable(uint32_t usart_periph);
/* NACK disable in smartcard mode */
void usart_smartcard_mode_nack_disable(uint32_t usart_periph);
/* smartcard auto-retry number configure */
void usart_smartcard_autoretry_config(uint32_t usart_periph, uint32_t scrtnum);
/* block length configure */
void usart_block_length_config(uint32_t usart_periph, uint32_t bl);

/* IrDA communication */
/* enable IrDA mode */
void usart_irda_mode_enable(uint32_t usart_periph);
/* disable IrDA mode */
void usart_irda_mode_disable(uint32_t usart_periph);
/* configure the peripheral clock prescaler */
void usart_prescaler_config(uint32_t usart_periph, uint8_t psc);
/* configure IrDA low-power */
void usart_irda_lowpower_config(uint32_t usart_periph, uint32_t irlp);

/* hardware flow communication */
/* configure hardware flow control RTS */
void usart_hardware_flow_rts_config(uint32_t usart_periph, uint32_t rtsconfig);
/* configure hardware flow control CTS */
void usart_hardware_flow_cts_config(uint32_t usart_periph, uint32_t ctsconfig);

/* configure USART DMA for reception */
void usart_dma_receive_config(uint32_t usart_periph, uint32_t dmacmd);
/* configure USART DMA for transmission */
void usart_dma_transmit_config(uint32_t usart_periph, uint32_t dmacmd);

/* flag functions */
/* get flag in STAT0/STAT1 register */
FlagStatus usart_flag_get(uint32_t usart_periph, usart_flag_enum flag);
/* clear flag in STAT0/STAT1 register */
void usart_flag_clear(uint32_t usart_periph, usart_flag_enum flag);

/* interrupt functions */
/* enable USART interrupt */
void usart_interrupt_enable(uint32_t usart_periph, uint32_t int_flag);
/* disable USART interrupt */
void usart_interrupt_disable(uint32_t usart_periph, uint32_t int_flag);
/* get USART interrupt and flag status */
FlagStatus usart_interrupt_flag_get(uint32_t usart_periph, uint32_t int_flag);
/* clear interrupt flag in STAT0/STAT1 register */
void usart_interrupt_flag_clear(uint32_t usart_periph, uint32_t flag);
#endif /* GD32F30x_USART_H */ 
