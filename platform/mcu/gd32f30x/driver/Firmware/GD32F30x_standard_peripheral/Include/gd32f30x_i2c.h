/*!
    \file  gd32f30x_i2c.h
    \brief definitions for the I2C
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F30x
*/

#ifndef GD32F30X_I2C_H
#define GD32F30X_I2C_H

#include "gd32f30x.h"

/* I2Cx(x=0,1) definitions */
#define I2C0                          I2C_BASE                   /*!< I2C0 base address */
#define I2C1                          (I2C_BASE+0x400U)          /*!< I2C1 base address */

/* registers definitions */
#define I2C_CTL0(i2cx)                REG32((i2cx) + 0x00U)      /*!< I2C control register 0 */
#define I2C_CTL1(i2cx)                REG32((i2cx) + 0x04U)      /*!< I2C control register 1 */
#define I2C_SADDR0(i2cx)              REG32((i2cx) + 0x08U)      /*!< I2C slave address register 0*/
#define I2C_SADDR1(i2cx)              REG32((i2cx) + 0x0CU)      /*!< I2C slave address register */
#define I2C_DATA(i2cx)                REG32((i2cx) + 0x10U)      /*!< I2C transfer buffer register */
#define I2C_STAT0(i2cx)               REG32((i2cx) + 0x14U)      /*!< I2C transfer status register 0 */
#define I2C_STAT1(i2cx)               REG32((i2cx) + 0x18U)      /*!< I2C transfer status register */
#define I2C_CKCFG(i2cx)               REG32((i2cx) + 0x1CU)      /*!< I2C clock configure register */
#define I2C_RT(i2cx)                  REG32((i2cx) + 0x20U)      /*!< I2C rise time register */
#define I2C_FMPCFG(i2cx)              REG32((i2cx) + 0x90U)      /*!< I2C fast-mode-plus configure register */

/* bits definitions */
/* I2Cx_CTL0 */
#define I2C_CTL0_I2CEN                BIT(0)        /*!< peripheral enable */
#define I2C_CTL0_SMBEN                BIT(1)        /*!< SMBus mode */
#define I2C_CTL0_SMBSEL               BIT(3)        /*!< SMBus type */
#define I2C_CTL0_ARPEN                BIT(4)        /*!< ARP enable */
#define I2C_CTL0_PECEN                BIT(5)        /*!< PEC enable */
#define I2C_CTL0_GCEN                 BIT(6)        /*!< general call enable */
#define I2C_CTL0_DISSTRC              BIT(7)        /*!< clock stretching disable (slave mode) */
#define I2C_CTL0_START                BIT(8)        /*!< start generation */
#define I2C_CTL0_STOP                 BIT(9)        /*!< stop generation */
#define I2C_CTL0_ACKEN                BIT(10)       /*!< acknowledge enable */
#define I2C_CTL0_POAP                 BIT(11)       /*!< acknowledge/PEC position (for data reception) */
#define I2C_CTL0_PECTRANS             BIT(12)       /*!< packet error checking */
#define I2C_CTL0_SALT                 BIT(13)       /*!< SMBus alert */
#define I2C_CTL0_SRESET               BIT(15)       /*!< software reset */

/* I2Cx_CTL1 */
#define I2C_CTL1_I2CCLK               BITS(0,6)     /*!< I2CCLK[6:0] bits (peripheral clock frequency) */
#define I2C_CTL1_ERRIE                BIT(8)        /*!< error interrupt inable */
#define I2C_CTL1_EVIE                 BIT(9)        /*!< event interrupt enable */
#define I2C_CTL1_BUFIE                BIT(10)       /*!< buffer interrupt enable */
#define I2C_CTL1_DMAON                BIT(11)       /*!< DMA requests enable */
#define I2C_CTL1_DMALST               BIT(12)       /*!< DMA last transfer */

/* I2Cx_SADDR0 */
#define I2C_SADDR0_ADDRESS0           BIT(0)        /*!< bit 0 of a 10-bit address */
#define I2C_SADDR0_ADDRESS            BITS(1,7)     /*!< 7-bit address or bits 7:1 of a 10-bit address */
#define I2C_SADDR0_ADDRESS_H          BITS(8,9)     /*!< highest two bits of a 10-bit address */
#define I2C_SADDR0_ADDFORMAT          BIT(15)       /*!< address mode for the I2C slave */

