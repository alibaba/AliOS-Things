#ifndef _RC_BEKEN_H_
#define _RC_BEKEN_H_

#define BASEADDR_RC_BEKEN                                       0x01050000

//ADDRC_BEKEN_REG0x0
#define ADDRC_BEKEN_REG0x0                                      *((volatile unsigned long *) (0x01050000+0x0*4))
#define POSRC_BEKEN_REG0x0_force_enable                         31
#define BITRC_BEKEN_REG0x0_force_enable                         0x80000000
#define SET_RC_BEKEN_REG0x0_force_enable(val)                   ADDRC_BEKEN_REG0x0 = ((ADDRC_BEKEN_REG0x0 & (~0x80000000)) | (val << 31))
#define SETF_RC_BEKEN_REG0x0_force_enable                       ADDRC_BEKEN_REG0x0 |= 0x80000000
#define CLRF_RC_BEKEN_REG0x0_force_enable                       ADDRC_BEKEN_REG0x0 &= ~0x80000000
#define GET_RC_BEKEN_REG0x0_force_enable                        ((ADDRC_BEKEN_REG0x0 & 0x80000000) >> 31)

#define POSRC_BEKEN_REG0x0_spi_reset                            30
#define BITRC_BEKEN_REG0x0_spi_reset                            0x40000000
#define SET_RC_BEKEN_REG0x0_spi_reset(val)                      ADDRC_BEKEN_REG0x0 = ((ADDRC_BEKEN_REG0x0 & (~0x40000000)) | (val << 30))
#define SETF_RC_BEKEN_REG0x0_spi_reset                          ADDRC_BEKEN_REG0x0 |= 0x40000000
#define CLRF_RC_BEKEN_REG0x0_spi_reset                          ADDRC_BEKEN_REG0x0 &= ~0x40000000
#define GET_RC_BEKEN_REG0x0_spi_reset                           ((ADDRC_BEKEN_REG0x0 & 0x40000000) >> 30)


#define POSRC_BEKEN_REG0x0_rc_state                             16
#define BITRC_BEKEN_REG0x0_rc_state                             0x70000
#define SET_RC_BEKEN_REG0x0_rc_state(val)                       ADDRC_BEKEN_REG0x0 = ((ADDRC_BEKEN_REG0x0 & (~0x70000)) | (val << 16))
#define GET_RC_BEKEN_REG0x0_rc_state                            ((ADDRC_BEKEN_REG0x0 & 0x70000) >> 16)


#define POSRC_BEKEN_REG0x0_ch0_shdn_stat                        12
#define BITRC_BEKEN_REG0x0_ch0_shdn_stat                        0x1000
#define SET_RC_BEKEN_REG0x0_ch0_shdn_stat(val)                  ADDRC_BEKEN_REG0x0 = ((ADDRC_BEKEN_REG0x0 & (~0x1000)) | (val << 12))
#define SETF_RC_BEKEN_REG0x0_ch0_shdn_stat                      ADDRC_BEKEN_REG0x0 |= 0x1000
#define CLRF_RC_BEKEN_REG0x0_ch0_shdn_stat                      ADDRC_BEKEN_REG0x0 &= ~0x1000
#define GET_RC_BEKEN_REG0x0_ch0_shdn_stat                       ((ADDRC_BEKEN_REG0x0 & 0x1000) >> 12)


#define POSRC_BEKEN_REG0x0_ch0_ld                               8
#define BITRC_BEKEN_REG0x0_ch0_ld                               0x100
#define SET_RC_BEKEN_REG0x0_ch0_ld(val)                         ADDRC_BEKEN_REG0x0 = ((ADDRC_BEKEN_REG0x0 & (~0x100)) | (val << 8))
#define SETF_RC_BEKEN_REG0x0_ch0_ld                             ADDRC_BEKEN_REG0x0 |= 0x100
#define CLRF_RC_BEKEN_REG0x0_ch0_ld                             ADDRC_BEKEN_REG0x0 &= ~0x100
#define GET_RC_BEKEN_REG0x0_ch0_ld                              ((ADDRC_BEKEN_REG0x0 & 0x100) >> 8)


#define POSRC_BEKEN_REG0x0_rc_en                                3
#define BITRC_BEKEN_REG0x0_rc_en                                0x8
#define SET_RC_BEKEN_REG0x0_rc_en(val)                          ADDRC_BEKEN_REG0x0 = ((ADDRC_BEKEN_REG0x0 & (~0x8)) | (val << 3))
#define SETF_RC_BEKEN_REG0x0_rc_en                              ADDRC_BEKEN_REG0x0 |= 0x8
#define CLRF_RC_BEKEN_REG0x0_rc_en                              ADDRC_BEKEN_REG0x0 &= ~0x8
#define GET_RC_BEKEN_REG0x0_rc_en                               ((ADDRC_BEKEN_REG0x0 & 0x8) >> 3)


#define POSRC_BEKEN_REG0x0_ch0_en                               0
#define BITRC_BEKEN_REG0x0_ch0_en                               0x1
#define SET_RC_BEKEN_REG0x0_ch0_en(val)                         ADDRC_BEKEN_REG0x0 = ((ADDRC_BEKEN_REG0x0 & (~0x1)) | (val << 0))
#define SETF_RC_BEKEN_REG0x0_ch0_en                             ADDRC_BEKEN_REG0x0 |= 0x1
#define CLRF_RC_BEKEN_REG0x0_ch0_en                             ADDRC_BEKEN_REG0x0 &= ~0x1
#define GET_RC_BEKEN_REG0x0_ch0_en                              (ADDRC_BEKEN_REG0x0 & 0x1)

//ADDRC_BEKEN_REG0x1
#define ADDRC_BEKEN_REG0x1                                      *((volatile unsigned long *) (0x01050000+0x1*4))
#define POSRC_BEKEN_REG0x1_prescaler                            28
#define BITRC_BEKEN_REG0x1_prescaler                            0xF0000000
#define SET_RC_BEKEN_REG0x1_prescaler(val)                      ADDRC_BEKEN_REG0x1 = ((ADDRC_BEKEN_REG0x1 & (~0xF0000000)) | (val << 28))
#define GET_RC_BEKEN_REG0x1_prescaler                           ((ADDRC_BEKEN_REG0x1 & 0xF0000000) >> 28)

#define POSRC_BEKEN_REG0x1_trx_reg_stat                         0
#define BITRC_BEKEN_REG0x1_trx_reg_stat                         0xFFFFFFF
#define SET_RC_BEKEN_REG0x1_trx_reg_stat(val)                   ADDRC_BEKEN_REG0x1 = ((ADDRC_BEKEN_REG0x1 & (~0xFFFFFFF)) | (val << 0))
#define GET_RC_BEKEN_REG0x1_trx_reg_stat                        (ADDRC_BEKEN_REG0x1 & 0xFFFFFFF)

//ADDRC_BEKEN_REG0x5
#define ADDRC_BEKEN_REG0x5                                      *((volatile unsigned long *) (0x01050000+0x5*4))

#define POSRC_BEKEN_REG0x5_ch0_outpower                         0
#define BITRC_BEKEN_REG0x5_ch0_outpower                         0xFF
#define SET_RC_BEKEN_REG0x5_ch0_outpower(val)                   ADDRC_BEKEN_REG0x5 = ((ADDRC_BEKEN_REG0x5 & (~0xFF)) | (val << 0))
#define GET_RC_BEKEN_REG0x5_ch0_outpower                        (ADDRC_BEKEN_REG0x5 & 0xFF)

