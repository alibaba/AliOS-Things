/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __BES_IRLIB_H__
#define __BES_IRLIB_H__

//#include "net_defs.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "cmsis.h"
#include "hal_timer.h"
#include "hal_cmu.h"
#include "hal_pwm.h"
#include "hal_gpio.h"


#ifdef __cplusplus
extern "C" {
#endif

#if 0
#define ir_printf(s,...) TRACE(s, ##__VA_ARGS__)
#define ir_printf_debug(s,...) TRACE(s, ##__VA_ARGS__)
#define ir_printf_info(s,...) TRACE(s, ##__VA_ARGS__)
#define ir_printf_warn(s,...) TRACE(s, ##__VA_ARGS__)
#else
#define ir_printf(s,...) printf(s, ##__VA_ARGS__)
#define ir_printf_debug(...)
#define ir_printf_info(s,...) printf(s, ##__VA_ARGS__)
#define ir_printf_warn(s,...) printf(s, ##__VA_ARGS__)
#endif

#define MAX_IR_TX_BUF        512


//FUNCTION DEFILE
#define CONFIG_IR_CHECK_ADDR_AND_CODE   1
#define CONFIG_IR_TIMER2                1

/*
 *  NEC protocal Show:
 *  8 bits addr, 8 bits cmd code.
 *  send addr and cmd code twice every time to improve reliable.
 * -----------------------------------------------------------------------------
 * |         Logical '1'         |      Logical '0'     |    Logical 'x'     |
 * -----------------------------------------------------------------------------
 * | 1(carrier)|         0       | 1(carrier)|    0     | 1(carrier)|    -   |
 * -----------------------------------------------------------------------------
 * |   560uS   |      1680uS     |   560uS   |   560uS  |   560uS   |        |
 * |           2.25mS            |        1.12mS        |           |        |
 * -----------------------------------------------------------------------------
 *  1(pulses): f=38KHz, T=26.3us, N=560/26.3=21
 *  38KHz carrier frequency.
 *  bit time is 1.12ms or 2.25ms.
 *  send time is 68ms, send cycle is 110ms.
 
 * example: send 0x89(1001 0001 LSB first) addr, 0x82(0100 0001 LSB first) cmd code.
 * ---------------------------------------------------------------------------------------------------------------------
 * |  S1   | S2  |L1|L0|L0|L1|L0|L0|L0|L1|L0|L1|L1|L0|L1|L1|L1|L0|L0|L1|L0|L0|L0|L0|L0|L1|L1|L1|L0|L1|L0|L1|L1|L1| E1  |
 * |  9mS  |4.5mS|LSB                 MSB|LSB                 MSB|LSB                 MSB|LSB                 MSB|560uS|
 * |    Start    |     0x89 Address      |     ~0x89 Address     |     0x82 Command      |    ~0x82 Command      | End |
 * ---------------------------------------------------------------------------------------------------------------------
 * the repeat code S2 is 2.25mS.
 *
 * code dealt:
 *  0x89 = b 1000 1001 right shift >> dealt bit,so will get: b 1001 0001 (LSB First)
 * ~0x89 = b 0111 0110 right shift >> dealt bit,so will get: b 0110 1110 (LSB First)
 *  0x82 = b 1000 0010 right shift >> dealt bit,so will get: b 0100 0001
 * ~0x82 = b 0111 1101 right shift >> dealt bit,so will get: b 1011 1110
 *
 * 0x0b = b 0000 1011 right shift >> dealt bit,so will get: b 1101 0000 (LSB First)
 * 
*/



/*****************************************************************************
 * struct or unit
*****************************************************************************/

/**
 * Media r05d cmd.
 */
typedef enum
{
	DRV_CMD_R05D_POWEROFF   = 2,     /* R05d power off command */
    DRV_CMD_R05D_NORMAL     = 4,
} IR_R05D_T;

#if 1
/* IR return parameter definition */
typedef enum
{
	IR_OK		            = 0x00, /* IR OK,SUCCESS*/
	IR_BUSY		            = 0x01, /* IR is busy state */
	IR_PARAM_ERROR          = 0x02, /* IR parameters error */
	IR_MEM_ERROR            = 0X03, /* IR memory error */
	IR_UNKNOW        	    = 0x04  /* IR UNKNOW state */
} IR_RET_T;
#endif

/* IR cmd definition */
typedef enum
{
	IR_CMD_TX               = 0x00, /* IR TX */
	IR_CMD_RX               = 0x01, /* IR RX */
	IR_CMD_SLEEP            = 0x02, /* IR SLEEP */
	IR_CMD_EXIT             = 0x03, /* EXIT */
	IR_CMD_ERROR            = 0x04, /* Error */
} IR_CMD_T;

/* IR FINISH EVENT definition */
typedef enum
{
    IR_TX_FINISH_EVENT      = 0x00,  /* IR ONE CODE TRANSMIT FINISHED */
    IR_TX_TIMEOUT_EVENT     = 0x01,  /* IR TX TIMEOUT EVENT */
    IR_TX_ERROR_EVENT       = 0x02,  /* IR TX ERROR EVENT */
} IR_EVENT_T;

/* IR STATE */
typedef enum
{
	IR_IDLE_STATE			= 0x00, /* IDLE MODE*/
	IR_TX_STATE             = 0x01, /* BUSY! TX Doing */
	IR_RX_STATE				= 0X02, /* BUSY! RX Doing */
} IR_STATE_T;

/* IR STUDY STATE */
typedef enum
{
	IR_STU_INIT			    = 0x00, /* Study initial state */
	IR_STU_DOING            = 0x01, /* Study doing */
	IR_STU_EXIT		        = 0X02, /* Study exit */
} IR_STUDY_T;


/*
 * ir protocal.
 */
typedef enum
{
	IR_STUDY_PROTO         = 0,     /* 学习到的码，这里简称学习协议 */
	IR_NEC_PROTO,                   /* NEC protocol */
	IR_ITT_PROTO,                   /* ITT protocol */
	IR_MEDIA_R05D_PROTO,            /* Media R05d protocol */
	IR_GREE_YB0F2_PROTO,            /* GREE YB0F2 protocol*/
	IR_PROTO_MAX
} IR_PROTO_T;


/*****************************************************************************
 * functions
*****************************************************************************/
typedef void (* IR_HW_FUNC)(void);
typedef void (* IR_HW_START_FUNC)(uint32_t time);
typedef void (* IR_CARRIER_INIT_FUNC)(enum HAL_IOMUX_PIN_T gpio);
typedef void (* IR_CARRIER_FUNC)(void);
typedef int (* IR_CARRIER_START_FUNC)(uint32_t carrier_freq, uint8_t freq_ratio);
typedef void (* IR_TX_CB)(int type);
typedef void (* IR_RX_CB)(IR_PROTO_T proto,uint16_t *frame,uint16_t num,uint8_t repeat);
typedef void (* INPUT_CB)(uint32_t tm, enum HAL_GPIO_IRQ_POLARITY_T polarity);

typedef struct {
    IR_HW_FUNC          rtx_timer_init;         /* 红外定时器初始化*/
    IR_HW_START_FUNC    txr_timer_start;        /* 红外定时器设置周期并开启 */
    IR_HW_FUNC          txr_timer_stop;         /* 红外定时器暂停 */
    IR_HW_FUNC          trx_timer_free;         /* 释放红外定时*/
} IR_FUNC_S;

typedef struct {
   IR_CARRIER_INIT_FUNC     carrier_init;      /* 红外载波初始化 */
   IR_CARRIER_START_FUNC    carrier_start;     /* 红外载波发送开始 */
   IR_CARRIER_FUNC          carrier_stop;      /* 红外载波发送停止 */
} IR_CARRI_F_S;

#if 0
typedef struct {
    osMutexId   mutex_id;
#if (osCMSIS >= 0x20000U)
    osMutexAttr_t mutex_attr;
#else
    uint32_t    os_mutex_cb[3];
#endif
}IR_MUTEX_T;
#endif

typedef struct ir_finish_event {
    uint8_t event_id;      /**< even ID */
} IR_EVENT_S;


typedef struct {
    uint8_t     ircode[8];              /* useing bytes to code*/
    uint16_t    nums;                   /* ir code/frame number */
    uint8_t     cmd;                    /* IR RX OR IR TX cmd*/
    uint16_t    *frame;                 /* ir frame time buf */
    IR_PROTO_T  proto;                  /* ir protocal */
    uint8_t     repeat;                 /* ir repeat: 1 true,0: flase ; 像空调这种没有repeat的，可以直接复用代码命令*/ 
    /*定义
    bit0 for repeat
    bit1~4: other cmd
    */
} IR_MESG_S;


typedef struct {
    uint8_t             mode;           /* repeat mode,<1>: repeat mode,<0>: no repeat mode*/
    uint8_t             repeat_sending; /* repeat sending <1>,doing repeat,<0>,normal*/
    uint8_t             framenum;       /* ir repeat frame numbers*/
    uint16_t            frametime[4];   /* ir repeat time buffer */
    //uint32_t          repeat_interval; /* ir repeat interval*/
}IR_REPEAT_S;



/**********************
* 红外载波参数结构体
**********************/
typedef struct {
    enum HAL_PWM_ID_T   id;             /* 载波pwd id */
    uint8_t             ratio;          /* 频率占空比 */
    uint16_t            freq;            /* 载波频率 */
    uint16_t            *txbuf;         /* ir tx time buffer */
    uint16_t            txnum;          /* txbuf time numbers */
    uint8_t             txfinish;       /* 标志红外码是否发送完 */
    IR_REPEAT_S         repeat;
    IR_PROTO_T          prototal;       /* IR protocal */
//    IR_MUTEX_T          tx_mutex;       /* tx mutex */
    IR_CARRI_F_S        *funcs;         /* tx functions */
} IR_CARRI_S;

typedef struct {
    IR_TX_CB            app_tx_cb;      /* ir tx app cb func */
    IR_RX_CB            app_rx_cb;      /* ir rx app cb func */
    INPUT_CB            app_input_cb;   /* input app cb func */
}IR_CB_FUNCS;


/**********************
* 红外接收参数结构体
**********************/
typedef struct {
    uint8_t             rx_mode;        //接收模式：
    uint16_t            *rx_frame;      //每帧时间
    uint16_t            rx_num;
    //uint8_t           rx_triger;      //用于标志收到第一个中断
    IR_STUDY_T          study_state;    //红外学习状态。
    enum HAL_GPIO_PIN_T io;             //GPIO
} IR_RX_S;


/**********************
* 红外参数结构体
**********************/
typedef struct {
    IR_FUNC_S 		*ir_funcs;
	IR_STATE_T      ir_state;
} IR_PARAM_S;


extern IR_CARRI_F_S ir_init_tx_funcs;
extern IR_PARAM_S   ir_pub_parm;
extern IR_CARRI_S   *ir_tx_param;
extern IR_RX_S      ir_rx_parm;
extern IR_FUNC_S    ir_init_cb;


/***************************************************************
 * Function     : ir_get_status
 * Description  : 获取红外工作状态
 * Input        : None
 * Output       : None
 * Return       : IR_STATE_T
****************************************************************/
IR_STATE_T ir_get_status(void);

/***************************************************************
 * Function     : ir_set_status
 * Description  : 设置红外工作状态
 * Input        : IR_STATE_T
 * Output       : None
 * Return       : None
****************************************************************/
void ir_set_status(const IR_STATE_T state);

/***************************************************************
 * Function     : ir_get_status
 * Description  : 获取红外学习状态
 * Input        : None
 * Output       : None
 * Return       : IR_RX_S
****************************************************************/
IR_STUDY_T ir_rx_get_status(void);

/***************************************************************
 * Function     : ir_rx_set_status
 * Description  : 设置红外学习状态
 * Input        : IR_STUDY_T
 * Output       : None
 * Return       : None
****************************************************************/
void ir_rx_set_status(const IR_STUDY_T state);


/***************************************************************
 * Function     : ir_rx_gpio_irq_enable
 * Description  : 
 * Input        : None
 * Output       : None
 * Return       : 0: success; others: failed
****************************************************************/
uint8_t ir_rx_gpio_irq_enable(void);


/***************************************************************
 * Function     : ir_rx_gpio_irq_disable
 * Description  : 
 * Input        : None
 * Output       : None
 * Return       : 0: success; others: failed
****************************************************************/
uint8_t ir_rx_gpio_irq_disable(void);


/***************************************************************
 * Function 	: ir_mailbox_put
 * Description	: 
 * Input		: IR_MESG_S *
 * Output		: None
 * Return		: 0: success; others: failed
****************************************************************/
int ir_mailbox_put(IR_MESG_S *input);

/***************************************************************
 * Function     : ir_mailbox_init
 * Description  : 
 * Input        : None
 * Output       : None
 * Return       : 0: success; others: failed
****************************************************************/
int ir_mailbox_init(void);

/***************************************************************
 * Function     : ir_tx_feedback_mailbox_init
 * Description  : 
 * Input        : None
 * Output       : None
 * Return       : 0: success; others: failed
****************************************************************/
//int ir_tx_feedback_mailbox_init(void);

/***************************************************************
 * Function     : ir_repeat_timer_init
 * Description  : 
 * Input        : None
 * Output       : None
 * Return       : None
****************************************************************/
void ir_repeat_timer_init(void);

/***************************************************************
 * Function     : ir_repeat_timer_del
 * Description  : 
 * Input        : None
 * Output       : None
 * Return       : None
****************************************************************/
void ir_repeat_timer_del(void);

/******************************************************************************
 * Function     : ir_create_thread
 * Description  : 创建IR线程处理函数
 * Input        : None
 * Output       : None
 * Return       : 0: success,others: failed!
******************************************************************************/
int ir_create_thread(void);


#ifdef __cplusplus
}
#endif

#endif
