/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __HAL_IR_H__
#define __HAL_IR_H__

#include "plat_types.h"
#include "irlib.h"
#include "hal_gpio.h"


#ifdef __cplusplus
extern "C" {
#endif

#define CARRIER_FREQ        38000

/* IR duty ratio: on puls in totol*/
typedef enum
{   //debug find that pwm value max 100 !!!
	DUTY_RATIO_1VS4         = 25,//51,   //256,   //0x00,   /* 1:4*/
	DUTY_RATIO_1VS3         = 34,//64,   //256,   //0x01,   /* 1:3*/
	DUTY_RATIO_1VS2         = 50,//85,   //256,   //0x02,   /* 1:2*/
	DUTY_RATIO_1VS1         = 100,//128,  //256,   //0x04,  /* 1:1*/
} IR_DUTY_RATIO_T;
    

/***************************************************************
 * Function     : hal_ir_tx_cb_register
 * Description  : 
 * Input        : cb
****************************************************************/
void hal_ir_tx_cb_register(IR_TX_CB      cb);

/***************************************************************
 * Function     : hal_ir_rx_cb_register
 * Description  : 
 * Input        : cb
****************************************************************/
void hal_ir_rx_cb_register(IR_RX_CB      cb);

/***************************************************************
 * Function     : hal_input_cb_register
 * Description  : 
 * Input        : cb
****************************************************************/
void hal_input_cb_register(INPUT_CB      cb);

/***************************************************************
 * Function     : hal_ir_tx
 * Description  : 发送红外码
 * Input        : proto: 传输的协议； *keycode: 发送的红外码; num:红外码数; repeat: 重复码
 * repeat: bit0 repeat function:
 * bit1~bit7: others potocol cmd
 * Output       : None
 * Return       : 0: success; others: failed
 * //TODO: add mutex for muti access
****************************************************************/
int hal_ir_tx(IR_PROTO_T proto, uint8_t * keycode,uint16_t num,uint8_t repeat);

/***************************************************************
 * Function     : hal_ir_tx_raw
 * Description  : 发送红外帧原始的时间数据
 * Input        : proto: 传输的协议； *frame: 红外帧数据; num:帧个数; repeat: 重复码
 * repeat: bit0 repeat function:
 * Output       : None
 * Return       : 0: success; others: failed
 * //TODO: add mutex for muti access
****************************************************************/
int hal_ir_tx_raw(IR_PROTO_T proto, uint16_t * frame, uint16_t num,uint8_t repeat);

/***************************************************************
 * Function     : hal_ir_tx_init
 * Description  : 
 * Input        : PWM gpio,freq,ratio
 * Output       : None
 * Return       : 0: success; others: failed
****************************************************************/
int hal_ir_tx_init(enum HAL_IOMUX_PIN_T io,uint16_t freq,uint8_t ratio);

/***************************************************************
 * Function     : hal_ir_tx_deinit
 * Description  : 
 * Input        : None 
 * Output       : None
 * Return       : None
****************************************************************/
void hal_ir_tx_deinit(void);

/***************************************************************
 * Function     : hal_ir_rx_init
 * Description  : 
 * Input        : gpio
 * Output       : None
 * Return       : 0: success; others: failed
****************************************************************/
int hal_ir_rx_init(enum HAL_IOMUX_PIN_T gpio);
int hal_input_init(enum HAL_IOMUX_PIN_T gpio);

/***************************************************************
 * Function     : hal_ir_rx_deinit
 * Description  : 
 * Input        : None
 * Output       : None
 * Return       : 0: success; others: failed
****************************************************************/
int hal_ir_rx_deinit(void);

/***************************************************************
 * Function     : hal_ir_init
 * Description  : 
 * Input        : IR_PARAM_S 
 * Output       : None
 * Return       : 0: success; others: failed
****************************************************************/
int hal_ir_init(void);

/***************************************************************
 * Function     : hal_ir_deinit
 * Description  : 
 * Input        : None 
 * Output       : None
 * Return       : 0: success; others: failed
****************************************************************/
int hal_ir_deinit(void);


#ifdef __cplusplus
}
#endif

#endif