//ADDRC_BEKEN_REG0x8
#define ADDRC_BEKEN_REG0x8                                      *((volatile unsigned long *) (0x01050000+0x8*4))

#define POSRC_BEKEN_REG0x8_CH0_RX_OFF_DELAY                     16
#define BITRC_BEKEN_REG0x8_CH0_RX_OFF_DELAY                     0xFF0000
#define SET_RC_BEKEN_REG0x8_CH0_RX_OFF_DELAY(val)               ADDRC_BEKEN_REG0x8 = ((ADDRC_BEKEN_REG0x8 & (~0xFF0000)) | (val << 16))
#define GET_RC_BEKEN_REG0x8_CH0_RX_OFF_DELAY                    ((ADDRC_BEKEN_REG0x8 & 0xFF0000) >> 16)


#define POSRC_BEKEN_REG0x8_CH0_RX_ON_DELAY                      0
#define BITRC_BEKEN_REG0x8_CH0_RX_ON_DELAY                      0xFF
#define SET_RC_BEKEN_REG0x8_CH0_RX_ON_DELAY(val)                ADDRC_BEKEN_REG0x8 = ((ADDRC_BEKEN_REG0x8 & (~0xFF)) | (val << 0))
#define GET_RC_BEKEN_REG0x8_CH0_RX_ON_DELAY                     (ADDRC_BEKEN_REG0x8 & 0xFF)

//ADDRC_BEKEN_REG0xb
#define ADDRC_BEKEN_REG0xb                                      *((volatile unsigned long *) (0x01050000+0xb*4))

#define POSRC_BEKEN_REG0xb_CH0_TX_OFF_DELAY                     16
#define BITRC_BEKEN_REG0xb_CH0_TX_OFF_DELAY                     0xFF0000
#define SET_RC_BEKEN_REG0xb_CH0_TX_OFF_DELAY(val)               ADDRC_BEKEN_REG0xb = ((ADDRC_BEKEN_REG0xb & (~0xFF0000)) | (val << 16))
#define GET_RC_BEKEN_REG0xb_CH0_TX_OFF_DELAY                    ((ADDRC_BEKEN_REG0xb & 0xFF0000) >> 16)


#define POSRC_BEKEN_REG0xb_CH0_TX_ON_DELAY                      0
#define BITRC_BEKEN_REG0xb_CH0_TX_ON_DELAY                      0xFF
#define SET_RC_BEKEN_REG0xb_CH0_TX_ON_DELAY(val)                ADDRC_BEKEN_REG0xb = ((ADDRC_BEKEN_REG0xb & (~0xFF)) | (val << 0))
#define GET_RC_BEKEN_REG0xb_CH0_TX_ON_DELAY                     (ADDRC_BEKEN_REG0xb & 0xFF)

//ADDRC_BEKEN_REG0xe
#define ADDRC_BEKEN_REG0xe                                      *((volatile unsigned long *) (0x01050000+0xe*4))

#define POSRC_BEKEN_REG0xe_CH0_PA_OFF_DELAY                     16
#define BITRC_BEKEN_REG0xe_CH0_PA_OFF_DELAY                     0xFF0000
#define SET_RC_BEKEN_REG0xe_CH0_PA_OFF_DELAY(val)               ADDRC_BEKEN_REG0xe = ((ADDRC_BEKEN_REG0xe & (~0xFF0000)) | (val << 16))
#define GET_RC_BEKEN_REG0xe_CH0_PA_OFF_DELAY                    ((ADDRC_BEKEN_REG0xe & 0xFF0000) >> 16)


#define POSRC_BEKEN_REG0xe_CH0_PA_ON_DELAY                      0
#define BITRC_BEKEN_REG0xe_CH0_PA_ON_DELAY                      0xFF
#define SET_RC_BEKEN_REG0xe_CH0_PA_ON_DELAY(val)                ADDRC_BEKEN_REG0xe = ((ADDRC_BEKEN_REG0xe & (~0xFF)) | (val << 0))
#define GET_RC_BEKEN_REG0xe_CH0_PA_ON_DELAY                     (ADDRC_BEKEN_REG0xe & 0xFF)

//ADDRC_BEKEN_REG0x11
#define ADDRC_BEKEN_REG0x11                                     *((volatile unsigned long *) (0x01050000+0x11*4))

#define POSRC_BEKEN_REG0x11_CH0_SHDN_OFF_DELAY                  16
#define BITRC_BEKEN_REG0x11_CH0_SHDN_OFF_DELAY                  0xFF0000
#define SET_RC_BEKEN_REG0x11_CH0_SHDN_OFF_DELAY(val)            ADDRC_BEKEN_REG0x11 = ((ADDRC_BEKEN_REG0x11 & (~0xFF0000)) | (val << 16))
#define GET_RC_BEKEN_REG0x11_CH0_SHDN_OFF_DELAY                 ((ADDRC_BEKEN_REG0x11 & 0xFF0000) >> 16)


#define POSRC_BEKEN_REG0x11_CH0_SHDN_ON_DELAY                   0
#define BITRC_BEKEN_REG0x11_CH0_SHDN_ON_DELAY                   0xFF
#define SET_RC_BEKEN_REG0x11_CH0_SHDN_ON_DELAY(val)             ADDRC_BEKEN_REG0x11 = ((ADDRC_BEKEN_REG0x11 & (~0xFF)) | (val << 0))
#define GET_RC_BEKEN_REG0x11_CH0_SHDN_ON_DELAY                  (ADDRC_BEKEN_REG0x11 & 0xFF)

//ADDRC_BEKEN_REG0x19
#define ADDRC_BEKEN_REG0x19                                     *((volatile unsigned long *) (0x01050000+0x19*4))

#define POSRC_BEKEN_REG0x19_F_CH0_EN                            16
#define BITRC_BEKEN_REG0x19_F_CH0_EN                            0x10000
#define SET_RC_BEKEN_REG0x19_F_CH0_EN(val)                      ADDRC_BEKEN_REG0x19 = ((ADDRC_BEKEN_REG0x19 & (~0x10000)) | (val << 16))
#define SETF_RC_BEKEN_REG0x19_F_CH0_EN                          ADDRC_BEKEN_REG0x19 |= 0x10000
#define CLRF_RC_BEKEN_REG0x19_F_CH0_EN                          ADDRC_BEKEN_REG0x19 &= ~0x10000
#define GET_RC_BEKEN_REG0x19_F_CH0_EN                           ((ADDRC_BEKEN_REG0x19 & 0x10000) >> 16)

#define POSRC_BEKEN_REG0x19_F_CH0_B                             8
#define BITRC_BEKEN_REG0x19_F_CH0_B                             0xFF00
#define SET_RC_BEKEN_REG0x19_F_CH0_B(val)                       ADDRC_BEKEN_REG0x19 = ((ADDRC_BEKEN_REG0x19 & (~0xFF00)) | (val << 8))
#define GET_RC_BEKEN_REG0x19_F_CH0_B                            ((ADDRC_BEKEN_REG0x19 & 0xFF00) >> 8)


