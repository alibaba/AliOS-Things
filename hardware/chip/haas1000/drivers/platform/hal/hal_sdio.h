#ifndef __HAL_SDIO_H__
#define __HAL_SDIO_H__

#define SDIO_MAX_FUNCTIONS		4

/* SDIO commands                                          type  argument     response */
#define SD_IO_SEND_OP_COND          5 /* bcr  [23:0] OCR         R4  */
#define SD_IO_RW_DIRECT            52 /* ac   [31:0] See below   R5  */
#define SD_IO_RW_EXTENDED          53 /* adtc [31:0] See below   R5  */

/* CMD52 arguments */
#define SDIO_ARG_CMD52_READ             (0<<31)
#define SDIO_ARG_CMD52_WRITE            (1u<<31)
#define SDIO_ARG_CMD52_FUNC_SHIFT       28
#define SDIO_ARG_CMD52_FUNC_MASK        0x7
#define SDIO_ARG_CMD52_RAW_FLAG         (1u<<27)
#define SDIO_ARG_CMD52_REG_SHIFT        9
#define SDIO_ARG_CMD52_REG_MASK         0x1ffff
#define SDIO_ARG_CMD52_DATA_SHIFT       0
#define SDIO_ARG_CMD52_DATA_MASK        0xff
#define SDIO_R5_DATA(resp)            ((resp)[0] & 0xff)

/* CMD53 arguments */
#define SDIO_ARG_CMD53_READ             (0<<31)
#define SDIO_ARG_CMD53_WRITE            (1u<<31)
#define SDIO_ARG_CMD53_FUNC_SHIFT       28
#define SDIO_ARG_CMD53_FUNC_MASK        0x7
#define SDIO_ARG_CMD53_BLOCK_MODE       (1u<<27)
#define SDIO_ARG_CMD53_INCREMENT        (1u<<26)
#define SDIO_ARG_CMD53_REG_SHIFT        9
#define SDIO_ARG_CMD53_REG_MASK         0x1ffff
#define SDIO_ARG_CMD53_LENGTH_SHIFT     0
#define SDIO_ARG_CMD53_LENGTH_MASK      0x1ff
#define SDIO_ARG_CMD53_LENGTH_MAX       511

/* R5 response bits */
#define R5_COM_CRC_ERROR	(1 << 15)
#define R5_ILLEGAL_COMMAND	(1 << 14)
#define R5_ERROR			(1 << 11)
#define R5_FUNCTION_NUMBER	(1 << 9)
#define R5_OUT_OF_RANGE		(1 << 8)
#define R5_STATUS(x)		(x & 0xCB00)
#define R5_IO_CURRENT_STATE(x)	((x & 0x3000) >> 12)

/*
 * Card Common Control Registers (CCCR)
 */

#define SDIO_REG_CCCR_CCCR_REV      0x00

#define  SDIO_CCCR_REV_1_00 0   /* CCCR/FBR Version 1.00 */
#define  SDIO_CCCR_REV_1_10 1   /* CCCR/FBR Version 1.10 */
#define  SDIO_CCCR_REV_1_20 2   /* CCCR/FBR Version 1.20 */

#define  SDIO_SDIO_REV_1_00 0   /* SDIO Spec Version 1.00 */
#define  SDIO_SDIO_REV_1_10 1   /* SDIO Spec Version 1.10 */
#define  SDIO_SDIO_REV_1_20 2   /* SDIO Spec Version 1.20 */
#define  SDIO_SDIO_REV_2_00 3   /* SDIO Spec Version 2.00 */

#define SDIO_REG_CCCR_SD_REV        0x01

#define  SDIO_SD_REV_1_01   0   /* SD Physical Spec Version 1.01 */
#define  SDIO_SD_REV_1_10   1   /* SD Physical Spec Version 1.10 */
#define  SDIO_SD_REV_2_00   2   /* SD Physical Spec Version 2.00 */

#define SDIO_REG_CCCR_IO_EN         0x02
#define SDIO_REG_CCCR_IO_RDY        0x03

#define SDIO_REG_CCCR_INT_EN        0x04    /* Function/Master Interrupt Enable */
#define SDIO_REG_CCCR_INT_PEND      0x05    /* Function Interrupt Pending */

#define SDIO_REG_CCCR_IO_ABORT      0x06    /* function abort/card reset */

#define SDIO_REG_CCCR_BUS_IF        0x07    /* bus interface controls */

#define  SDIO_BUS_WIDTH_1BIT        0x00
#define  SDIO_BUS_WIDTH_4BIT        0x02
#define  SDIO_BUS_ECSI              0x20    /* Enable continuous SPI interrupt */
#define  SDIO_BUS_SCSI              0x40    /* Support continuous SPI interrupt */

