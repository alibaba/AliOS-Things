#include "include.h"

#include "sys_rtos.h"
#include "error.h"
#if CFG_USE_SDCARD_HOST
#include "sdio_driver.h"
#include "rtos_pub.h"
#include "sys_rtos.h"
#include "sdcard.h"
#include "sdcard_pub.h"

#include "drv_model_pub.h"
#include "sys_ctrl_pub.h"
#include "mem_pub.h"
#include "arch.h"
#include "arm_arch.h"
#include "gpio_pub.h"
#include "mcu_ps_pub.h"
#include "target_util_pub.h"

/* Standard sd  commands (  )           type  argument     response */
#define GO_IDLE_STATE             0   /* bc                          */
#define ALL_SEND_CID              2
#define SEND_RELATIVE_ADDR        3   /* ac   [31:16] RCA        R6  */
#define IO_SEND_OP_COND  		  5   /* ac                      R4  */
#define SWITCH_FUNC               6
#define SELECT_CARD               7   /* ac   [31:16] RCA        R7  */
#define SEND_IF_COND              8   /* adtc                    R1  */
#define SEND_CSD                  9
#define SEND_STATUS               13
#define READ_SINGLE_BLOCK         17
#define WRITE_BLOCK               24
#define SD_APP_OP_COND            41
#define IO_RW_DIRECT              52  /* ac   [31:0] See below   R5  */
#define IO_RW_EXTENDED            53  /* adtc [31:0] See below   R5  */
#define APP_CMD                   55

#define R5_COM_CRC_ERROR	      (1 << 15)	/* er, b */
#define R5_ILLEGAL_COMMAND	      (1 << 14)	/* er, b */
#define R5_ERROR		          (1 << 11)	/* erx, c */
#define R5_FUNCTION_NUMBER	      (1 << 9)	/* er, c */
#define R5_OUT_OF_RANGE		      (1 << 8)	/* er, c */
#define R5_STATUS(x)		      (x & 0xCB00)
#define R5_IO_CURRENT_STATE(x)	  ((x & 0x3000) >> 12) /* s, b */

/*STM32 register bit define*/
#define SDIO_ICR_MASK             0x5FF
#define SDIO_STATIC_FLAGS         ((UINT32)0x000005FF)
#define SDIO_FIFO_ADDRESS         ((UINT32)0x40018080)

#define OCR_MSK_BUSY             0x80000000 // Busy flag
#define OCR_MSK_HC               0x40000000 // High Capacity flag
#define OCR_MSK_VOLTAGE_ALL      0x00FF8000 // All Voltage flag

#define SD_DEFAULT_OCR           (OCR_MSK_VOLTAGE_ALL|OCR_MSK_HC)

//#define SD_CLK_PIN                              34
#define REG_A2_CONFIG                        ((0x0802800) + 50*4)

typedef enum
{
    SD_CARD_IDLE                 = ((UINT32)0),
    SD_CARD_READY                = ((UINT32)1),
    SD_CARD_IDENTIFICATION       = ((UINT32)2),
    SD_CARD_STANDBY              = ((UINT32)3),
    SD_CARD_TRANSFER             = ((UINT32)4),
    SD_CARD_SENDING              = ((UINT32)5),
    SD_CARD_RECEIVING            = ((UINT32)6),
    SD_CARD_PROGRAMMING          = ((UINT32)7),
    SD_CARD_DISCONNECTED         = ((UINT32)8),
    SD_CARD_ERROR                = ((UINT32)0xff)
} SDCardState;

typedef struct sdio_command
{
    UINT32	index;
    UINT32  arg;
    UINT32	flags;		    /* expected response type */
    UINT32  timeout;
    UINT32	resp[4];
    void    *data;		    /* data segment associated with cmd */
    SDIO_Error	err;		/* command error */
} SDIO_CMD_S, *SDIO_CMD_PTR;

static SDCARD_S sdcard;
static DD_OPERATIONS sdcard_op =
{
    sdcard_open,
    sdcard_close,
    sdcard_read,
    sdcard_write,
    sdcard_ctrl
};


//#define SD_DEBOUNCE_COUNT 			    10