#define POSRC_BEKEN_REG0x19_F_CH0_RXHP                          3
#define BITRC_BEKEN_REG0x19_F_CH0_RXHP                          0x8
#define SET_RC_BEKEN_REG0x19_F_CH0_RXHP(val)                    ADDRC_BEKEN_REG0x19 = ((ADDRC_BEKEN_REG0x19 & (~0x8)) | (val << 3))
#define SETF_RC_BEKEN_REG0x19_F_CH0_RXHP                        ADDRC_BEKEN_REG0x19 |= 0x8
#define CLRF_RC_BEKEN_REG0x19_F_CH0_RXHP                        ADDRC_BEKEN_REG0x19 &= ~0x8
#define GET_RC_BEKEN_REG0x19_F_CH0_RXHP                         ((ADDRC_BEKEN_REG0x19 & 0x8) >> 3)

#define POSRC_BEKEN_REG0x19_F_CH0_TXEN                          2
#define BITRC_BEKEN_REG0x19_F_CH0_TXEN                          0x4
#define SET_RC_BEKEN_REG0x19_F_CH0_TXEN(val)                    ADDRC_BEKEN_REG0x19 = ((ADDRC_BEKEN_REG0x19 & (~0x4)) | (val << 2))
#define SETF_RC_BEKEN_REG0x19_F_CH0_TXEN                        ADDRC_BEKEN_REG0x19 |= 0x4
#define CLRF_RC_BEKEN_REG0x19_F_CH0_TXEN                        ADDRC_BEKEN_REG0x19 &= ~0x4
#define GET_RC_BEKEN_REG0x19_F_CH0_TXEN                         ((ADDRC_BEKEN_REG0x19 & 0x4) >> 2)

#define POSRC_BEKEN_REG0x19_F_CH0_RXEN                          1
#define BITRC_BEKEN_REG0x19_F_CH0_RXEN                          0x2
#define SET_RC_BEKEN_REG0x19_F_CH0_RXEN(val)                    ADDRC_BEKEN_REG0x19 = ((ADDRC_BEKEN_REG0x19 & (~0x2)) | (val << 1))
#define SETF_RC_BEKEN_REG0x19_F_CH0_RXEN                        ADDRC_BEKEN_REG0x19 |= 0x2
#define CLRF_RC_BEKEN_REG0x19_F_CH0_RXEN                        ADDRC_BEKEN_REG0x19 &= ~0x2
#define GET_RC_BEKEN_REG0x19_F_CH0_RXEN                         ((ADDRC_BEKEN_REG0x19 & 0x2) >> 1)

#define POSRC_BEKEN_REG0x19_F_CH0_SHDN                          0
#define BITRC_BEKEN_REG0x19_F_CH0_SHDN                          0x1
#define SET_RC_BEKEN_REG0x19_F_CH0_SHDN(val)                    ADDRC_BEKEN_REG0x19 = ((ADDRC_BEKEN_REG0x19 & (~0x1)) | (val << 0))
#define SETF_RC_BEKEN_REG0x19_F_CH0_SHDN                        ADDRC_BEKEN_REG0x19 |= 0x1
#define CLRF_RC_BEKEN_REG0x19_F_CH0_SHDN                        ADDRC_BEKEN_REG0x19 &= ~0x1
#define GET_RC_BEKEN_REG0x19_F_CH0_SHDN                         (ADDRC_BEKEN_REG0x19 & 0x1)

//ADDRC_BEKEN_REG0x1c
#define ADDRC_BEKEN_REG0x1c                                     *((volatile unsigned long *) (0x01050000+0x1c*4))

#define POSRC_BEKEN_REG0x1c_F_TX_ON                             1
#define BITRC_BEKEN_REG0x1c_F_TX_ON                             0x2
#define SET_RC_BEKEN_REG0x1c_F_TX_ON(val)                       ADDRC_BEKEN_REG0x1c = ((ADDRC_BEKEN_REG0x1c & (~0x2)) | (val << 1))
#define SETF_RC_BEKEN_REG0x1c_F_TX_ON                           ADDRC_BEKEN_REG0x1c |= 0x2
#define CLRF_RC_BEKEN_REG0x1c_F_TX_ON                           ADDRC_BEKEN_REG0x1c &= ~0x2
#define GET_RC_BEKEN_REG0x1c_F_TX_ON                            ((ADDRC_BEKEN_REG0x1c & 0x2) >> 1)

#define POSRC_BEKEN_REG0x1c_F_RX_ON                             0
#define BITRC_BEKEN_REG0x1c_F_RX_ON                             0x1
#define SET_RC_BEKEN_REG0x1c_F_RX_ON(val)                       ADDRC_BEKEN_REG0x1c = ((ADDRC_BEKEN_REG0x1c & (~0x1)) | (val << 0))
#define SETF_RC_BEKEN_REG0x1c_F_RX_ON                           ADDRC_BEKEN_REG0x1c |= 0x1
#define CLRF_RC_BEKEN_REG0x1c_F_RX_ON                           ADDRC_BEKEN_REG0x1c &= ~0x1
#define GET_RC_BEKEN_REG0x1c_F_RX_ON                            (ADDRC_BEKEN_REG0x1c & 0x1)

//ADDRC_BEKEN_REG0x1e
#define ADDRC_BEKEN_REG0x1e                                     *((volatile unsigned long *) (0x01050000+0x1e*4))

#define POSRC_BEKEN_REG0x1e_FE_RX_ON_DEL                        0
#define BITRC_BEKEN_REG0x1e_FE_RX_ON_DEL                        0xFFF
#define SET_RC_BEKEN_REG0x1e_FE_RX_ON_DEL(val)                  ADDRC_BEKEN_REG0x1e = ((ADDRC_BEKEN_REG0x1e & (~0xFFF)) | (val << 0))
#define GET_RC_BEKEN_REG0x1e_FE_RX_ON_DEL                       (ADDRC_BEKEN_REG0x1e & 0xFFF)

//ADDRC_BEKEN_REG0x3c
#define ADDRC_BEKEN_REG0x3c                                     *((volatile unsigned long *) (0x01050000+0x3c*4))
#define POSRC_BEKEN_REG0x3c_RX_DC_CAL_EN                        31
#define BITRC_BEKEN_REG0x3c_RX_DC_CAL_EN                        0x80000000
#define SET_RC_BEKEN_REG0x3c_RX_DC_CAL_EN(val)                  ADDRC_BEKEN_REG0x3c = ((ADDRC_BEKEN_REG0x3c & (~0x80000000)) | (val << 31))
#define SETF_RC_BEKEN_REG0x3c_RX_DC_CAL_EN                      ADDRC_BEKEN_REG0x3c |= 0x80000000
#define CLRF_RC_BEKEN_REG0x3c_RX_DC_CAL_EN                      ADDRC_BEKEN_REG0x3c &= ~0x80000000
#define GET_RC_BEKEN_REG0x3c_RX_DC_CAL_EN                       ((ADDRC_BEKEN_REG0x3c & 0x80000000) >> 31)