/* I2Cx_SADDR1 */
#define I2C_SADDR1_DUADEN             BIT(0)        /*!< aual-address mode switch */
#define I2C_SADDR1_ADDRESS2           BITS(1,7)     /*!< second I2C address for the slave in dual-address mode */

/* I2Cx_DATA */
#define I2C_DATA_TRB                  BITS(0,7)     /*!< 8-bit data register */

/* I2Cx_STAT0 */
#define I2C_STAT0_SBSEND              BIT(0)        /*!< start bit (master mode) */
#define I2C_STAT0_ADDSEND             BIT(1)        /*!< address sent (master mode)/matched (slave mode) */
#define I2C_STAT0_BTC                 BIT(2)        /*!< byte transfer finished */
#define I2C_STAT0_ADD10SEND           BIT(3)        /*!< 10-bit header sent (master mode) */
#define I2C_STAT0_STPDET              BIT(4)        /*!< stop detection (slave mode) */
#define I2C_STAT0_RBNE                BIT(6)        /*!< data register not empty (receivers) */
#define I2C_STAT0_TBE                 BIT(7)        /*!< data register empty (transmitters) */
#define I2C_STAT0_BERR                BIT(8)        /*!< bus error */
#define I2C_STAT0_LOSTARB             BIT(9)        /*!< arbitration lost (master mode) */
#define I2C_STAT0_AERR                BIT(10)       /*!< acknowledge failure */
#define I2C_STAT0_OUERR               BIT(11)       /*!< overrun/underrun */
#define I2C_STAT0_PECERR              BIT(12)       /*!< PEC error in reception */
#define I2C_STAT0_SMBTO               BIT(14)       /*!< timeout signal in SMBus mode */
#define I2C_STAT0_SMBALT              BIT(15)       /*!< SMBus alert status */

/* I2Cx_STAT1 */
#define I2C_STAT1_MASTER              BIT(0)        /*!< master/slave */
#define I2C_STAT1_I2CBSY              BIT(1)        /*!< bus busy */
#define I2C_STAT1_TRS                 BIT(2)        /*!< transmitter/receiver */
#define I2C_STAT1_RXGC                BIT(4)        /*!< general call address (slave mode) */
#define I2C_STAT1_DEFSMB              BIT(5)        /*!< SMBus device default address (slave mode) */
#define I2C_STAT1_HSTSMB              BIT(6)        /*!< SMBus host header (slave mode) */
#define I2C_STAT1_DUMODF              BIT(7)        /*!< dual flag (slave mode) */
#define I2C_STAT1_ECV                 BITS(8,15)    /*!< packet error checking value */

/* I2Cx_CKCFG */
#define I2C_CKCFG_CLKC                BITS(0,11)    /*!< clock control register in fast/standard mode or fast mode plus(master mode) */
#define I2C_CKCFG_DTCY                BIT(14)       /*!< duty cycle of fast mode or fast mode plus */
#define I2C_CKCFG_FAST                BIT(15)       /*!< I2C speed selection in master mode */

/* I2Cx_RT */
#define I2C_RT_RISETIME               BITS(0,6)     /*!< maximum rise time in fast/standard mode or fast mode plus(master mode) */

/* I2Cx_FMPCFG */
#define I2C_FMPCFG_FMPEN              BIT(0)        /*!< fast mode plus enable bit */

/* constants definitions */
/* SMBus/I2C mode switch and SMBus type selection */
#define I2C_I2CMODE_ENABLE            ((uint32_t)0x00000000U)                  /*!< I2C mode */
#define I2C_SMBUSMODE_ENABLE          I2C_CTL0_SMBEN                           /*!< SMBus mode */

/* SMBus/I2C mode switch and SMBus type selection */
#define I2C_SMBUS_DEVICE              ((uint32_t)0x00000000U)                  /*!< SMBus mode device type */
#define I2C_SMBUS_HOST                I2C_CTL0_SMBSEL                          /*!< SMBus mode host type */

/* I2C transfer direction */
#define I2C_RECEIVER                  ((uint32_t)0x00000001U)                  /*!< receiver */
#define I2C_TRANSMITTER               ((uint32_t)0xFFFFFFFEU)                  /*!< transmitter */

/* whether or not to send an ACK */
#define I2C_ACK_DISABLE               ((uint32_t)0x00000000U)                  /*!< ACK will be not sent */
#define I2C_ACK_ENABLE                ((uint32_t)0x00000001U)                  /*!< ACK will be sent */