//static uint8 sd_online = SD_CARD_OFFLINE;
//static uint32 sd_clk_pin = SD_CLK_PIN;
//static uint32 sd_cd_pin = SD_DETECT_DEFAULT_GPIO;

static uint16 NoneedInitflag = 0;
//uint8 SD_det_gpio_flag = 1;
//static uint16 Sd_MMC_flag = 0;
//static uint16 cnt_online = 0;
//static beken_timer_t sd_cd_timer = {0};

/******************************************************************************/
/***************************** public function ********************************/
/******************************************************************************/
static void sdio_hw_init(void)
{
    /* config sdcard gpio */
    sdio_gpio_config();

    /* reset sdcard moudle register */
    sdio_register_reset();

    /* set sdcard low clk */
    sdio_set_low_clk();

    /* set sdcard  clk enable*/
    sdio_clk_config(1);

}

static void sdio_send_cmd(SDIO_CMD_PTR sdio_cmd_ptr)
{
    sdio_sendcmd_function(sdio_cmd_ptr->index,
                          sdio_cmd_ptr->flags, 
                          sdio_cmd_ptr->timeout, 
                          (void *)sdio_cmd_ptr->arg);
}

static void sdio_hw_uninit(void)
{
    UINT32 param;
    sdio_clk_config(0);

#if 0
    if(!SD_det_gpio_flag)
    {
        param = GPIO_CFG_PARAM(sd_clk_pin, GMODE_INPUT_PULLUP);
        sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
    }
#endif
}

static void sdio_sw_init(void)
{
    os_memset((void *)&sdcard, 0, sizeof(SDCARD_S));
}

/******************************************************************************/
/***************************** sdcard function ********************************/
/******************************************************************************/
/* GO_IDLE_STATE */
static SDIO_Error sdcard_cmd0_process(void)
{
    SDIO_CMD_S cmd;

    cmd.index = GO_IDLE_STATE;
    cmd.arg = 0;
    cmd.flags = SD_CMD_NORESP;
    cmd.timeout = DEF_LOW_SPEED_CMD_TIMEOUT;//DEF_CMD_TIME_OUT;
    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);
    return cmd.err;
}

static SDIO_Error sdcard_cmd1_process(void)
{
    SDIO_CMD_S cmd;
    uint32 response, reg;

    cmd.index = 1;
    cmd.arg = 0x40ff8000;
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout = 0x900;//DEF_CMD_TIME_OUT;
cmd1_loop:
    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);
    if(cmd.err == SD_OK)
    {
        sdio_get_cmdresponse_argument(0, &response);
        if(!(response & OCR_MSK_VOLTAGE_ALL))
            cmd.err = SD_ERR_CMD41_CNT;
        if(!(response & OCR_MSK_BUSY))
            goto cmd1_loop;
        if(response & OCR_MSK_HC)
            sdcard.Addr_shift_bit = 0;
        else
            sdcard.Addr_shift_bit = 9;
    }
    reg = REG_READ(REG_SDCARD_FIFO_THRESHOLD);
    reg |= 20;
    REG_WRITE(REG_SDCARD_FIFO_THRESHOLD, reg);

    return cmd.err;
}

static SDIO_Error sdcard_mmc_cmd8_process(void)
{
    int i;
    SDIO_CMD_S cmd;
    uint32 response, reg;
    uint32 tmp;
    uint8 *tmpptr = (uint8 *)os_malloc(512);
    if(tmpptr == NULL)
        return 1;
    os_memset(tmpptr, 0, 512);

    cmd.index = SEND_IF_COND;
    cmd.arg = 0;
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout = 0x90000; // DEF_CMD_TIME_OUT;
    sdio_send_cmd(&cmd);

    if (cmd.err != SD_OK)
        goto freebuf;
    REG_WRITE(REG_SDCARD_FIFO_THRESHOLD, (1 << 20));// reset first
    REG_WRITE(REG_SDCARD_FIFO_THRESHOLD, 0x3ffff);// set fifo later
    driver_sdcard_recv_data_start(DEF_CMD_TIME_OUT);

    tmp = 0;
    cmd.err = wait_Receive_Data();
    if(cmd.err == SD_OK)
    {
        for (i = 0; i < 128; i++)
        {
            while(!(REG_READ(REG_SDCARD_FIFO_THRESHOLD) & (0x1 << 18)))
            {
                tmp++;
                if(tmp > 0x20)
                    break;
            }

            *((uint32 *)tmpptr + i) = REG_READ(REG_SDCARD_RD_DATA_ADDR);
        }
        sdcard.total_block = tmpptr[212] | (tmpptr[213] << 8) | (tmpptr[214] << 16) | (tmpptr[215] << 24);
    }

freebuf:
    os_free(tmpptr);
    return cmd.err;
}