#define POSRC_BEKEN_REG0x3c_RX_AVG_MODE                         30
#define BITRC_BEKEN_REG0x3c_RX_AVG_MODE                         0x40000000
#define SET_RC_BEKEN_REG0x3c_RX_AVG_MODE(val)                   ADDRC_BEKEN_REG0x3c = ((ADDRC_BEKEN_REG0x3c & (~0x40000000)) | (val << 30))
#define SETF_RC_BEKEN_REG0x3c_RX_AVG_MODE                       ADDRC_BEKEN_REG0x3c |= 0x40000000
#define CLRF_RC_BEKEN_REG0x3c_RX_AVG_MODE                       ADDRC_BEKEN_REG0x3c &= ~0x40000000
#define GET_RC_BEKEN_REG0x3c_RX_AVG_MODE                        ((ADDRC_BEKEN_REG0x3c & 0x40000000) >> 30)

#define POSRC_BEKEN_REG0x3c_RX_IQ_SWAP                          29
#define BITRC_BEKEN_REG0x3c_RX_IQ_SWAP                          0x20000000
#define SET_RC_BEKEN_REG0x3c_RX_IQ_SWAP(val)                    ADDRC_BEKEN_REG0x3c = ((ADDRC_BEKEN_REG0x3c & (~0x20000000)) | (val << 29))
#define SETF_RC_BEKEN_REG0x3c_RX_IQ_SWAP                        ADDRC_BEKEN_REG0x3c |= 0x20000000
#define CLRF_RC_BEKEN_REG0x3c_RX_IQ_SWAP                        ADDRC_BEKEN_REG0x3c &= ~0x20000000
#define GET_RC_BEKEN_REG0x3c_RX_IQ_SWAP                         ((ADDRC_BEKEN_REG0x3c & 0x20000000) >> 29)

#define POSRC_BEKEN_REG0x3c_RX_HPF_BYPASS                       28
#define BITRC_BEKEN_REG0x3c_RX_HPF_BYPASS                       0x10000000
#define SET_RC_BEKEN_REG0x3c_RX_HPF_BYPASS(val)                 ADDRC_BEKEN_REG0x3c = ((ADDRC_BEKEN_REG0x3c & (~0x10000000)) | (val << 28))
#define SETF_RC_BEKEN_REG0x3c_RX_HPF_BYPASS                     ADDRC_BEKEN_REG0x3c |= 0x10000000
#define CLRF_RC_BEKEN_REG0x3c_RX_HPF_BYPASS                     ADDRC_BEKEN_REG0x3c &= ~0x10000000
#define GET_RC_BEKEN_REG0x3c_RX_HPF_BYPASS                      ((ADDRC_BEKEN_REG0x3c & 0x10000000) >> 28)


#define POSRC_BEKEN_REG0x3c_RX_AVG_I_RD                         12
#define BITRC_BEKEN_REG0x3c_RX_AVG_I_RD                         0xFFF000
#define SET_RC_BEKEN_REG0x3c_RX_AVG_I_RD(val)                   ADDRC_BEKEN_REG0x3c = ((ADDRC_BEKEN_REG0x3c & (~0xFFF000)) | (val << 12))
#define GET_RC_BEKEN_REG0x3c_RX_AVG_I_RD                        ((ADDRC_BEKEN_REG0x3c & 0xFFF000) >> 12)

#define POSRC_BEKEN_REG0x3c_RX_AVG_Q_RD                         0
#define BITRC_BEKEN_REG0x3c_RX_AVG_Q_RD                         0xFFF
#define SET_RC_BEKEN_REG0x3c_RX_AVG_Q_RD(val)                   ADDRC_BEKEN_REG0x3c = ((ADDRC_BEKEN_REG0x3c & (~0xFFF)) | (val << 0))
#define GET_RC_BEKEN_REG0x3c_RX_AVG_Q_RD                        (ADDRC_BEKEN_REG0x3c & 0xFFF)

//ADDRC_BEKEN_REG0x3e
#define ADDRC_BEKEN_REG0x3e                                     *((volatile unsigned long *) (0x01050000+0x3e*4))
#define POSRC_BEKEN_REG0x3e_RX_CAL_EN                           31
#define BITRC_BEKEN_REG0x3e_RX_CAL_EN                           0x80000000
#define SET_RC_BEKEN_REG0x3e_RX_CAL_EN(val)                     ADDRC_BEKEN_REG0x3e = ((ADDRC_BEKEN_REG0x3e & (~0x80000000)) | (val << 31))
#define SETF_RC_BEKEN_REG0x3e_RX_CAL_EN                         ADDRC_BEKEN_REG0x3e |= 0x80000000
#define CLRF_RC_BEKEN_REG0x3e_RX_CAL_EN                         ADDRC_BEKEN_REG0x3e &= ~0x80000000
#define GET_RC_BEKEN_REG0x3e_RX_CAL_EN                          ((ADDRC_BEKEN_REG0x3e & 0x80000000) >> 31)

#define POSRC_BEKEN_REG0x3e_RX_COMP_EN                          30
#define BITRC_BEKEN_REG0x3e_RX_COMP_EN                          0x40000000
#define SET_RC_BEKEN_REG0x3e_RX_COMP_EN(val)                    ADDRC_BEKEN_REG0x3e = ((ADDRC_BEKEN_REG0x3e & (~0x40000000)) | (val << 30))
#define SETF_RC_BEKEN_REG0x3e_RX_COMP_EN                        ADDRC_BEKEN_REG0x3e |= 0x40000000
#define CLRF_RC_BEKEN_REG0x3e_RX_COMP_EN                        ADDRC_BEKEN_REG0x3e &= ~0x40000000
#define GET_RC_BEKEN_REG0x3e_RX_COMP_EN                         ((ADDRC_BEKEN_REG0x3e & 0x40000000) >> 30)


#define POSRC_BEKEN_REG0x3e_RX_DC_I_RD                          12
#define BITRC_BEKEN_REG0x3e_RX_DC_I_RD                          0xFFF000
#define SET_RC_BEKEN_REG0x3e_RX_DC_I_RD(val)                    ADDRC_BEKEN_REG0x3e = ((ADDRC_BEKEN_REG0x3e & (~0xFFF000)) | (val << 12))
#define GET_RC_BEKEN_REG0x3e_RX_DC_I_RD                         ((ADDRC_BEKEN_REG0x3e & 0xFFF000) >> 12)

#define POSRC_BEKEN_REG0x3e_RX_DC_Q_RD                          0
#define BITRC_BEKEN_REG0x3e_RX_DC_Q_RD                          0xFFF
#define SET_RC_BEKEN_REG0x3e_RX_DC_Q_RD(val)                    ADDRC_BEKEN_REG0x3e = ((ADDRC_BEKEN_REG0x3e & (~0xFFF)) | (val << 0))
#define GET_RC_BEKEN_REG0x3e_RX_DC_Q_RD                         (ADDRC_BEKEN_REG0x3e & 0xFFF)

//ADDRC_BEKEN_REG0x3f
#define ADDRC_BEKEN_REG0x3f                                     *((volatile unsigned long *) (0x01050000+0x3f*4))

#define POSRC_BEKEN_REG0x3f_RX_AMP_ERR_RD                       16
#define BITRC_BEKEN_REG0x3f_RX_AMP_ERR_RD                       0x3FF0000
#define SET_RC_BEKEN_REG0x3f_RX_AMP_ERR_RD(val)                 ADDRC_BEKEN_REG0x3f = ((ADDRC_BEKEN_REG0x3f & (~0x3FF0000)) | (val << 16))
#define GET_RC_BEKEN_REG0x3f_RX_AMP_ERR_RD                      ((ADDRC_BEKEN_REG0x3f & 0x3FF0000) >> 16)