#define  SDIO_BUS_ASYNC_INT         0x20

#define  SDIO_BUS_CD_DISABLE        0x80   /* disable pull-up on DAT3 (pin 1) */

#define SDIO_REG_CCCR_CARD_CAPS     0x08

#define  SDIO_CCCR_CAP_SDC  0x01    /* can do CMD52 while data transfer */
#define  SDIO_CCCR_CAP_SMB  0x02    /* can do multi-block xfers (CMD53) */
#define  SDIO_CCCR_CAP_SRW  0x04    /* supports read-wait protocol */
#define  SDIO_CCCR_CAP_SBS  0x08    /* supports suspend/resume */
#define  SDIO_CCCR_CAP_S4MI 0x10    /* interrupt during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_E4MI 0x20    /* enable ints during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_LSC  0x40    /* low speed card */
#define  SDIO_CCCR_CAP_4BLS 0x80    /* 4 bit low speed card */

#define SDIO_REG_CCCR_CIS_PTR       0x09    /* common CIS pointer (3 bytes) */

/* Following 4 regs are valid only if SBS is set */
#define SDIO_REG_CCCR_BUS_SUSPEND   0x0c
#define SDIO_REG_CCCR_FUNC_SEL      0x0d
#define SDIO_REG_CCCR_EXEC_FLAG     0x0e
#define SDIO_REG_CCCR_READY_FLAG    0x0f

#define SDIO_REG_CCCR_FN0_BLKSIZE   0x10 /* 2bytes, 0x10~0x11 */

#define SDIO_REG_CCCR_POWER_CTRL    0x12

#define  SDIO_POWER_SMPC            0x01    /* Supports Master Power Control */
#define  SDIO_POWER_EMPC            0x02    /* Enable Master Power Control */

#define SDIO_REG_CCCR_SPEED         0x13

#define  SDIO_SPEED_SHS             0x01    /* Supports High-Speed mode */
#define  SDIO_SPEED_EHS             0x02    /* Enable High-Speed mode */

/*
 * Function Basic Registers (FBR)
 */

#define SDIO_REG_FBR_BASE(f)    ((f) * 0x100) /* base of function f's FBRs */

#define SDIO_REG_FBR_STD_FUNC_IF    0x00

#define  SDIO_FBR_SUPPORTS_CSA      0x40    /* supports Code Storage Area */
#define  SDIO_FBR_ENABLE_CSA        0x80    /* enable Code Storage Area */

#define SDIO_REG_FBR_STD_IF_EXT     0x01

#define SDIO_REG_FBR_POWER          0x02

#define  SDIO_FBR_POWER_SPS         0x01    /* Supports Power Selection */
#define  SDIO_FBR_POWER_EPS         0x02    /* Enable (low) Power Selection */

#define SDIO_REG_FBR_CIS            0x09    /* CIS pointer (3 bytes) */


#define SDIO_REG_FBR_CSA            0x0C    /* CSA pointer (3 bytes) */

#define SDIO_REG_FBR_CSA_DATA       0x0F

#define SDIO_REG_FBR_BLKSIZE        0x10    /* block size (2 bytes) */

/* SDIO CIS Tuple code */
#define CISTPL_NULL      0x00
#define CISTPL_CHECKSUM  0x10
#define CISTPL_VERS_1    0x15
#define CISTPL_ALTSTR    0x16
#define CISTPL_MANFID    0x20
#define CISTPL_FUNCID    0x21
#define CISTPL_FUNCE     0x22
#define CISTPL_SDIO_STD  0x91
#define CISTPL_SDIO_EXT  0x92
#define CISTPL_END       0xff

/* SDIO device id */
#define SDIO_ANY_FUNC_ID    0xff
#define SDIO_ANY_MAN_ID     0xffff
#define SDIO_ANY_PROD_ID    0xffff

struct sdio_device_id
{
    uint8_t   func_code;
    uint16_t  manufacturer;
    uint16_t  product;
};

struct sdio_cccr {
	uint8_t		sdio_version;
	uint8_t		sd_version;
	uint8_t		direct_cmd:1,     /*  Card Supports Direct Commands during data transfer
	                                               only SD mode, not used for SPI mode */
				multi_block:1,    /*  Card Supports Multi-Block */
				read_wait:1,      /*  Card Supports Read Wait
				                       only SD mode, not used for SPI mode */
				suspend_resume:1, /*  Card supports Suspend/Resume
				                       only SD mode, not used for SPI mode */
				s4mi:1,            /* generate interrupts during a 4-bit
				                      multi-block data transfer */
				e4mi:1,            /*  Enable the multi-block IRQ during
				                       4-bit transfer for the SDIO card */
				low_speed:1,      /*  Card  is  a  Low-Speed  card */
				low_speed_4:1;    /*  4-bit support for Low-Speed cards */

