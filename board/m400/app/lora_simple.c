/** ****************************************************************************
 * @copyright Copyright (c) XXX
 * All rights reserved.
 *
 * @file    lora_simple.c
 * @brief   lora simple mode
 * @version V1.0.0
 * @author  liucp
 * @date    2018-3-5
 * @warning none
 * @note <b>history:</b>
 * 1. create file
 *
 */

/*******************************************************************************
 * INCLUDES
 */
#include "lora_simple.h"
#include "radio.h"
#include "sx1276.h"

#include <string.h>

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
 * \defgroup GROUP_LOCAL_FUNCTIONS APP:LORA:LOCAL_FUNCTIONS
 */


/*******************************************************************************
 * CONSTANTS
 */
/*******************************************************************************
 * LOCAL FUNCTIONS DECLEAR
 */
/**
 * \brief on radio tx done
 * \return none
 */
static void lora_simp_on_radio_tx_done(void);

/**
 * \brief on radio rx done
 * \param [in] a_payload : rx payload
 * \param [in] a_size : rx payload size
 * \param [in] a_rssi : rssi
 * \param [in] a_snr : snr
 * \return none
 */
static void lora_simp_on_radio_rx_done(uint8_t *a_payload, \
                                        uint16_t a_size, \
                                        int16_t a_rssi, \
                                        int8_t a_snr \
                                        );

/**
 * \brief on radio tx timeout
 * \return none
 */
static void lora_simp_on_radio_tx_timeout(void);

/**
 * \brief on radio rx timeout
 * \return none
 */
static void lora_simp_on_radio_rx_timeout(void);

/**
 * \brief on radio rx error
 * \return none
 */
static void lora_simp_on_radio_rx_error(void);


/**
 * \brief start rx
 * \param [in] a_para : rx parameters
 * \param [in] a_timeout : rx timeout in ms, if 0, rx continuous, otherwise one
 *                          time
 * \return none
 */
void lora_simp_start_rx(lora_simp_para_t *a_para, uint32_t a_timeout);


/*******************************************************************************
 * GLOBAL VARIABLES
 */


/*******************************************************************************
 * STATIC VARIABLES
 */
RadioEvents_t lt_radio_event;

lora_simp_callbacks_t lt_callback;

lora_simp_para_t lt_rx_param;

uint32_t lu32_rx_timeout;

bool lb_rx_request = false;


/*******************************************************************************
 * EXTERNAL VARIABLES
 */

/*******************************************************************************
 *  GLOBAL FUNCTIONS IMPLEMENTATION
 */
void lora_simp_init(lora_simp_callbacks_t *a_cb)
{
    if (NULL != a_cb)
    {
        memcpy(&lt_callback, a_cb, sizeof(lora_simp_callbacks_t));
    }

    lt_radio_event.TxDone = lora_simp_on_radio_tx_done;
    lt_radio_event.RxDone = lora_simp_on_radio_rx_done;
    lt_radio_event.RxError = lora_simp_on_radio_rx_error;
    lt_radio_event.TxTimeout = lora_simp_on_radio_tx_timeout;
    lt_radio_event.RxTimeout = lora_simp_on_radio_rx_timeout;
    Radio.Init( &lt_radio_event );

//    Radio.SetPublicNetwork( false );
    Radio.Sleep( );

    lb_rx_request = false;
}


void lora_simp_tx(lora_simp_para_t *a_tx_para, uint8_t *a_data, uint16_t a_len)
{
    int8_t phy_tx_power;
    uint32_t tx_timeout = 3000;
    uint32_t bw;

    // TODO: Calculate physical TX power
    phy_tx_power = a_tx_para->power;

    // Setup the radio frequency
    Radio.SetChannel( a_tx_para->frequency );

//    Radio.SetPublicNetwork( a_tx_para->nwk_public );

    if (a_tx_para->modem == MODEM_LORA)
    {
        bw = a_tx_para->bandwidth - 7;
    }
    else
    {
        bw = a_tx_para->bandwidth;
    }

    Radio.SetTxConfig( a_tx_para->modem, \
                        phy_tx_power,
                        a_tx_para->fdev, \
                        bw, \
                        a_tx_para->datarate, \
                        a_tx_para->coderate, \
                        a_tx_para->preambleLen, \
                        a_tx_para->fixLen, \
                        a_tx_para->crcOn, \
                        a_tx_para->FreqHopOn, \
                        a_tx_para->HopPeriod, \
                        a_tx_para->iqInverted, \
                        tx_timeout );
    // Setup maximum payload lenght of the radio driver
    //Radio.SetMaxPayloadLength( MODEM_LORA, txConfig->PktLen );

    // Get the time-on-air of the next tx frame
    //*txTimeOnAir = Radio.TimeOnAir( MODEM_LORA,  a_len );
    if ((a_tx_para->modem == 1) && (a_tx_para->datarate > 10) && (a_tx_para->nwk_public == 0))
    {
        SX1276Write( REG_LR_MODEMCONFIG3,
                     ( SX1276Read( REG_LR_MODEMCONFIG3 ) &
                       RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK ));
    }
        
    Radio.Send(a_data, a_len);

    lb_rx_request = false;
}


