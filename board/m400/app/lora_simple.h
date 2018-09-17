/** ****************************************************************************
 * @copyright Copyright (c) XXX
 * All rights reserved.
 *
 * @file    lora_simple.h
 * @brief   lora simple mode
 * @version V1.0.0
 * @author  liucp
 * @date    2018-3-5
 * @warning No Warnings
 * @note <b>history:</b>
 * 1.
 *
 */
#ifndef __LORA_SIMPLE_H__
#define __LORA_SIMPLE_H__

/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "radio.h"
#ifdef __cplusplus
extern "C"{
#endif

/**
 * \defgroup LORA_SIMPLE APP:LORA
 * @{
 */


/*******************************************************************************
 * DEBUG SWITCH MACROS
 */


/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */
/**
 * \brief lora simple callbacks
 */
typedef struct tag_lora_simple_callbacks
{
    void (*rx_done)(uint8_t *a_data, uint16_t a_len, int16_t rssi, int8_t snr);
    void (*rx_timeout)(void);
    void (*rx_error)(void);
    void (*tx_timeout)(void);
    void (*tx_done)(void);
}lora_simp_callbacks_t;

typedef struct tag_lora_simp_para
{
    RadioModems_t modem;
    uint32_t frequency;
    int8_t power;
    uint32_t fdev;
    uint32_t bandwidth;
    uint32_t bandwidthAfc;      /* for rx in fsk */
    uint32_t datarate;
    uint8_t coderate;
    uint16_t preambleLen;
    uint16_t symbTimeout;       /* for rx */
    uint8_t payloadLen;         /* fox rx */
    bool fixLen;
    bool crcOn;
    bool FreqHopOn;
    uint8_t HopPeriod;
    bool iqInverted;
    bool nwk_public;
}lora_simp_para_t;

/*******************************************************************************
 * CONSTANTS
 */


/*******************************************************************************
 * GLOBAL VARIABLES DECLEAR
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 */
/**
 * \brief lora simple init
 * \param [in] a_cb : callbacks used by lora simple module
 * \return none
 */
void lora_simp_init(lora_simp_callbacks_t *a_cb);


/**
 * \brief lora simple tx
 * \param [in] a_tx_para : tx param
 * \param [in] a_data : tx data
 * \param [in] a_len : tx len
 * \return none
 */
void lora_simp_tx(lora_simp_para_t *a_tx_para, uint8_t *a_data, uint16_t a_len);


/**
 * \brief lora simple tx and rx within time
 * \param [in] a_tx_para : tx parameters
 * \param [in] a_data : tx data
 * \param [in] a_len : tx length
 * \param [in] a_rx_para : rx parameters
 * \param [in] a_rx_time : rx time with unit ms
 * \return none
 */
void lora_simp_tx_rx(lora_simp_para_t *a_tx_para,
                        uint8_t *a_data, \
                        uint16_t a_len, \
                        lora_simp_para_t *a_rx_para, \
                        uint32_t a_rx_time);

/**
 * \brief lora simple tx and rx continuous
 * \param [in] a_rx_para : rx parameters
 * \param [in] a_rx_time : tx time in ms
 * \return none
 */
void lora_simp_rx(lora_simp_para_t *a_rx_para, uint32_t a_rx_time);

/**
 * end of group LORA_SIMPLE
 * @}
 */

#ifdef __cplusplus
}
#endif


#endif