/* I2C POAP position*/
#define I2C_ACKPOS_NEXT               ((uint32_t)0x00000000U)                  /*!< ACKEN bit decides whether or not to send ACK for the next byte */
#define I2C_ACKPOS_CURRENT            ((uint32_t)0x00000001U)                  /*!< ACKEN bit decides whether or not to send ACK or not for the current byte */

/* I2C dual-address mode switch */
#define I2C_DUADEN_DISABLE            ((uint32_t)0x00000000U)                  /*!< dual-address mode disabled */
#define I2C_DUADEN_ENABLE             ((uint32_t)0x00000001U)                  /*!< dual-address mode enabled */

/* whether or not to stretch SCL low */
#define I2C_SCLSTRETCH_ENABLE         ((uint32_t)0x00000000U)                  /*!< SCL stretching is enabled */
#define I2C_SCLSTRETCH_DISABLE        I2C_CTL0_DISSTRC                         /*!< SCL stretching is disabled */

/* whether or not to response to a general call */
#define I2C_GCEN_ENABLE               I2C_CTL0_GCEN                            /*!< slave will response to a general call */
#define I2C_GCEN_DISABLE              ((uint32_t)0x00000000U)                  /*!< slave will not response to a general call */

/* software reset I2C */
#define I2C_SRESET_SET                I2C_CTL0_SRESET                          /*!< I2C is under reset */
#define I2C_SRESET_RESET              ((uint32_t)0x00000000U)                  /*!< I2C is not under reset */

/* I2C DMA mode configure */
/* DMA mode switch */
#define I2C_DMA_ON                    I2C_CTL1_DMAON                           /*!< DMA mode enabled */
#define I2C_DMA_OFF                   ((uint32_t)0x00000000U)                  /*!< DMA mode disabled */

/* flag indicating DMA last transfer */
#define I2C_DMALST_ON                 I2C_CTL1_DMALST                          /*!< next DMA EOT is the last transfer */
#define I2C_DMALST_OFF                ((uint32_t)0x00000000U)                  /*!< next DMA EOT is not the last transfer */

/* I2C PEC configure */
/* PEC enable */
#define I2C_PEC_ENABLE                I2C_CTL0_PECEN                           /*!< PEC calculation on */
#define I2C_PEC_DISABLE               ((uint32_t)0x00000000U)                   /*!< PEC calculation off */

/* PEC transfer */
#define I2C_PECTRANS_ENABLE           I2C_CTL0_PECTRANS                        /*!< transfer PEC */
#define I2C_PECTRANS_DISABLE          ((uint32_t)0x00000000U)                  /*!< not transfer PEC value */

/* I2C SMBus configure */
/* issue or not alert through SMBA pin */
#define I2C_SALTSEND_ENABLE           I2C_CTL0_SALT                            /*!< issue alert through SMBA pin */
#define I2C_SALTSEND_DISABLE          ((uint32_t)0x00000000U)                  /*!< not issue alert through SMBA */

/* ARP protocol in SMBus switch */
#define I2C_ARP_ENABLE                I2C_CTL0_ARPEN                           /*!< ARP enable */
#define I2C_ARP_DISABLE               ((uint32_t)0x00000000U)                  /*!< ARP disable */

/* fast mode plus enable */
#define I2C_FAST_MODE_PLUS_ENABLE     I2C_FMPCFG_FMPEN                         /*!< fast mode plus enable */
#define I2C_FAST_MODE_PLUS_DISABLE    ((uint32_t)0x00000000U)                  /*!< fast mode plus disable */

/* transmit I2C data */
#define DATA_TRANS(regval)            (BITS(0,7) & ((uint32_t)(regval) << 0))

/* receive I2C data */
#define DATA_RECV(regval)             GET_BITS((uint32_t)(regval), 0, 7)