void lora_simp_tx_rx(lora_simp_para_t *a_tx_para,
                        uint8_t *a_data, \
                        uint16_t a_len, \
                        lora_simp_para_t *a_rx_para, \
                        uint32_t a_rx_time)
{
    lora_simp_tx(a_tx_para, a_data, a_len);

    if (NULL != a_rx_para)
    {
        lb_rx_request = true;
        memcpy(&lt_rx_param, a_rx_para, sizeof(lora_simp_para_t));
        lu32_rx_timeout = a_rx_time;
    }
}

void lora_simp_rx(lora_simp_para_t *a_rx_para, uint32_t a_rx_timeout)
{
    lora_simp_start_rx(a_rx_para, a_rx_timeout);
}

/*******************************************************************************
 * LOCAL FUNCTIONS IMPLEMENTATION
 */

void lora_simp_on_radio_tx_done(void)
{
    if (true == lb_rx_request)
    {
        lora_simp_start_rx(&lt_rx_param, lu32_rx_timeout);
    }
    else
    {
        Radio.Sleep();
    }

    if (NULL != lt_callback.tx_done)
    {
        lt_callback.tx_done();
    }
}


void lora_simp_on_radio_rx_done(uint8_t *a_payload, \
                                        uint16_t a_size, \
                                        int16_t a_rssi, \
                                        int8_t a_snr \
                                        )
{
    if (0 != lu32_rx_timeout)
    {
        Radio.Sleep();
    }

    if (NULL != lt_callback.rx_done)
    {
        lt_callback.rx_done(a_payload, a_size, a_rssi, a_snr);
    }
}

void lora_simp_on_radio_tx_timeout(void)
{
    if (true == lb_rx_request)
    {
        lora_simp_start_rx(&lt_rx_param, lu32_rx_timeout);
    }
    else
    {
        Radio.Sleep();
    }
}


void lora_simp_on_radio_rx_timeout(void)
{
    if (0 != lu32_rx_timeout)
    {
        Radio.Sleep();
    }

    if (NULL != lt_callback.rx_timeout)
    {
        lt_callback.rx_timeout();
    }

}

void lora_simp_on_radio_rx_error(void)
{
    if (0 != lu32_rx_timeout)
    {
        Radio.Sleep();
    }

    if (NULL != lt_callback.rx_error)
    {
        lt_callback.rx_error();
    }

}


void lora_simp_start_rx(lora_simp_para_t *a_para, uint32_t a_timeout)
{
    uint32_t bw;

    if (NULL != a_para)
    {
        if (a_para->modem == MODEM_LORA)
        {
            bw = a_para->bandwidth - 7;
        }
        else
        {
            bw = a_para->bandwidth;
        }

        lu32_rx_timeout = a_timeout;
        if ( a_timeout > 0 )
        {
            a_para->symbTimeout = 1023; /* 4 ~ 1023 */
        }

        Radio.SetChannel(a_para->frequency);

        //Radio.SetPublicNetwork( a_para->nwk_public );

        Radio.SetRxConfig(a_para->modem, \
                                bw, \
                                a_para->datarate, \
                                a_para->coderate, \
                                a_para->bandwidthAfc, \
                                a_para->preambleLen, \
                                a_para->symbTimeout, \
                                a_para->fixLen, \
                                a_para->payloadLen, \
                                a_para->crcOn, \
                                a_para->FreqHopOn, \
                                a_para->HopPeriod, \
                                a_para->iqInverted, \
                                lu32_rx_timeout == 0 ? true : false);
        
        if ((a_para->modem == 1) && (a_para->datarate > 10) && (a_para->nwk_public == 0))
        {
            SX1276Write( REG_LR_MODEMCONFIG3,
                         ( SX1276Read( REG_LR_MODEMCONFIG3 ) &
                           RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK ));
        }
        Radio.Rx(lu32_rx_timeout);
    }
}