static SDIO_Error sdcard_cmd8_process(void)
{
    SDIO_CMD_S cmd;
    UINT8 voltage_accpet, check_pattern;

    cmd.index = SEND_IF_COND;
    cmd.arg = 0x1AA;
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout = DEF_LOW_SPEED_CMD_TIMEOUT; // DEF_CMD_TIME_OUT;

    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);

    if(cmd.err == SD_CMD_RSP_TIMEOUT)
    {
        SDCARD_WARN("cmd8 noresp, voltage mismatch or Ver1.X SD or not SD\r\n");
        return SD_CMD_RSP_TIMEOUT;
    }
    else if(cmd.err == SD_CMD_CRC_FAIL)
    {
        SDCARD_WARN("cmd8 cmdcrc err\r\n");
        return SD_CMD_CRC_FAIL;
    }

    SDCARD_PRT("found a Ver2.00 or later SDCard\r\n");

    // check Valid Response,
    // R7-[11:8]:voltage accepted, [7:0] echo-back of check pattern
    sdio_get_cmdresponse_argument(0, &cmd.resp[0]);

    check_pattern = cmd.resp[0] & 0xff;
    voltage_accpet = cmd.resp[0] >> 8 & 0xf;

    if(voltage_accpet == 0x1 && check_pattern == 0xaa)
    {
        SDCARD_PRT("support 2.7~3.6V\r\n");
        return SD_OK;
    }
    else
    {
        SDCARD_WARN("unsupport voltage\r\n");
        return SD_INVALID_VOLTRANGE;
    }
    return SD_OK;
}

/*Send host capacity support information(HCS) and  asks
  the card to send its OCR in the response on CMD line*/
static SDIO_Error sdcard_acmd41_process(UINT32 ocr)
{
    SDIO_CMD_S cmd;


    cmd.index = APP_CMD;
    cmd.arg = 0;
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout = DEF_LOW_SPEED_CMD_TIMEOUT; // DEF_CMD_TIME_OUT;
    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);
    if(cmd.err != SD_OK)
    {
        SDCARD_WARN("send cmd55 err:%d\r\n", cmd.err);
        return cmd.err;
    }


    cmd.index = SD_APP_OP_COND;
    cmd.arg = ocr;
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout = DEF_LOW_SPEED_CMD_TIMEOUT;//DEF_CMD_TIME_OUT;
    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);
    // why cmd41 always return crc fail?
    if(cmd.err != SD_OK && cmd.err != SD_CMD_CRC_FAIL)
    {
        SDCARD_WARN("send cmd41 err:%d\r\n", cmd.err);
        return cmd.err;
    }

    return SD_OK;
}

/*ask the CID number on the CMD line*/
// Manufacturer ID	        MID	    8	[127:120]
// OEM/Application          ID	OID	16	[119:104]
// Product name	            PNM	    40	[103:64]
// Product revision	        PRV	    8	[63:56]
// Product serial number	PSN	    32	[55:24]
// reserved	                --	    4	[23:20]
// Manufacturing date	    MDT	    12	[19:8]
static SDIO_Error sdcard_cmd2_process(void)
{
    SDIO_CMD_S cmd;

    cmd.index = ALL_SEND_CID;
    cmd.arg = 0;
    cmd.flags = SD_CMD_LONG;
    cmd.timeout = DEF_LOW_SPEED_CMD_TIMEOUT; // DEF_CMD_TIME_OUT;
    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);

    // dismiss the CID info

    return cmd.err;
}