/* I2C flag definitions */
#define I2C_FLAG_SBSEND               BIT(0)                                   /*!< start condition sent out in master mode */
#define I2C_FLAG_ADDSEND              BIT(1)                                   /*!< address is sent in master mode or received and matches in slave mode */
#define I2C_FLAG_BTC                  BIT(2)                                   /*!< byte transmission finishes */
#define I2C_FLAG_ADD10SEND            BIT(3)                                   /*!< header of 10-bit address is sent in master mode */
#define I2C_FLAG_STPDET               BIT(4)                                   /*!< etop condition detected in slave mode */
#define I2C_FLAG_RBNE                 BIT(6)                                   /*!< I2C_DATA is not Empty during receiving */
#define I2C_FLAG_TBE                  BIT(7)                                   /*!< I2C_DATA is empty during transmitting */
#define I2C_FLAG_BERR                 BIT(8)                                   /*!< a bus error occurs indication a unexpected start or stop condition on I2C bus */
#define I2C_FLAG_LOSTARB              BIT(9)                                   /*!< arbitration lost in master mode */
#define I2C_FLAG_AERR                 BIT(10)                                  /*!< acknowledge error */
#define I2C_FLAG_OUERR                BIT(11)                                  /*!< over-run or under-run situation occurs in slave mode */
#define I2C_FLAG_PECERR               BIT(12)                                  /*!< PEC error when receiving data */
#define I2C_FLAG_SMBTO                BIT(14)                                  /*!< timeout signal in SMBus mode */
#define I2C_FLAG_SMBALT               BIT(15)                                  /*!< SMBus alert status */
#define I2C_FLAG_MASTER               (BIT(0)|BIT(31))                         /*!< a flag indicating whether I2C block is in master or slave mode */
#define I2C_FLAG_I2CBSY               (BIT(1)|BIT(31))                         /*!< busy flag */
#define I2C_FLAG_TRS                  (BIT(2)|BIT(31))                         /*!< whether the I2C is a transmitter or a receiver */
#define I2C_FLAG_RXGC                 (BIT(4)|BIT(31))                         /*!< general call address (00h) received */
#define I2C_FLAG_DEFSMB               (BIT(5)|BIT(31))                         /*!< default address of SMBus device */
#define I2C_FLAG_HSTSMB               (BIT(6)|BIT(31))                         /*!< SMBus host header detected in slave mode */
#define I2C_FLAG_DUMOD                (BIT(7)|BIT(31))                         /*!< dual flag in slave mode indicating which address is matched in dual-address mode */

/* I2C interrupt flags */
#define I2C_INT_FLAG_SBSEND           I2C_FLAG_SBSEND                          /*!< start condition sent out in master mode interrupt flag */
#define I2C_INT_FLAG_ADDSEND          I2C_FLAG_ADDSEND                         /*!< address is sent in master mode or received and matches in slave mode interrupt flag */
#define I2C_INT_FLAG_BTC              I2C_FLAG_BTC                             /*!< byte transmission finishes */
#define I2C_INT_FLAG_ADD10SEND        I2C_FLAG_ADD10SEND                       /*!< header of 10-bit address is sent in master mode interrupt flag */
#define I2C_INT_FLAG_STPDET           I2C_FLAG_STPDET                          /*!< stop condition detected in slave mode interrupt flag */
#define I2C_INT_FLAG_RBNE             I2C_FLAG_RBNE                            /*!< I2C_DATA is not Empty during receiving interrupt flag */
#define I2C_INT_FLAG_TBE              I2C_FLAG_TBE                             /*!< I2C_DATA is empty during transmitting interrupt flag */
#define I2C_INT_FLAG_BERR             I2C_FLAG_BERR                            /*!< a bus error occurs indication a unexpected start or stop condition on I2C bus interrupt flag */
#define I2C_INT_FLAG_LOSTARB          I2C_FLAG_LOSTARB                         /*!< arbitration lost in master mode interrupt flag */
#define I2C_INT_FLAG_AERR             I2C_FLAG_AERR                            /*!< acknowledge error interrupt flag */
#define I2C_INT_FLAG_OUERR            I2C_FLAG_OUERR                           /*!< over-run or under-run situation occurs in slave mode interrupt flag */
#define I2C_INT_FLAG_PECERR           I2C_FLAG_PECERR                          /*!< PEC error when receiving data interrupt flag */
#define I2C_INT_FLAG_SMBTO            I2C_FLAG_SMBTO                           /*!< timeout signal in SMBus mode interrupt flag */
#define I2C_INT_FLAG_SMBALT           I2C_FLAG_SMBALT                          /*!< SMBus Alert status interrupt flag */

/* I2C interrupt enable bit */
#define I2C_INT_ERR                   I2C_CTL1_ERRIE                           /*!< error interrupt enable */
#define I2C_INT_EV                    I2C_CTL1_EVIE                            /*!< event interrupt enable */
#define I2C_INT_BUF                   I2C_CTL1_BUFIE                           /*!< buffer interrupt enable */

