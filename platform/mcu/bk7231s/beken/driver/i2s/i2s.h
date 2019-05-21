#ifndef _I2S_H_
#define _I2S_H_

#define I2S_BASE                     (0x00802500)

#define PCM_CTRL                     (I2S_BASE + 0 * 4)
#define BITRATIO_POSI                (0)
#define BITRATIO_MASK                (0xFF)
#define SMPRATIO_POSI                (8)
#define SMPRATIO_MASK                (0x1F)
#define PCM_DLEN_POSI                (13)
#define PCM_DLEN_MASK                (0x07)
#define DATALEN_POSI                 (16)
#define DATALEN_MASK                 (0x1F)
#define SYNCLEN_POSI                 (21)
#define SYNCLEN_MASK                 (0x07)
#define LSB_FIRST                    (0x01UL << 24)
#define SCK_INV                      (0x01UL << 25)
#define LRCK_RP                      (0x01UL << 26)
#define MODE_SEL_POSI                (27)
#define MODE_SEL_MASK                (0x07)
#define MSTEN                        (0x01UL << 30)
#define SLAVEEN						 (0 << 30)
#define I2S_PCM_EN                   (0x01UL << 31)

#define PCM_CN                       (I2S_BASE + 1 * 4)
#define RX_INT_EN                    (0x01UL << 0)
#define TX_INT0_EN                   (0x01UL << 1)
#define TX_INT1_EN                   (0x01UL << 2)
#define TX_INT2_EN                   (0x01UL << 3)
#define RX_OVF_EN                    (0x01UL << 4)
#define TX_UDF0_EN                   (0x01UL << 5)
#define TX_UDF1_EN                   (0x01UL << 6)
#define TX_UDF2_EN                   (0x01UL << 7)
#define RX_FIFO_LEVEL_POSI           (8)
#define RX_FIFO_LEVEL_MASK           (0x03)
#define TX_FIFO0_LEVEL_POSI          (10)
#define TX_FIFO0_LEVEL_MASK          (0x03)
#define TX_FIFO1_LEVEL_POSI          (12)
#define TX_FIFO1_LEVEL_MASK          (0x03)
#define TX_FIFO2_LEVEL_POSI          (14)
#define TX_FIFO2_LEVEL_MASK          (0x03)
#define RX_FIFO_CLR                  (0x01UL << 16)
#define TX_FIFO0_CLR                 (0x01UL << 17)
#define TX_FIFO1_CLR                 (0x01UL << 18)
#define TX_FIFO2_CLR                 (0x01UL << 19)

#define PCM_STAT                     (I2S_BASE + 2 * 4)
#define RX_INT                       (0x01UL << 0)
#define TX_INT0                      (0x01UL << 1)
#define TX_INT1                      (0x01UL << 2)
#define TX_INT2                      (0x01UL << 3)
#define RX_OVF                       (0x01UL << 4)
#define TX_UDF0                      (0x01UL << 5)
#define TX_UDF1                      (0x01UL << 6)
#define TX_UDF2                      (0x01UL << 7)
#define RX_FIFO0_EMPTY               (0x01UL << 8)
#define RX_FIFO0_ALMOST_EMPTY        (0x01UL << 9)
#define TX_FIFO0_FULL                (0x01UL << 10)
#define TX_FIFO0_ALMOST_FULL         (0x01UL << 11)
#define TX_FIFO1_FULL                (0x01UL << 12)
#define TX_FIFO1_ALMOST_FULL         (0x01UL << 13)
#define TX_FIFO2_FULL                (0x01UL << 14)
#define TX_FIFO2_ALMOST_FULL         (0x01UL << 15)

#define PCM_DAT0                     (I2S_BASE + 3 * 4)

#define PCM_DAT1                     (I2S_BASE + 4 * 4)

#define PCM_DAT2                     (I2S_BASE + 5 * 4)

#define	PCM_CFG2					 (I2S_BASE + 6 * 4)

#define TXFIFO_TXFIFO_MODE       	 6
#define TXFIFO_BITSEQ_MODE        	 4
#define TXFIFO_DOWN_SMPRATIO     	 0

//#define I2S_SYS_CLK     24576000
#define I2S_SYS_CLK     26000000

/*******************************************************************************
* Function Declarations
*******************************************************************************/
static UINT32 i2s_ctrl(UINT32 cmd, void *param);
#endif //_I2S_H_