static SDIO_Error sdcard_mmc_cmd3_process(void)
{
    SDIO_CMD_S cmd;

    cmd.index = SEND_RELATIVE_ADDR;
#if 0
    cmd.arg = 0;
#else
    sdcard.card_rca = 1;
    cmd.arg = (sdcard.card_rca << 16);
#endif
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout = DEF_LOW_SPEED_CMD_TIMEOUT;//DEF_CMD_TIME_OUT;

    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);

    if(cmd.err == SD_CMD_RSP_TIMEOUT)
    {
        SDCARD_WARN("mmc cmd3 noresp \r\n");
        return SD_CMD_RSP_TIMEOUT;
    }
    else if(cmd.err == SD_CMD_CRC_FAIL)
    {
        SDCARD_WARN("mmc cmd3 cmdcrc err\r\n");
        return SD_CMD_CRC_FAIL;
    }

#if 0
    sdio_get_cmdresponse_argument(0, &cmd.resp[0]);
    sdcard.card_rca = (UINT16) (cmd.resp[0] >> 16);
#endif
    SDCARD_PRT("mmc cmd3 is ok, card rca:0x%x\r\n", sdcard.card_rca);
    return SD_OK;
}

/*ask the card to publish a new RCA*/
static SDIO_Error sdcard_cmd3_process(void)
{
    SDIO_CMD_S cmd;

    cmd.index = SEND_RELATIVE_ADDR;
    cmd.arg = 0;
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout = DEF_LOW_SPEED_CMD_TIMEOUT;//DEF_CMD_TIME_OUT;

    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);

    if(cmd.err == SD_CMD_RSP_TIMEOUT)
    {
        SDCARD_WARN("cmd3 noresp \r\n");
        return SD_CMD_RSP_TIMEOUT;
    }
    else if(cmd.err == SD_CMD_CRC_FAIL)
    {
        SDCARD_WARN("cmd3 cmdcrc err\r\n");
        return SD_CMD_CRC_FAIL;
    }

    sdio_get_cmdresponse_argument(0, &cmd.resp[0]);
    sdcard.card_rca = (UINT16) (cmd.resp[0] >> 16);
    SDCARD_PRT("cmd3 is ok, card rca:0x%x\r\n", sdcard.card_rca);
    return SD_OK;
}

#define SD_CARD 0
#define MMC_CARD 1
/*get CSD Register content*/
static SDIO_Error sdcard_cmd9_process(uint8 card_type)
{
    SDIO_CMD_S cmd;
    int mult, csize;

    cmd.index = SEND_CSD;
    cmd.arg = (UINT32)(sdcard.card_rca << 16);
    cmd.flags = SD_CMD_LONG;
    cmd.timeout = DEF_HIGH_SPEED_CMD_TIMEOUT;//DEF_CMD_TIME_OUT;

    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);
    if(cmd.err != SD_OK)
    {
        return cmd.err;
    }

    sdio_get_cmdresponse_argument(0, &cmd.resp[0]);
    sdio_get_cmdresponse_argument(1, &cmd.resp[1]);
    sdio_get_cmdresponse_argument(2, &cmd.resp[2]);

    sdcard.block_size = 1 << ((cmd.resp[1] >> 16) & 0xf);
    //os_printf("arg:%x %x %x %x size:%x\r\n", cmd.resp[0],
    //          cmd.resp[1], cmd.resp[2], cmd.resp[3], sdcard.block_size);

    if(card_type == SD_CARD)
    {

        if((cmd.resp[0] >> 30) & 0x3 == 0)
        {
            csize = (((cmd.resp[1] & 0x3FF ) << 2) | ((cmd.resp[2] >> 30 ) & 0x3));
            mult  = ( cmd.resp[2] >> 15 ) & 0x7;

            sdcard.total_block = (csize + 1 ) * ( 1 << (mult + 2 ) );
            sdcard.total_block *= (sdcard.block_size >> 9);
        }
        else
        {
            csize = (((cmd.resp[1] & 0x3F ) << 16) | ((cmd.resp[2] >> 16 ) & 0xFFFF));
            sdcard.total_block = (csize + 1) * 1024;
        }

        os_printf("size:%x total_block:%x\r\n", sdcard.block_size, sdcard.total_block);
    }
    else
    {
        if(sdcard.Addr_shift_bit != 0)
        {
            csize = (((cmd.resp[1] & 0x3FF ) << 2)
                     | ((cmd.resp[2] >> 30 ) & 0x3));
            mult = ( cmd.resp[2] >> 15 ) & 0x7;

            sdcard.total_block = (csize + 1 ) * ( 1 << (mult + 2 ) );
            sdcard.total_block *= (sdcard.block_size >> 9);
        }
        else
        {
            sdcard.total_block = 0;
        }
    }

    sdcard.block_size = SD_DEFAULT_BLOCK_SIZE;
    SDCARD_PRT("Bsize:%x;Total_block:%x\r\n", sdcard.block_size, sdcard.total_block);
    ASSERT_ERR(sdcard.block_size == SD_DEFAULT_BLOCK_SIZE);

    return SD_OK;
}