#define POSRC_BEKEN_REG0x3f_RX_PHASE_ERR_RD                     0
#define BITRC_BEKEN_REG0x3f_RX_PHASE_ERR_RD                     0x3FF
#define SET_RC_BEKEN_REG0x3f_RX_PHASE_ERR_RD(val)               ADDRC_BEKEN_REG0x3f = ((ADDRC_BEKEN_REG0x3f & (~0x3FF)) | (val << 0))
#define GET_RC_BEKEN_REG0x3f_RX_PHASE_ERR_RD                    (ADDRC_BEKEN_REG0x3f & 0x3FF)

//ADDRC_BEKEN_REG0x40
#define ADDRC_BEKEN_REG0x40                                     *((volatile unsigned long *) (0x01050000+0x40*4))

#define POSRC_BEKEN_REG0x40_RX_TY2_RD                           0
#define BITRC_BEKEN_REG0x40_RX_TY2_RD                           0x3FF
#define SET_RC_BEKEN_REG0x40_RX_TY2_RD(val)                     ADDRC_BEKEN_REG0x40 = ((ADDRC_BEKEN_REG0x40 & (~0x3FF)) | (val << 0))
#define GET_RC_BEKEN_REG0x40_RX_TY2_RD                          (ADDRC_BEKEN_REG0x40 & 0x3FF)

//ADDRC_BEKEN_REG0x41
#define ADDRC_BEKEN_REG0x41                                     *((volatile unsigned long *) (0x01050000+0x41*4))

#define POSRC_BEKEN_REG0x41_RX_DC_I_WR                          12
#define BITRC_BEKEN_REG0x41_RX_DC_I_WR                          0xFFF000
#define SET_RC_BEKEN_REG0x41_RX_DC_I_WR(val)                    ADDRC_BEKEN_REG0x41 = ((ADDRC_BEKEN_REG0x41 & (~0xFFF000)) | (val << 12))
#define GET_RC_BEKEN_REG0x41_RX_DC_I_WR                         ((ADDRC_BEKEN_REG0x41 & 0xFFF000) >> 12)

#define POSRC_BEKEN_REG0x41_RX_DC_Q_WR                          0
#define BITRC_BEKEN_REG0x41_RX_DC_Q_WR                          0xFFF
#define SET_RC_BEKEN_REG0x41_RX_DC_Q_WR(val)                    ADDRC_BEKEN_REG0x41 = ((ADDRC_BEKEN_REG0x41 & (~0xFFF)) | (val << 0))
#define GET_RC_BEKEN_REG0x41_RX_DC_Q_WR                         (ADDRC_BEKEN_REG0x41 & 0xFFF)

//ADDRC_BEKEN_REG0x42
#define ADDRC_BEKEN_REG0x42                                     *((volatile unsigned long *) (0x01050000+0x42*4))

#define POSRC_BEKEN_REG0x42_RX_AMP_ERR_WR                       16
#define BITRC_BEKEN_REG0x42_RX_AMP_ERR_WR                       0x3FF0000
#define SET_RC_BEKEN_REG0x42_RX_AMP_ERR_WR(val)                 ADDRC_BEKEN_REG0x42 = ((ADDRC_BEKEN_REG0x42 & (~0x3FF0000)) | (val << 16))
#define GET_RC_BEKEN_REG0x42_RX_AMP_ERR_WR                      ((ADDRC_BEKEN_REG0x42 & 0x3FF0000) >> 16)


#define POSRC_BEKEN_REG0x42_RX_PHASE_ERR_WR                     0
#define BITRC_BEKEN_REG0x42_RX_PHASE_ERR_WR                     0x3FF
#define SET_RC_BEKEN_REG0x42_RX_PHASE_ERR_WR(val)               ADDRC_BEKEN_REG0x42 = ((ADDRC_BEKEN_REG0x42 & (~0x3FF)) | (val << 0))
#define GET_RC_BEKEN_REG0x42_RX_PHASE_ERR_WR                    (ADDRC_BEKEN_REG0x42 & 0x3FF)

//ADDRC_BEKEN_REG0x4c
#define ADDRC_BEKEN_REG0x4c                                     *((volatile unsigned long *) (0x01050000+0x4c*4))
#define POSRC_BEKEN_REG0x4c_TEST_PATTERN                        30
#define BITRC_BEKEN_REG0x4c_TEST_PATTERN                        0xC0000000
#define SET_RC_BEKEN_REG0x4c_TEST_PATTERN(val)                  ADDRC_BEKEN_REG0x4c = ((ADDRC_BEKEN_REG0x4c & (~0xC0000000)) | (val << 30))
#define GET_RC_BEKEN_REG0x4c_TEST_PATTERN                       ((ADDRC_BEKEN_REG0x4c & 0xC0000000) >> 30)

#define POSRC_BEKEN_REG0x4c_TX_COMP_DIS                         29
#define BITRC_BEKEN_REG0x4c_TX_COMP_DIS                         0x20000000
#define SET_RC_BEKEN_REG0x4c_TX_COMP_DIS(val)                   ADDRC_BEKEN_REG0x4c = ((ADDRC_BEKEN_REG0x4c & (~0x20000000)) | (val << 29))
#define SETF_RC_BEKEN_REG0x4c_TX_COMP_DIS                       ADDRC_BEKEN_REG0x4c |= 0x20000000
#define CLRF_RC_BEKEN_REG0x4c_TX_COMP_DIS                       ADDRC_BEKEN_REG0x4c &= ~0x20000000
#define GET_RC_BEKEN_REG0x4c_TX_COMP_DIS                        ((ADDRC_BEKEN_REG0x4c & 0x20000000) >> 29)

#define POSRC_BEKEN_REG0x4c_DPD_EN                              28
#define BITRC_BEKEN_REG0x4c_DPD_EN                              0x10000000
#define SET_RC_BEKEN_REG0x4c_DPD_EN(val)                        ADDRC_BEKEN_REG0x4c = ((ADDRC_BEKEN_REG0x4c & (~0x10000000)) | (val << 28))
#define SETF_RC_BEKEN_REG0x4c_DPD_EN                            ADDRC_BEKEN_REG0x4c |= 0x10000000
#define CLRF_RC_BEKEN_REG0x4c_DPD_EN                            ADDRC_BEKEN_REG0x4c &= ~0x10000000
#define GET_RC_BEKEN_REG0x4c_DPD_EN                             ((ADDRC_BEKEN_REG0x4c & 0x10000000) >> 28)

#define POSRC_BEKEN_REG0x4c_DPD_CAL_EN                          27
#define BITRC_BEKEN_REG0x4c_DPD_CAL_EN                          0x8000000
#define SET_RC_BEKEN_REG0x4c_DPD_CAL_EN(val)                    ADDRC_BEKEN_REG0x4c = ((ADDRC_BEKEN_REG0x4c & (~0x8000000)) | (val << 27))
#define SETF_RC_BEKEN_REG0x4c_DPD_CAL_EN                        ADDRC_BEKEN_REG0x4c |= 0x8000000
#define CLRF_RC_BEKEN_REG0x4c_DPD_CAL_EN                        ADDRC_BEKEN_REG0x4c &= ~0x8000000
#define GET_RC_BEKEN_REG0x4c_DPD_CAL_EN                         ((ADDRC_BEKEN_REG0x4c & 0x8000000) >> 27)