	uint8_t		bus_width:1,     /* Support SDIO bus width, 1:4bit, 0:1bit */
				cd_disable:1,    /*  Connect[0]/Disconnect[1] the 10K-90K ohm pull-up
				                     resistor on CD/DAT[3] (pin 1) of the card */
				power_ctrl:1,    /* Support Master Power Control */
				high_speed:1;    /* Support High-Speed  */
};

struct sdio_cis {
	uint16_t		manufacturer;
	uint16_t		product;
	uint16_t		func0_blk_size;
	uint32_t		max_tran_speed;
};

/*
 * SDIO function CIS tuple (unknown to the core)
 */
struct sdio_function_tuple {
	struct sdio_function_tuple *next;
	uint8_t code;
	uint8_t size;
	uint8_t *data;
};
struct sdio_function;
typedef void (*sdio_irq_handler_t)(struct sdio_function *);

struct mmc;
/*
 * SDIO function devices
 */
struct sdio_function {
	struct mmc *mmc;		/* the card this device belongs to */
    void*		dev;		/* the device */
	sdio_irq_handler_t	irq_handler;	/* IRQ callback */
	uint8_t		num;		/* function number */

	uint8_t		    func_code;   /*  Standard SDIO Function interface code  */
	uint16_t		manufacturer;		/* manufacturer id */
	uint16_t		product;		/* product id */

	uint32_t		max_blk_size;	/* maximum block size */
	uint32_t		cur_blk_size;	/* current block size */

	uint32_t		enable_timeout_val; /* max enable timeout in msec */

    uint32_t        buf_id_tx;
    int				buf_id_rx;	/* byte */
    uint32_t        hw_type;
    uint32_t        transq_pri;
    uint8_t         transq_irq_count;
    uint8_t         wsm_startup_done;
    void*           priv;
};
#define sdio_func sdio_function
int32_t sdio_io_send_op_cond(struct mmc *mmc, uint32_t ocr,
                                            uint32_t    *cmd5_resp);
int32_t sdio_io_rw_direct(struct mmc *mmc,
                             int32_t            rw,
                             uint32_t           fn,
                             uint32_t           reg_addr,
                             uint8_t           *pdata,
                             uint8_t            raw);
int32_t sdio_io_rw_extended(struct mmc *mmc,
                               int32_t            rw,
                               uint32_t           fn,
                               uint32_t           addr,
                               int32_t            op_code,
                               uint8_t           *buf,
                               uint32_t           blocks,
                               uint32_t           blksize);
uint8_t sdio_io_readb(struct sdio_function *func,
                         uint32_t              reg,
                         int32_t              *err);
int32_t sdio_io_writeb(struct sdio_function *func,
                          uint32_t              reg,
                          uint8_t               data);
uint16_t sdio_io_readw(struct sdio_function *func,
                          uint32_t              addr,
                          int32_t              *err);
int32_t sdio_io_writew(struct sdio_function *func,
                          uint16_t              data,
                          uint32_t              addr);
uint32_t sdio_io_readl(struct sdio_function *func,
                          uint32_t              addr,
                          int32_t              *err);
int32_t sdio_io_writel(struct sdio_function *func,
                          uint32_t              data,
                          uint32_t              addr);
int32_t sdio_io_read_block(struct sdio_function *func,
                                     uint32_t              addr,
                                     uint8_t              *buf,
                                     uint32_t              len);
int32_t sdio_io_write_block(struct  sdio_function *func,
                                      uint32_t              addr,
                                      uint8_t              *buf,
                                      uint32_t              len);

unsigned int sdio_align_size(struct sdio_function *func, unsigned int sz);
int32_t sdio_attach_irq(struct sdio_function *func,
                           sdio_irq_handler_t   handler);
int32_t sdio_detach_irq(struct sdio_function *func);
int32_t sdio_enable_func(struct sdio_function *func);
int32_t sdio_disable_func(struct sdio_function *func);
int32_t sdio_set_block_size(struct sdio_function *func,
                               uint32_t              blksize);
uint32_t mmc_detect_sdio(struct mmc *mmc);
void hal_sdio_enable_irq(struct sdio_function *func);
void hal_sdio_disable_irq(struct sdio_function *func);
void hal_sdio_check_irq(struct sdio_function *func);
#endif