/*select/deselect card*/
static SDIO_Error sdcard_cmd7_process(void)
{
    SDIO_CMD_S cmd;

    cmd.index = SELECT_CARD;
    cmd.arg = (UINT32)(sdcard.card_rca << 16);
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout = DEF_HIGH_SPEED_CMD_TIMEOUT;// DEF_CMD_TIME_OUT;

    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);
    return cmd.err;
}

/*set bus width*/
static SDIO_Error sdcard_acmd6_process(void)
{
    SDIO_CMD_S cmd;
    cmd.index = APP_CMD;
    cmd.arg = (UINT32)(sdcard.card_rca << 16);
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout = DEF_HIGH_SPEED_CMD_TIMEOUT;// DEF_CMD_TIME_OUT;
    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);
    if(cmd.err != SD_OK)
        return cmd.err;

    cmd.index = SWITCH_FUNC;
#ifdef CONFIG_SDCARD_BUSWIDTH_4LINE
    cmd.arg = 2;
#else
    cmd.arg = 0;
#endif
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout =  DEF_HIGH_SPEED_CMD_TIMEOUT;//DEF_CMD_TIME_OUT;

    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);

    return cmd.err;
}

static SDIO_Error sdcard_cmd18_process(uint32 addr)
{
    SDIO_CMD_S cmd;

    cmd.index = 18;
    cmd.arg = addr;
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout = 0x90000;
    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);
    return cmd.err;
}
static SDIO_Error sdcard_cmd12_process(uint32 addr)
{
    SDIO_CMD_S cmd;

    cmd.index = 12;
    cmd.arg = addr;
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout = DEF_CMD_TIME_OUT;
    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);
    return cmd.err;
}
static SDIO_Error sdcard_cmd17_process(uint32 addr)
{
    SDIO_CMD_S cmd;

    cmd.index = 17;
    cmd.arg = addr;
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout = 0x90000;
    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);
    return cmd.err;
}