#define POSRC_BEKEN_REG0x4c_DPD_COMP_EN                         26
#define BITRC_BEKEN_REG0x4c_DPD_COMP_EN                         0x4000000
#define SET_RC_BEKEN_REG0x4c_DPD_COMP_EN(val)                   ADDRC_BEKEN_REG0x4c = ((ADDRC_BEKEN_REG0x4c & (~0x4000000)) | (val << 26))
#define SETF_RC_BEKEN_REG0x4c_DPD_COMP_EN                       ADDRC_BEKEN_REG0x4c |= 0x4000000
#define CLRF_RC_BEKEN_REG0x4c_DPD_COMP_EN                       ADDRC_BEKEN_REG0x4c &= ~0x4000000
#define GET_RC_BEKEN_REG0x4c_DPD_COMP_EN                        ((ADDRC_BEKEN_REG0x4c & 0x4000000) >> 26)

#define POSRC_BEKEN_REG0x4c_I_CONSTANT                          16
#define BITRC_BEKEN_REG0x4c_I_CONSTANT                          0x3FF0000
#define SET_RC_BEKEN_REG0x4c_I_CONSTANT(val)                    ADDRC_BEKEN_REG0x4c = ((ADDRC_BEKEN_REG0x4c & (~0x3FF0000)) | (val << 16))
#define GET_RC_BEKEN_REG0x4c_I_CONSTANT                         ((ADDRC_BEKEN_REG0x4c & 0x3FF0000) >> 16)


#define POSRC_BEKEN_REG0x4c_Q_CONSTANT                          0
#define BITRC_BEKEN_REG0x4c_Q_CONSTANT                          0x3FF
#define SET_RC_BEKEN_REG0x4c_Q_CONSTANT(val)                    ADDRC_BEKEN_REG0x4c = ((ADDRC_BEKEN_REG0x4c & (~0x3FF)) | (val << 0))
#define GET_RC_BEKEN_REG0x4c_Q_CONSTANT                         (ADDRC_BEKEN_REG0x4c & 0x3FF)

//ADDRC_BEKEN_REG0x4d
#define ADDRC_BEKEN_REG0x4d                                     *((volatile unsigned long *) (0x01050000+0x4d*4))
#define POSRC_BEKEN_REG0x4d_TX_SIN_F                            22
#define BITRC_BEKEN_REG0x4d_TX_SIN_F                            0xFFC00000
#define SET_RC_BEKEN_REG0x4d_TX_SIN_F(val)                      ADDRC_BEKEN_REG0x4d = ((ADDRC_BEKEN_REG0x4d & (~0xFFC00000)) | (val << 22))
#define GET_RC_BEKEN_REG0x4d_TX_SIN_F                           ((ADDRC_BEKEN_REG0x4d & 0xFFC00000) >> 22)


#define POSRC_BEKEN_REG0x4d_TX_SIN_MODE                         16
#define BITRC_BEKEN_REG0x4d_TX_SIN_MODE                         0x30000
#define SET_RC_BEKEN_REG0x4d_TX_SIN_MODE(val)                   ADDRC_BEKEN_REG0x4d = ((ADDRC_BEKEN_REG0x4d & (~0x30000)) | (val << 16))
#define GET_RC_BEKEN_REG0x4d_TX_SIN_MODE                        ((ADDRC_BEKEN_REG0x4d & 0x30000) >> 16)

#define POSRC_BEKEN_REG0x4d_TX_SIN_AMP                          12
#define BITRC_BEKEN_REG0x4d_TX_SIN_AMP                          0xF000
#define SET_RC_BEKEN_REG0x4d_TX_SIN_AMP(val)                    ADDRC_BEKEN_REG0x4d = ((ADDRC_BEKEN_REG0x4d & (~0xF000)) | (val << 12))
#define GET_RC_BEKEN_REG0x4d_TX_SIN_AMP                         ((ADDRC_BEKEN_REG0x4d & 0xF000) >> 12)


//ADDRC_BEKEN_REG0x4f
#define ADDRC_BEKEN_REG0x4f                                     *((volatile unsigned long *) (0x01050000+0x4f*4))

#define POSRC_BEKEN_REG0x4f_TX_I_DC_COMP                        16
#define BITRC_BEKEN_REG0x4f_TX_I_DC_COMP                        0x3FF0000
#define SET_RC_BEKEN_REG0x4f_TX_I_DC_COMP(val)                  ADDRC_BEKEN_REG0x4f = ((ADDRC_BEKEN_REG0x4f & (~0x3FF0000)) | (val << 16))
#define GET_RC_BEKEN_REG0x4f_TX_I_DC_COMP                       ((ADDRC_BEKEN_REG0x4f & 0x3FF0000) >> 16)


#define POSRC_BEKEN_REG0x4f_TX_Q_DC_COMP                        0
#define BITRC_BEKEN_REG0x4f_TX_Q_DC_COMP                        0x3FF
#define SET_RC_BEKEN_REG0x4f_TX_Q_DC_COMP(val)                  ADDRC_BEKEN_REG0x4f = ((ADDRC_BEKEN_REG0x4f & (~0x3FF)) | (val << 0))
#define GET_RC_BEKEN_REG0x4f_TX_Q_DC_COMP                       (ADDRC_BEKEN_REG0x4f & 0x3FF)

//ADDRC_BEKEN_REG0x50
#define ADDRC_BEKEN_REG0x50                                     *((volatile unsigned long *) (0x01050000+0x50*4))

#define POSRC_BEKEN_REG0x50_TX_I_GAIN_COMP                      16
#define BITRC_BEKEN_REG0x50_TX_I_GAIN_COMP                      0x3FF0000
#define SET_RC_BEKEN_REG0x50_TX_I_GAIN_COMP(val)                ADDRC_BEKEN_REG0x50 = ((ADDRC_BEKEN_REG0x50 & (~0x3FF0000)) | (val << 16))
#define GET_RC_BEKEN_REG0x50_TX_I_GAIN_COMP                     ((ADDRC_BEKEN_REG0x50 & 0x3FF0000) >> 16)


#define POSRC_BEKEN_REG0x50_TX_Q_GAIN_COMP                      0
#define BITRC_BEKEN_REG0x50_TX_Q_GAIN_COMP                      0x3FF
#define SET_RC_BEKEN_REG0x50_TX_Q_GAIN_COMP(val)                ADDRC_BEKEN_REG0x50 = ((ADDRC_BEKEN_REG0x50 & (~0x3FF)) | (val << 0))
#define GET_RC_BEKEN_REG0x50_TX_Q_GAIN_COMP                     (ADDRC_BEKEN_REG0x50 & 0x3FF)

//ADDRC_BEKEN_REG0x51
#define ADDRC_BEKEN_REG0x51                                     *((volatile unsigned long *) (0x01050000+0x51*4))

#define POSRC_BEKEN_REG0x51_TX_PHASE_COMP                       16
#define BITRC_BEKEN_REG0x51_TX_PHASE_COMP                       0x3FF0000
#define SET_RC_BEKEN_REG0x51_TX_PHASE_COMP(val)                 ADDRC_BEKEN_REG0x51 = ((ADDRC_BEKEN_REG0x51 & (~0x3FF0000)) | (val << 16))
#define GET_RC_BEKEN_REG0x51_TX_PHASE_COMP                      ((ADDRC_BEKEN_REG0x51 & 0x3FF0000) >> 16)


