/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _YUNOS_BSP_I2S_H
#define _YUNOS_BSP_I2S_H

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/*******************************************************************************
 *                           Include header files                              *
 ******************************************************************************/
#include <stdbool.h>
#include <bsp/yunos_bsp_errno.h>
/*******************************************************************************
 *                             Macro definitions                               *
 ******************************************************************************/
#define YUNOS_BSP_API_I2S_ERRNOTSTARTED				0x0100
#define YUNOS_BSP_API_I2S_ERRNOTOPENED				0x0101
#define YUNOS_BSP_API_I2S_ERRMEMALLOC				0x0102
#define YUNOS_BSP_API_I2S_ERRINVALIDPARAM			0x0103
#define YUNOS_BSP_API_I2S_ERROR						0x0104 
#define YUNOS_BSP_API_I2S_ERROR_BUSY				0x0105

/*******************************************************************************
 *                             Type definitions                                *
 ******************************************************************************/
enum i2s_pcmslotnr {
    PCM_1TIME_SLOT = 0x00,
    PCM_2TIME_SLOT = 0x01,

    MAX_PCM_SLOTNR
};

typedef void (*i2s_aud_cb)(void);

enum i2s_clkmode {
    I2S_TIME_MODE = 0,
    PCM_TIME_MODE,

    MAX_I2S_TIME_MODE
};

enum i2s_ts_cfg {
	I2S_RIGHT_CHANNEL  = 0x01,
	I2S_LEFT_CHANNEL   = 0x02,
	I2S_STEREO_CHANNEL = 0x03,

	I2S_MAX_CHANNEL
};

enum i2s_transmode {
    DATA_ALIGN_STD_I2S,
    DATA_ALIGN_MSB_JUSTIF,
    DATA_ALIGN_LSB_JUSTIF,
    DATA_ALIGN_NORMAL,
    
    MAX_DATA_ALIGN_MODE
};

/**
 * @brief describe the codec agc Parameter 
 * @param buffersize	buffer size
 * @param p_cb 			the callback of audio play	 	
 */
struct i2s_params {
	uint32_t buffersize;
	i2s_aud_cb p_cb;
};

/**
 * @brief describe the i2s device config info
 * @param bMaster		 	TRUE: master mode; FALSE: slave mode
 * @param sample_rate 		sample rate of auido file
 * @param clkmode	 		the clk mode of i2s	 	
 * @param ts_cfg 		tTrackFmt
 * @param tPcmSlotNum 		tPcmSlotNum
 * @param tTransMode 		tTransMode		
 */
struct i2s_fmtcfg {
	bool bmaster;       
	uint32_t sample_rate;    
	enum i2s_clkmode clkmode;       
	enum i2s_ts_cfg ts_cfg;
	enum i2s_pcmslotnr tpcmslotnum;
	enum i2s_transmode  ttransmode;

};

/*******************************************************************************
 *                       Global variable declarations                          *
 ******************************************************************************/


/*******************************************************************************
 *                       Global function declarations                          *
 ******************************************************************************/
/**
* @brief	open i2s device.
*
* @param	i2sTransMode	 enum i2s_transmode 
*
* @return	0-OK, other-error
*/
int32_t yunos_bsp_i2s_open();

/**
* @brief	close i2s device.
*
* @param	void	    
*
* @return	0-OK, other-error
*/
int32_t yunos_bsp_i2s_close();

/**
* @brief	i2s trans data to fifo from ram while playing.
*
* @param	pBuf	 one buffer is how many bytes
* @param	uiLen	 buffer length
*
* @return	0-OK, other-error
*/
int32_t yunos_bsp_i2s_transmit(const uint8_t *pbuf, uint32_t uilen);

/**
* @brief	i2s transmit start .
*
* @param	params	 pointer to struct struct i2s_params
* @param	ptCfg	 pointer to struct t_zdrvi2s_Cfg
* @return	0-DRV_SUCCESS, other-error
*/
int32_t yunos_bsp_i2s_transmit_start(struct i2s_params *params, struct i2s_fmtcfg *ptcfg);

/**
* @brief 	i2s transmit stop ,use to stop playing or recording.
*
* @param	void	    
*
* @return	0-OK, other-error
*/
int32_t yunos_bsp_i2s_transmit_stop();

/**
* @brief  	i2s get buffer ,get buffer to transmit playing data.
*
* @param	pBuf	 playing data buffer
* @param	uiLen	 buffer length
* @return	0-OK, other-error
*/
int32_t yunos_bsp_i2s_transmit_alloc(uint8_t **pbuf, uint32_t *uilen);

/**
* @brief	pause while playing.
*
* @param	void	    
* @return	0-OK, other-error
*/
void yunos_bsp_i2s_transmit_pause();

/**
* @brief	resume playing if pause.
*
* @param	void	    
* @return	0-OK, other-error
*/
void yunos_bsp_i2s_transmit_resume();

#ifdef CONFIG_I2S_RX_MODULE
/**
* @brief	This function is used to read data by i2s.
*
* @param	pBuf	one buffer is how many bytes
* @param	uiLen	buffer length
*
* @return	0-OK, other-error
*/
int32_t yunos_bsp_i2s_receive(uint8_t **pbuf, uint32_t *uilen);

/**
* @brief	i2s read start.
*
* @param	params	 pointer to struct i2s_params
* @param	ptCfg	 pointer to t_zdrvi2s_Cfg
* @return	0-DRV_SUCCESS, other-error
*/
int32_t yunos_bsp_i2s_receive_start(struct i2s_params *params, struct i2s_fmtcfg *ptcfg);

/**
* @brief i2s read stop ,use to stop playing or recording.
*
* @param	void	    
*
* @return	0-OK, other-error
*/
int32_t yunos_bsp_i2s_receive_stop();

/**
* @brief 	i2s free buffer ,get buffer to transmit playing data.
*
* @param	pBuf	 data buffer
* @return	0-OK, other-error
*/
int32_t yunos_bsp_i2s_receive_dealloc(uint8_t *pbuf);

#endif


#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif    /* _YUNOS_BSP_I2S_H */