SDIO_Error sdcard_initialize(void)
{
    SDIO_Error err = SD_OK;
    sdio_sw_init();
	rtos_delay_milliseconds(20);
    sdio_hw_init();
	rtos_delay_milliseconds(30);
    // rest card
    err = sdcard_cmd0_process();
    if(err != SD_OK)
    {
        SDCARD_FATAL("send cmd0 err\r\n");
        goto err_return;
    }
    rtos_delay_milliseconds(5);

    rtos_delay_milliseconds(50);
    err = sdcard_cmd1_process();
    os_printf("cmd 1:%x \r\n", err);
    if(err == SD_OK)
    {
        goto MMC_init;
    }
    rtos_delay_milliseconds(5);


    // check support voltage
    err = sdcard_cmd8_process();
    if(err != SD_OK && err != SD_CMD_RSP_TIMEOUT )
    {
        SDCARD_FATAL("send cmd8 err\r\n");
        goto err_return;
    }

    if(err == SD_OK)
    {
        int retry_time = SD_MAX_VOLT_TRIAL;
        UINT32 resp0;
        while(retry_time)
        {
            err = sdcard_acmd41_process(SD_DEFAULT_OCR);
            if(err != SD_OK)
            {
                SDCARD_FATAL("send cmd55&cmd41 err:%d, quite loop\r\n", err);
                goto err_return;
            }
            sdio_get_cmdresponse_argument(0, &resp0);
            if(resp0 & OCR_MSK_BUSY)
            {
                if( resp0 & OCR_MSK_HC )
                    sdcard.Addr_shift_bit = 0;
                else
                    sdcard.Addr_shift_bit = 9;
                break;
            }

			rtos_delay_milliseconds(2);
            retry_time--;
        }
        if(!retry_time)
        {
            SDCARD_FATAL("send cmd55&cmd41 retry time out\r\n");
            return SD_INVALID_VOLTRANGE;
        }

        SDCARD_PRT("send cmd55&cmd41 complete, card is ready\r\n");

        if(resp0 & OCR_MSK_HC)
        {
            SDCARD_PRT("High Capacity SD Memory Card\r\n");
        }
        else
        {
            SDCARD_PRT("Standard Capacity SD Memory Card\r\n");
        }
    }
    else if(err == SD_CMD_RSP_TIMEOUT)
    {
        int retry_time = SD_MAX_VOLT_TRIAL;
        UINT32 resp0;
        while(retry_time)
        {
            err = sdcard_acmd41_process(OCR_MSK_VOLTAGE_ALL);
            if(err != SD_OK)
            {
                SDCARD_FATAL("send cmd55&cmd41 err, quite loop\r\n");
                goto err_return;
            }
            sdio_get_cmdresponse_argument(0, &resp0);
            if(resp0 & OCR_MSK_BUSY)
                break;
			rtos_delay_milliseconds(2);
            retry_time--;
        }
        if(!retry_time)
        {
            SDCARD_FATAL("send cmd55&cmd41 retry time out, maybe a MMC card\r\n");
            err = SD_ERROR;
            goto err_return;
        }
        SDCARD_PRT("send cmd55&cmd41 complete, SD V1.X card is ready\r\n");
    }
	rtos_delay_milliseconds(2);
    // get CID, return R2
    err = sdcard_cmd2_process();
    if(err != SD_OK)
    {
        SDCARD_FATAL("send cmd2 err:%d\r\n", err);
        goto err_return;
    }
	rtos_delay_milliseconds(2);
    // get RCA,
    err = sdcard_cmd3_process();
    if(err != SD_OK)
    {
        SDCARD_FATAL("send cmd3 err:%d\r\n", err);
        goto err_return;
    }

    // change to high speed clk
    sdio_set_high_clk();
	rtos_delay_milliseconds(2);
    // get CSD
    err = sdcard_cmd9_process(SD_CARD);
    if(err != SD_OK)
    {
        SDCARD_FATAL("send cmd9 err:%d\r\n", err);
        goto err_return;
    }
	rtos_delay_milliseconds(2);
    // select card
    err = sdcard_cmd7_process();
    if(err != SD_OK)
    {
        SDCARD_FATAL("send cmd7 err:%d\r\n", err);
        goto err_return;
    }
	rtos_delay_milliseconds(2);
    // change bus width, for high speed
    err = sdcard_acmd6_process();
    if(err != SD_OK)
    {
        SDCARD_FATAL("send acmd6 err:%d\r\n", err);
        goto err_return;
    }

    // Sd_MMC_flag = SD_CARD;
    err = SD_OK;
    SDCARD_PRT("sdcard initialize is done\r\n");
    goto right_return;

MMC_init:
    err = sdcard_cmd2_process();
    os_printf("cmd 2 :%x\r\n", err);
    if(err != SD_OK)
        goto err_return;
    err = sdcard_mmc_cmd3_process();
    os_printf("cmd 3 :%x\r\n", err);
    sdio_set_high_clk();
    err = sdcard_cmd9_process(MMC_CARD);
    os_printf("cmd 9 :%x\r\n", err);
    if(sdcard.Addr_shift_bit == 0)
    {
        err = sdcard_mmc_cmd8_process();
        os_printf("cmd 8 :%x\r\n", err);
    }
    if(err != SD_OK)
        goto err_return;
    err = sdcard_cmd7_process();
    if(err != SD_OK)
        goto err_return;
    // Sd_MMC_flag = MMC_CARD;
    goto right_return;

right_return:
    NoneedInitflag = 1;
err_return:
    sdio_clk_config(0);
    return err;
}