#define POSRC_BEKEN_REG0x51_TX_TY2                              0
#define BITRC_BEKEN_REG0x51_TX_TY2                              0x3FF
#define SET_RC_BEKEN_REG0x51_TX_TY2(val)                        ADDRC_BEKEN_REG0x51 = ((ADDRC_BEKEN_REG0x51 & (~0x3FF)) | (val << 0))
#define GET_RC_BEKEN_REG0x51_TX_TY2                             (ADDRC_BEKEN_REG0x51 & 0x3FF)

//ADDRC_BEKEN_REG0x52
#define ADDRC_BEKEN_REG0x52                                     *((volatile unsigned long *) (0x01050000+0x52*4))
#define POSRC_BEKEN_REG0x52_TX_IQ_SWAP                          31
#define BITRC_BEKEN_REG0x52_TX_IQ_SWAP                          0x80000000
#define SET_RC_BEKEN_REG0x52_TX_IQ_SWAP(val)                    ADDRC_BEKEN_REG0x52 = ((ADDRC_BEKEN_REG0x52 & (~0x80000000)) | (val << 31))
#define SETF_RC_BEKEN_REG0x52_TX_IQ_SWAP                        ADDRC_BEKEN_REG0x52 |= 0x80000000
#define CLRF_RC_BEKEN_REG0x52_TX_IQ_SWAP                        ADDRC_BEKEN_REG0x52 &= ~0x80000000
#define GET_RC_BEKEN_REG0x52_TX_IQ_SWAP                         ((ADDRC_BEKEN_REG0x52 & 0x80000000) >> 31)

#define POSRC_BEKEN_REG0x52_IQ_CONSTANT_POUT                    21
#define BITRC_BEKEN_REG0x52_IQ_CONSTANT_POUT                    0x7FE00000
#define SET_RC_BEKEN_REG0x52_IQ_CONSTANT_POUT(val)              ADDRC_BEKEN_REG0x52 = ((ADDRC_BEKEN_REG0x52 & (~0x7FE00000)) | (val << 21))
#define GET_RC_BEKEN_REG0x52_IQ_CONSTANT_POUT                   ((ADDRC_BEKEN_REG0x52 & 0x7FE00000) >> 21)

#define POSRC_BEKEN_REG0x52_TX_PRE_GAIN                         16
#define BITRC_BEKEN_REG0x52_TX_PRE_GAIN                         0x1F0000
#define SET_RC_BEKEN_REG0x52_TX_PRE_GAIN(val)                   ADDRC_BEKEN_REG0x52 = ((ADDRC_BEKEN_REG0x52 & (~0x1F0000)) | (val << 16))
#define GET_RC_BEKEN_REG0x52_TX_PRE_GAIN                        ((ADDRC_BEKEN_REG0x52 & 0x1F0000) >> 16)

#define POSRC_BEKEN_REG0x52_IQ_CONSTANT_IQCAL_P                 6
#define BITRC_BEKEN_REG0x52_IQ_CONSTANT_IQCAL_P                 0xFFC0
#define SET_RC_BEKEN_REG0x52_IQ_CONSTANT_IQCAL_P(val)           ADDRC_BEKEN_REG0x52 = ((ADDRC_BEKEN_REG0x52 & (~0xFFC0)) | (val << 6))
#define GET_RC_BEKEN_REG0x52_IQ_CONSTANT_IQCAL_P                ((ADDRC_BEKEN_REG0x52 & 0xFFC0) >> 6)


//ADDRC_BEKEN_REG0x54
#define ADDRC_BEKEN_REG0x54                                     *((volatile unsigned long *) (0x01050000+0x54*4))
#define POSRC_BEKEN_REG0x54_AGC_MAP_MODE                        31
#define BITRC_BEKEN_REG0x54_AGC_MAP_MODE                        0x80000000
#define SET_RC_BEKEN_REG0x54_AGC_MAP_MODE(val)                  ADDRC_BEKEN_REG0x54 = ((ADDRC_BEKEN_REG0x54 & (~0x80000000)) | (val << 31))
#define SETF_RC_BEKEN_REG0x54_AGC_MAP_MODE                      ADDRC_BEKEN_REG0x54 |= 0x80000000
#define CLRF_RC_BEKEN_REG0x54_AGC_MAP_MODE                      ADDRC_BEKEN_REG0x54 &= ~0x80000000
#define GET_RC_BEKEN_REG0x54_AGC_MAP_MODE                       ((ADDRC_BEKEN_REG0x54 & 0x80000000) >> 31)

#define POSRC_BEKEN_REG0x54_AGC_LNA_RD                          29
#define BITRC_BEKEN_REG0x54_AGC_LNA_RD                          0x60000000
#define SET_RC_BEKEN_REG0x54_AGC_LNA_RD(val)                    ADDRC_BEKEN_REG0x54 = ((ADDRC_BEKEN_REG0x54 & (~0x60000000)) | (val << 29))
#define GET_RC_BEKEN_REG0x54_AGC_LNA_RD                         ((ADDRC_BEKEN_REG0x54 & 0x60000000) >> 29)

#define POSRC_BEKEN_REG0x54_AGC_BUF_RD                          28
#define BITRC_BEKEN_REG0x54_AGC_BUF_RD                          0x10000000
#define SET_RC_BEKEN_REG0x54_AGC_BUF_RD(val)                    ADDRC_BEKEN_REG0x54 = ((ADDRC_BEKEN_REG0x54 & (~0x10000000)) | (val << 28))
#define SETF_RC_BEKEN_REG0x54_AGC_BUF_RD                        ADDRC_BEKEN_REG0x54 |= 0x10000000
#define CLRF_RC_BEKEN_REG0x54_AGC_BUF_RD                        ADDRC_BEKEN_REG0x54 &= ~0x10000000
#define GET_RC_BEKEN_REG0x54_AGC_BUF_RD                         ((ADDRC_BEKEN_REG0x54 & 0x10000000) >> 28)

#define POSRC_BEKEN_REG0x54_AGC_PGA_RD                          24
#define BITRC_BEKEN_REG0x54_AGC_PGA_RD                          0xF000000
#define SET_RC_BEKEN_REG0x54_AGC_PGA_RD(val)                    ADDRC_BEKEN_REG0x54 = ((ADDRC_BEKEN_REG0x54 & (~0xF000000)) | (val << 24))
#define GET_RC_BEKEN_REG0x54_AGC_PGA_RD                         ((ADDRC_BEKEN_REG0x54 & 0xF000000) >> 24)

#define POSRC_BEKEN_REG0x54_TSSI_RD                             16
#define BITRC_BEKEN_REG0x54_TSSI_RD                             0xFF0000
#define SET_RC_BEKEN_REG0x54_TSSI_RD(val)                       ADDRC_BEKEN_REG0x54 = ((ADDRC_BEKEN_REG0x54 & (~0xFF0000)) | (val << 16))
#define GET_RC_BEKEN_REG0x54_TSSI_RD                            ((ADDRC_BEKEN_REG0x54 & 0xFF0000) >> 16)