/* I2C duty cycle in fast mode or fast mode plus */
#define CKCFG_DTCY(regval)            (BIT(14) & ((uint32_t)(regval) << 14))
#define I2C_DTCY_2                    CKCFG_DTCY(0)                            /*!< in I2C fast mode or fast mode plus Tlow/Thigh = 2 */
#define I2C_DTCY_16_9                 CKCFG_DTCY(1)                            /*!< in I2C fast mode or fast mode plus Tlow/Thigh = 16/9 */

/* address mode for the I2C slave */
#define SADDR0_ADDFORMAT(regval)      (BIT(15) & ((uint32_t)(regval) << 15))
#define I2C_ADDFORMAT_7BITS           SADDR0_ADDFORMAT(0)                      /*!< address:7 bits */
#define I2C_ADDFORMAT_10BITS          SADDR0_ADDFORMAT(1)                      /*!< address:10 bits */

/* function declarations */
/* reset I2C */
void i2c_deinit(uint32_t i2c_periph);
/* configure I2C clock */
void i2c_clock_config(uint32_t i2c_periph, uint32_t clkspeed, uint32_t dutycyc);
/* configure I2C address */
void i2c_mode_addr_config(uint32_t i2c_periph, uint32_t mode, uint32_t addformat, uint32_t addr);
/* SMBus type selection */
void i2c_smbus_type_config(uint32_t i2c_periph, uint32_t type);
/* whether or not to send an ACK */
void i2c_ack_config(uint32_t i2c_periph, uint32_t ack);
/* configure I2C POAP position */
void i2c_ackpos_config(uint32_t i2c_periph, uint32_t pos);
/* master send slave address */
void i2c_master_addressing(uint32_t i2c_periph, uint32_t addr, uint32_t trandirection);
/* dual-address mode switch */
void i2c_dualaddr_enable(uint32_t i2c_periph, uint32_t dualaddr);

/* enable I2C */
void i2c_enable(uint32_t i2c_periph);
/* disable I2C */
void i2c_disable(uint32_t i2c_periph);
/* generate a START condition on I2C bus */
void i2c_start_on_bus(uint32_t i2c_periph);
/* generate a STOP condition on I2C bus */
void i2c_stop_on_bus(uint32_t i2c_periph);
/* I2C transmit data function */
void i2c_data_transmit(uint32_t i2c_periph, uint8_t data);
/* I2C receive data function */
uint8_t i2c_data_receive(uint32_t i2c_periph);
/* I2C DMA mode enable */
void i2c_dma_enable(uint32_t i2c_periph, uint32_t dmastate);
/* flag indicating DMA last transfer */
void i2c_dma_last_transfer_enable(uint32_t i2c_periph, uint32_t dmalast);
/* whether to stretch SCL low when data is not ready in slave mode */
void i2c_stretch_scl_low_config(uint32_t i2c_periph, uint32_t stretchpara );
/* whether or not to response to a general call */
void i2c_slave_response_to_gcall_config(uint32_t i2c_periph, uint32_t gcallpara);
/* software reset I2C */
void i2c_software_reset_config(uint32_t i2c_periph, uint32_t sreset);

/* check I2C flag is set or not */
FlagStatus i2c_flag_get(uint32_t i2c_periph, uint32_t flag);
/* clear I2C flag */
void i2c_flag_clear(uint32_t i2c_periph, uint32_t flag);
/* enable I2C interrupt */
void i2c_interrupt_enable(uint32_t i2c_periph, uint32_t inttype);
/* disable I2C interrupt */
void i2c_interrupt_disable(uint32_t i2c_periph, uint32_t inttype);
/* check I2C interrupt flag */
FlagStatus i2c_interrupt_flag_get(uint32_t i2c_periph,uint32_t intflag);
/* clear I2C interrupt flag */
void i2c_interrupt_flag_clear(uint32_t i2c_periph,uint32_t intflag);

/* I2C PEC calculation on or off */
void i2c_pec_enable(uint32_t i2c_periph, uint32_t pecstate);
/* I2C whether to transfer PEC value */
void i2c_pec_transfer_enable(uint32_t i2c_periph, uint32_t pecpara);
/* packet error checking value */
uint8_t i2c_pec_value_get(uint32_t i2c_periph);
/* I2C issue alert through SMBA pin */
void i2c_smbus_issue_alert(uint32_t i2c_periph, uint32_t smbuspara);
/* I2C ARP protocol in SMBus switch */
void i2c_smbus_arp_enable(uint32_t i2c_periph, uint32_t arpstate);

#endif /* GD32F30X_I2C_H */