void sdcard_uninitialize(void)
{
    sdio_hw_uninit();
    sdio_sw_init();
    NoneedInitflag = 0;
}

void sdcard_get_card_info(SDCARD_S *card_info)
{
    card_info->total_block = sdcard.total_block;
    card_info->block_size = sdcard.block_size;
    card_info->card_rca = sdcard.card_rca;
    card_info->init_flag = sdcard.init_flag;
    card_info->Addr_shift_bit = sdcard.Addr_shift_bit;
}

SDIO_Error 
sdcard_read_single_block(UINT8 *readbuff, UINT32 readaddr, UINT32 blocksize)
{
    SDIO_CMD_S cmd;
    SDIO_Error ret;
	UINT32 reg;
    sdio_clk_config(1);
#if 0
    if(blocksize != SD_DEFAULT_BLOCK_SIZE)
    {
        ret = SD_ERROR;
        goto read_return;
    }
    if(sdcard.block_size != blocksize)
    {
        ret = SD_ERROR;
        goto read_return;
    }
#endif
	while(1)
	{
		reg = REG_READ(REG_SDCARD_CMD_RSP_INT_SEL);
		if(!(reg & SDCARD_CMDRSP_DATA_BUSY))
			break;			
	}
	REG_WRITE(REG_SDCARD_CMD_RSP_INT_SEL, 0xffffffff);
    // setup data reg first
    sdio_set_data_timeout( DEF_HIGH_SPEED_CMD_TIMEOUT);//DEF_DATA_TIME_OUT);
    sdio_setup_data(SDIO_RD_DATA, blocksize);

    cmd.index = READ_SINGLE_BLOCK;
    cmd.arg = (UINT32)(readaddr << sdcard.Addr_shift_bit);
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout = DEF_HIGH_SPEED_CMD_TIMEOUT;// DEF_CMD_TIME_OUT;
    sdio_send_cmd(&cmd);
    cmd.err = sdio_wait_cmd_response(cmd.index);

    ret = cmd.err;

    if(ret == SD_CMD_RSP_TIMEOUT)
    {
        SDCARD_FATAL("cmd17 noresp, readsingle block err\r\n");
        goto read_return;
    }
    else if(ret == SD_CMD_CRC_FAIL)
    {
        SDCARD_FATAL("cmd17 cmdcrc err, readsingle block err\r\n");
        goto read_return;
    }

    cmd.err = sdcard_wait_receive_data(readbuff);
    ret = cmd.err;

    if(ret != SD_OK)
    {
        SDCARD_FATAL("read single block wait data receive err:%d\r\n", cmd.err);
        goto read_return;
    }
read_return:
    sdio_clk_config(0);
    return ret;
}

SDIO_Error sdcard_write_multi_block(UINT8 *writebuff, UINT32 writeaddr, UINT32 size)
{
    SDIO_CMD_S cmd;
    UINT32 i,j,blk,reg,tmpval;
    int ret;

    blk = size >> 9;//size/512;

    sdio_clk_config(1);
    REG_WRITE(REG_SDCARD_CMD_RSP_INT_SEL, 0xffffffff);
    reg = REG_READ(REG_SDCARD_FIFO_THRESHOLD);

    REG_WRITE(REG_SDCARD_FIFO_THRESHOLD, reg|SDCARD_FIFO_SD_STA_RST);

    reg &= (0xffff|(SDCARD_FIFO_SD_RATE_SELECT_MASK << SDCARD_FIFO_SD_RATE_SELECT_POSI));
    reg |= (0x0101 | SDCARD_FIFO_TX_FIFO_RST);
    REG_WRITE(REG_SDCARD_FIFO_THRESHOLD,reg);

    cmd.index = 25;//WRITE_MULTIPLE_BLOCK;
    cmd.arg = (UINT32)(writeaddr << sdcard.Addr_shift_bit);
    cmd.flags = SD_CMD_SHORT;
    cmd.timeout =  DEF_HIGH_SPEED_CMD_TIMEOUT;//DEF_CMD_TIME_OUT;
    sdio_send_cmd(&cmd);

    cmd.err = sdio_wait_cmd_response(cmd.index);
    ret = cmd.err;

    if(SD_OK == ret)
    {
        ret = sdcard_write_data(writebuff,blk);
    }

    sndcmd12:
    reg = REG_READ(REG_SDCARD_DATA_REC_CTRL);
    reg |= SDCARD_DATA_REC_CTRL_DATA_STOP_EN;
    REG_WRITE(REG_SDCARD_DATA_REC_CTRL,reg);

    reg = REG_READ(REG_SDCARD_CMD_RSP_INT_MASK);
    reg &= ~SDCARD_CMDRSP_TX_FIFO_EMPTY_MASK;
    REG_WRITE(REG_SDCARD_CMD_RSP_INT_MASK,reg);


    ret += sdcard_cmd12_process(DEF_HIGH_SPEED_CMD_TIMEOUT);
    if(ret != SD_OK)
    {
        SDCARD_FATAL("===write err:%x,%x,%x====\r\n",writeaddr,ret, cmd.err);
    }

    sdio_clk_config(0);
    return ret;
}