#define POSRC_BEKEN_REG0x54_ST_SAR_ADC                          14
#define BITRC_BEKEN_REG0x54_ST_SAR_ADC                          0xC000
#define SET_RC_BEKEN_REG0x54_ST_SAR_ADC(val)                    ADDRC_BEKEN_REG0x54 = ((ADDRC_BEKEN_REG0x54 & (~0xC000)) | (val << 14))
#define GET_RC_BEKEN_REG0x54_ST_SAR_ADC                         ((ADDRC_BEKEN_REG0x54 & 0xC000) >> 14)

#define POSRC_BEKEN_REG0x54_ST_RX_ADC                           12
#define BITRC_BEKEN_REG0x54_ST_RX_ADC                           0x3000
#define SET_RC_BEKEN_REG0x54_ST_RX_ADC(val)                     ADDRC_BEKEN_REG0x54 = ((ADDRC_BEKEN_REG0x54 & (~0x3000)) | (val << 12))
#define GET_RC_BEKEN_REG0x54_ST_RX_ADC                          ((ADDRC_BEKEN_REG0x54 & 0x3000) >> 12)

#define POSRC_BEKEN_REG0x54_TX_DC_N                             10
#define BITRC_BEKEN_REG0x54_TX_DC_N                             0xC00
#define SET_RC_BEKEN_REG0x54_TX_DC_N(val)                       ADDRC_BEKEN_REG0x54 = ((ADDRC_BEKEN_REG0x54 & (~0xC00)) | (val << 10))
#define GET_RC_BEKEN_REG0x54_TX_DC_N                            ((ADDRC_BEKEN_REG0x54 & 0xC00) >> 10)

#define POSRC_BEKEN_REG0x54_DSEL_VA                             9
#define BITRC_BEKEN_REG0x54_DSEL_VA                             0x200
#define SET_RC_BEKEN_REG0x54_DSEL_VA(val)                       ADDRC_BEKEN_REG0x54 = ((ADDRC_BEKEN_REG0x54 & (~0x200)) | (val << 9))
#define SETF_RC_BEKEN_REG0x54_DSEL_VA                           ADDRC_BEKEN_REG0x54 |= 0x200
#define CLRF_RC_BEKEN_REG0x54_DSEL_VA                           ADDRC_BEKEN_REG0x54 &= ~0x200
#define GET_RC_BEKEN_REG0x54_DSEL_VA                            ((ADDRC_BEKEN_REG0x54 & 0x200) >> 9)

#define POSRC_BEKEN_REG0x54_TSSI_POUT_TH                        1
#define BITRC_BEKEN_REG0x54_TSSI_POUT_TH                        0x1FE
#define SET_RC_BEKEN_REG0x54_TSSI_POUT_TH(val)                  ADDRC_BEKEN_REG0x54 = ((ADDRC_BEKEN_REG0x54 & (~0x1FE)) | (val << 1))
#define GET_RC_BEKEN_REG0x54_TSSI_POUT_TH                       ((ADDRC_BEKEN_REG0x54 & 0x1FE) >> 1)

#define POSRC_BEKEN_REG0x54_ST_RX_ADC_IQ                        0
#define BITRC_BEKEN_REG0x54_ST_RX_ADC_IQ                        0x1
#define SET_RC_BEKEN_REG0x54_ST_RX_ADC_IQ(val)                  ADDRC_BEKEN_REG0x54 = ((ADDRC_BEKEN_REG0x54 & (~0x1)) | (val << 0))
#define SETF_RC_BEKEN_REG0x54_ST_RX_ADC_IQ                      ADDRC_BEKEN_REG0x54 |= 0x1
#define CLRF_RC_BEKEN_REG0x54_ST_RX_ADC_IQ                      ADDRC_BEKEN_REG0x54 &= ~0x1
#define GET_RC_BEKEN_REG0x54_ST_RX_ADC_IQ                       (ADDRC_BEKEN_REG0x54 & 0x1)

//ADDRC_BEKEN_REG0x55
#define ADDRC_BEKEN_REG0x55                                     *((volatile unsigned long *) (0x01050000+0x55*4))

#define POSRC_BEKEN_REG0x55_RX_SNR_SIG                          16
#define BITRC_BEKEN_REG0x55_RX_SNR_SIG                          0x1FF0000
#define SET_RC_BEKEN_REG0x55_RX_SNR_SIG(val)                    ADDRC_BEKEN_REG0x55 = ((ADDRC_BEKEN_REG0x55 & (~0x1FF0000)) | (val << 16))
#define GET_RC_BEKEN_REG0x55_RX_SNR_SIG                         ((ADDRC_BEKEN_REG0x55 & 0x1FF0000) >> 16)


#define POSRC_BEKEN_REG0x55_RX_SNR_NOISE                        0
#define BITRC_BEKEN_REG0x55_RX_SNR_NOISE                        0x1FF
#define SET_RC_BEKEN_REG0x55_RX_SNR_NOISE(val)                  ADDRC_BEKEN_REG0x55 = ((ADDRC_BEKEN_REG0x55 & (~0x1FF)) | (val << 0))
#define GET_RC_BEKEN_REG0x55_RX_SNR_NOISE                       (ADDRC_BEKEN_REG0x55 & 0x1FF)

//ADDRC_BEKEN_REG0x5c
#define ADDRC_BEKEN_REG0x5c                                     *((volatile unsigned long *) (0x01050000+0x5c*4))
#define POSRC_BEKEN_REG0x5c_trx_bank_rpt                        31
#define BITRC_BEKEN_REG0x5c_trx_bank_rpt                        0x80000000
#define SET_RC_BEKEN_REG0x5c_trx_bank_rpt(val)                  ADDRC_BEKEN_REG0x5c = ((ADDRC_BEKEN_REG0x5c & (~0x80000000)) | (val << 31))
#define SETF_RC_BEKEN_REG0x5c_trx_bank_rpt                      ADDRC_BEKEN_REG0x5c |= 0x80000000
#define CLRF_RC_BEKEN_REG0x5c_trx_bank_rpt                      ADDRC_BEKEN_REG0x5c &= ~0x80000000
#define GET_RC_BEKEN_REG0x5c_trx_bank_rpt                       ((ADDRC_BEKEN_REG0x5c & 0x80000000) >> 31)


#define POSRC_BEKEN_REG0x5c_trx_spi_intval                      0
#define BITRC_BEKEN_REG0x5c_trx_spi_intval                      0x3FF
#define SET_RC_BEKEN_REG0x5c_trx_spi_intval(val)                ADDRC_BEKEN_REG0x5c = ((ADDRC_BEKEN_REG0x5c & (~0x3FF)) | (val << 0))
#define GET_RC_BEKEN_REG0x5c_trx_spi_intval                     (ADDRC_BEKEN_REG0x5c & 0x3FF)

//ADDRC_BEKEN_REG0x6a
#define ADDRC_BEKEN_REG0x6a                                     *((volatile unsigned long *) (0x01050000+0x6a*4))

#define POSRC_BEKEN_REG0x6a_trx_ADDa_reg_stat                   0
#define BITRC_BEKEN_REG0x6a_trx_ADDa_reg_stat                   0x3F
#define SET_RC_BEKEN_REG0x6a_trx_ADDa_reg_stat(val)             ADDRC_BEKEN_REG0x6a = ((ADDRC_BEKEN_REG0x6a & (~0x3F)) | (val << 0))
#define GET_RC_BEKEN_REG0x6a_trx_ADDa_reg_stat                  (ADDRC_BEKEN_REG0x6a & 0x3F)

#endif // _RC_BEKEN_H_