void sdcard_init(void)
{
    ddev_register_dev(SDCARD_DEV_NAME, &sdcard_op);
    //sdcard_cd_timer_init();
}

void sdcard_exit(void)
{
    ddev_unregister_dev(SDCARD_DEV_NAME);
}


/******************************************************************************/
/***************************** sdcard API function ****************************/
/******************************************************************************/

UINT32 sdcard_open(UINT32 op_flag)
{
    UINT8 cnt;
    UINT32 param, reg;

    rt_kprintf("===sd card open:%d===\r\n",NoneedInitflag);
    cnt = 3;
    while(1)
    {
        if(sdcard_initialize() == SD_OK)
            break;
        if(--cnt == 0)
        {
            SDCARD_FATAL("sdcard_open err\r\n");
            return SDCARD_FAILURE;
        }
    }

    return SDCARD_SUCCESS;
}

UINT32 sdcard_close(void)
{
    sdcard_uninitialize();
    return SDCARD_SUCCESS;
}

UINT32 sdcard_read(char *user_buf, UINT32 count, UINT32 op_flag)
{
    rt_uint32_t result = RT_EOK;
    UINT32 start_blk_addr;
    UINT8  read_blk_numb, numb;
    UINT8* read_data_buf;
    peri_busy_count_add();
    // check operate parameter
    start_blk_addr = op_flag;
    read_blk_numb = count;
    read_data_buf = (UINT8*)user_buf;
    
    {
        for(numb=0; numb<read_blk_numb; numb++)
        {
            result = sdcard_read_single_block(read_data_buf, start_blk_addr, 
                        SD_DEFAULT_BLOCK_SIZE);
            if(result!=SD_OK)
            {
                rt_kprintf("sdcard_read err:%d, curblk:0x%x\r\n",result, start_blk_addr);
                count = 0;
                goto exit;
            }

            start_blk_addr++;
            read_data_buf += SD_DEFAULT_BLOCK_SIZE;
        }
    }
    
exit:
    peri_busy_count_dec();
    return count;
}

UINT32 sdcard_write(char *user_buf, UINT32 count, UINT32 op_flag)
{
    SDIO_Error err = SD_OK;
    UINT32 start_blk_addr;
    UINT32 write_size;

    peri_busy_count_add();
    // check operate parameter
    start_blk_addr = op_flag;
    write_size = count * SD_DEFAULT_BLOCK_SIZE;
    err = sdcard_write_multi_block(user_buf,start_blk_addr,write_size);
    peri_busy_count_dec();

    return err;
}

UINT32 sdcard_ctrl(UINT32 cmd, void *parm)
{
    peri_busy_count_add();

    switch(cmd)
    {
        default:
            break;
    }

    peri_busy_count_dec();

    return 0;
}


int sdcard_get_size( void )
{
    return sdcard.total_block;
}
void clr_sd_noinitial_flag(void)
{
    NoneedInitflag = 0;
}
#endif  // CFG_USE_SDCARD_HOST
// EOF
