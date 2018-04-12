/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2016-2017 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_sci_rx.c
* Description  : Functions for using SCI on RX devices. 
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           01.10.2016 1.80    Initial Release. (The remake of the r01an1815ju0170 to the base.)
*           19.12.2016 1.90    FIT_NO_PTR check added to NULL check.
*                              Fixed a bug that may receive data more than the specified number of bytes
*                               on Clock Synchronous Mode.
*                              Fixed that R_SCI_Control function returns SCI_ERR_INVALID_ARG
*                               when using SCI_CMD_EN_CTS_IN on Simple SPI mode.
*                              Fix to clear error flag even if callback function is not set.
*                              Deleted unnecessary bit mask of SSR register from sci_error function.
*           07.03.2017 2.00    Fixed a bug that error condition not clear when FIFO enabled.
*                              Fixed a bug that where commands used only when FIFO mode is enable did not NULL check.
*                              Fixed a bug that sending data is overwrote by new R_SCI_Send() when FIFO(async) enabled.
*                              Fixed a bug that sending data is break up by new R_SCI_Send() when FIFO(sync) enabled.
*                              Fixed a bug that the new FIFO threshold was retained only on first receive.
*                              Fixed a bug that callback function work many times at receive interrupt
*                               when FIFO(async) enabled.
*                              Fixed a bug that the interrupt priority level can be changed only in async mode.
***********************************************************************************************************************/

/*****************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "rx_platform.h"

/* Defines for SCI support */
#include "r_sci_rx_private.h"

/* Include specifics for chosen MCU.  */
#include "r_sci_rx_platform.h"

#if (SCI_CFG_ASYNC_INCLUDED)
#include "r_byteq_if.h"
#endif
#include <aos/aos.h>
#include "uart/uart.h"
extern _uart_drv uart_drv[];
/*****************************************************************************
Typedef definitions
******************************************************************************/

/*****************************************************************************
Macro definitions
******************************************************************************/

/*****************************************************************************
Private global variables and functions
******************************************************************************/
#if (SCI_CFG_ASYNC_INCLUDED)
static sci_err_t sci_init_async(sci_hdl_t const     hdl,
                                sci_uart_t * const  p_cfg,
                                uint8_t * const     p_priority);

static sci_err_t sci_init_queues(uint8_t const  chan);

static sci_err_t sci_send_async_data(sci_hdl_t const hdl,
                                     uint8_t         *p_src,
                                     uint16_t const  length);

static byteq_err_t sci_put_byte(sci_hdl_t const    hdl,
                                uint8_t const      byte);

static void sci_transfer(sci_hdl_t const hdl);

#if SCI_CFG_FIFO_INCLUDED
static void sci_fifo_transfer(sci_hdl_t const hdl);
#endif

static sci_err_t sci_receive_async_data(sci_hdl_t const hdl,
                                        uint8_t         *p_dst,
                                        uint16_t const  length);
#endif

#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
static sci_err_t sci_init_sync(sci_hdl_t const         hdl,
                               sci_sync_sspi_t * const p_cfg,
                               uint8_t * const         p_priority);

static sci_err_t sci_send_sync_data(sci_hdl_t const hdl,
                                    uint8_t         *p_src,
                                    uint8_t         *p_dst,
                                    uint16_t const  length,
                                    bool            save_rx_data);

static sci_err_t sci_receive_sync_data(sci_hdl_t const hdl,
                                       uint8_t         *p_dst,
                                       uint16_t const  length);
#endif

static void power_on(sci_hdl_t const hdl);
static void power_off(sci_hdl_t const hdl);

#if SCI_CFG_FIFO_INCLUDED
static sci_err_t sci_init_fifo(sci_hdl_t const hdl);
#endif

static void sci_receive(sci_hdl_t const hdl);

#if SCI_CFG_FIFO_INCLUDED

#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
static void sci_fifo_receive_sync(sci_hdl_t const hdl);
#endif

static void sci_fifo_receive(sci_hdl_t const hdl);

#endif

static void sci_error(sci_hdl_t const hdl);

#if SCI_CFG_FIFO_INCLUDED
static void sci_fifo_error(sci_hdl_t const hdl);
#endif

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)

#if SCI_CFG_CH0_INCLUDED
static uint8_t  ch0_tx_buf[SCI_CFG_CH0_TX_BUFSIZ];
static uint8_t  ch0_rx_buf[SCI_CFG_CH0_RX_BUFSIZ];
#endif

#if SCI_CFG_CH1_INCLUDED
static uint8_t  ch1_tx_buf[SCI_CFG_CH1_TX_BUFSIZ];
static uint8_t  ch1_rx_buf[SCI_CFG_CH1_RX_BUFSIZ];
#endif

#if SCI_CFG_CH2_INCLUDED
static uint8_t  ch2_tx_buf[SCI_CFG_CH2_TX_BUFSIZ];
static uint8_t  ch2_rx_buf[SCI_CFG_CH2_RX_BUFSIZ];
#endif

#if SCI_CFG_CH3_INCLUDED
static uint8_t  ch3_tx_buf[SCI_CFG_CH3_TX_BUFSIZ];
static uint8_t  ch3_rx_buf[SCI_CFG_CH3_RX_BUFSIZ];
#endif

#if SCI_CFG_CH4_INCLUDED
static uint8_t  ch4_tx_buf[SCI_CFG_CH4_TX_BUFSIZ];
static uint8_t  ch4_rx_buf[SCI_CFG_CH4_RX_BUFSIZ];
#endif

#if SCI_CFG_CH5_INCLUDED
static uint8_t  ch5_tx_buf[SCI_CFG_CH5_TX_BUFSIZ];
static uint8_t  ch5_rx_buf[SCI_CFG_CH5_RX_BUFSIZ];
#endif

#if SCI_CFG_CH6_INCLUDED
static uint8_t  ch6_tx_buf[SCI_CFG_CH6_TX_BUFSIZ];
static uint8_t  ch6_rx_buf[SCI_CFG_CH6_RX_BUFSIZ];
#endif

#if SCI_CFG_CH7_INCLUDED
static uint8_t  ch7_tx_buf[SCI_CFG_CH7_TX_BUFSIZ];
static uint8_t  ch7_rx_buf[SCI_CFG_CH7_RX_BUFSIZ];
#endif

#if SCI_CFG_CH8_INCLUDED
static uint8_t  ch8_tx_buf[SCI_CFG_CH8_TX_BUFSIZ];
static uint8_t  ch8_rx_buf[SCI_CFG_CH8_RX_BUFSIZ];
#endif

#if SCI_CFG_CH9_INCLUDED
static uint8_t  ch9_tx_buf[SCI_CFG_CH9_TX_BUFSIZ];
static uint8_t  ch9_rx_buf[SCI_CFG_CH9_RX_BUFSIZ];
#endif

#if SCI_CFG_CH10_INCLUDED
static uint8_t  ch10_tx_buf[SCI_CFG_CH10_TX_BUFSIZ];
static uint8_t  ch10_rx_buf[SCI_CFG_CH10_RX_BUFSIZ];
#endif

#if SCI_CFG_CH11_INCLUDED
static uint8_t  ch11_tx_buf[SCI_CFG_CH11_TX_BUFSIZ];
static uint8_t  ch11_rx_buf[SCI_CFG_CH11_RX_BUFSIZ];
#endif

#if SCI_CFG_CH12_INCLUDED
static uint8_t  ch12_tx_buf[SCI_CFG_CH12_TX_BUFSIZ];
static uint8_t  ch12_rx_buf[SCI_CFG_CH12_RX_BUFSIZ];
#endif

#endif /* #if (SCI_CFG_ASYNC_INCLUDED) */

extern const sci_hdl_t g_handles[SCI_NUM_CH];


/*****************************************************************************
* Function Name: R_SCI_Open
* Description  : Initializes an SCI channel for a particular mode.
*
* NOTE: The associated port must be configured/initialized prior to
*       calling this function.
*
* Arguments    : chan -
*                    channel to initialize
*                mode -
*                    operational mode (UART, SPI, I2C, ...)
*                p_cfg -
*                    ptr to configuration union; structure specific to mode
*                p_callback -
*                    ptr to function called from interrupt when a receiver 
*                    error is detected or for transmit end (TEI) condition
*                p_hdl -
*                     pointer to a handle for channel (value set here)
* Return Value : SCI_SUCCESS -
*                    channel opened successfully
*                SCI_ERR_BAD_CHAN -
*                    channel number invalid for part
*                SCI_ERR_OMITTED_CHAN -
*                    channel not included in config.h
*                SCI_ERR_CH_NOT_CLOSED -
*                    channel already in use
*                SCI_ERR_BAD_MODE -
*                    unsupported mode
*                SCI_ERR_NULL_PTR -
*                    missing required p_cfg argument
*                SCI_ERR_INVALID_ARG -
*                    element of casted mode config structure (p_cfg) is invalid
*                SCI_ERR_QUEUE_UNAVAILABLE -
*                    cannot open transmit or receive queue or both
******************************************************************************/
sci_err_t R_SCI_Open(uint8_t const      chan,
                     sci_mode_t const   mode,
                     sci_cfg_t * const  p_cfg,
                     void               (* const p_callback)(void *p_args),
                     sci_hdl_t * const  p_hdl)
{
    sci_err_t   err=SCI_SUCCESS;
    uint8_t     priority=1;

    /* CHECK ARGUMENTS */
#if SCI_CFG_PARAM_CHECKING_ENABLE
    err = sci_mcu_param_check(chan);
    if (SCI_SUCCESS != err)
    {
        return err;
    }
    if ((NULL == g_handles[chan]) || (FIT_NO_PTR == g_handles[chan]))
    {
        return SCI_ERR_OMITTED_CHAN;
    }
    if (SCI_MODE_OFF != g_handles[chan]->mode)
    {
        return SCI_ERR_CH_NOT_CLOSED;
    }
    if ((SCI_MODE_OFF == mode) || (SCI_MODE_MAX <= mode))
    {
        return SCI_ERR_BAD_MODE;
    }
    if (((NULL == p_cfg) || (NULL == p_hdl)) || ((FIT_NO_PTR == p_cfg) || (FIT_NO_PTR == p_hdl)))
    {
        return SCI_ERR_NULL_PTR;
    }
#endif
    
     /* APPLY POWER TO CHANNEL */
    power_on(g_handles[chan]);

    /* INITIALIZE REGISTER */
    sci_init_register(g_handles[chan]);

    /* INITIALIZE MODE SPECIFIC FEATURES */
    g_handles[chan]->mode = mode;
    if (SCI_MODE_ASYNC == mode)
    {
#if (SCI_CFG_ASYNC_INCLUDED)
        err = sci_init_async(g_handles[chan], (sci_uart_t *)p_cfg, &priority);
#endif
    }
    else
    {
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
        err = sci_init_sync(g_handles[chan], (sci_sync_sspi_t *)p_cfg, &priority);
#endif
    }

    if (SCI_SUCCESS != err)
    {
        g_handles[chan]->mode = SCI_MODE_OFF;
        return err;
    }
    g_handles[chan]->callback = p_callback;

    /* INITIALIZE TX AND RX QUEUES */
#if (SCI_CFG_ASYNC_INCLUDED)
    if (SCI_MODE_ASYNC == mode)
    {
        err = sci_init_queues(chan);
        if (SCI_SUCCESS != err)
        {
            g_handles[chan]->mode = SCI_MODE_OFF;
            return err;
        }
    }
#endif

#if SCI_CFG_FIFO_INCLUDED
    if (true == g_handles[chan]->fifo_ctrl)
    {
        /* INITIALIZE TX AND RX FIFO */
        err = sci_init_fifo(g_handles[chan]);
        if (SCI_SUCCESS != err)
        {
            g_handles[chan]->mode = SCI_MODE_OFF;
            return err;
        }
    }
#endif

    /* ENABLE INTERRUPTS */
    sci_initialize_ints(g_handles[chan], priority);

    /* FINISH */
    *p_hdl = g_handles[chan];

    return SCI_SUCCESS;
}  /* End of function R_SCI_Open() */

/*****************************************************************************
* Function Name: power_on
* Description  : This function provides power to the channel referenced by
*                the handle by taking it out of the module stop state.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void power_on(sci_hdl_t const hdl)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    (*hdl->rom->mstp) &= (~hdl->rom->stop_mask);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

    return;
}  /* End of function power_on() */

/*****************************************************************************
* Function Name: power_off
* Description  : This function removes power to the channel referenced by
*                handle by putting it into the module stop state.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void power_off(sci_hdl_t const hdl)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    (*hdl->rom->mstp) |= (hdl->rom->stop_mask);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

    return;
}  /* End of function power_off() */

#if (SCI_CFG_ASYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_init_queues
* Description  : This function attaches transmit and receive queues to the
*                channel.
*
* Arguments    : chan -
*                    channel (ptr to chan control block)
* Return Value : SCI_SUCCESS -
*                    channel initialized successfully
*                SCI_ERR_QUEUE_UNAVAILABLE -
*                    no queue control blocks available
******************************************************************************/
static sci_err_t sci_init_queues(uint8_t const chan)
{
    byteq_err_t q_err1=BYTEQ_ERR_INVALID_ARG;
    byteq_err_t q_err2=BYTEQ_ERR_INVALID_ARG;
    sci_err_t   err=SCI_SUCCESS;

    /* channel number verified as legal prior to calling this function */
    switch (chan)
    {
#if SCI_CFG_CH0_INCLUDED
    case (SCI_CH0):
        q_err1 = R_BYTEQ_Open(ch0_tx_buf, SCI_CFG_CH0_TX_BUFSIZ, &g_handles[SCI_CH0]->u_tx_data.que);
        q_err2 = R_BYTEQ_Open(ch0_rx_buf, SCI_CFG_CH0_RX_BUFSIZ, &g_handles[SCI_CH0]->u_rx_data.que);
        break;
#endif
#if SCI_CFG_CH1_INCLUDED
    case (SCI_CH1):
        q_err1 = R_BYTEQ_Open(ch1_tx_buf, SCI_CFG_CH1_TX_BUFSIZ, &g_handles[SCI_CH1]->u_tx_data.que);
        q_err2 = R_BYTEQ_Open(ch1_rx_buf, SCI_CFG_CH1_RX_BUFSIZ, &g_handles[SCI_CH1]->u_rx_data.que);
        break;
#endif
#if SCI_CFG_CH2_INCLUDED
    case (SCI_CH2):
        q_err1 = R_BYTEQ_Open(ch2_tx_buf, SCI_CFG_CH2_TX_BUFSIZ, &g_handles[SCI_CH2]->u_tx_data.que);
        q_err2 = R_BYTEQ_Open(ch2_rx_buf, SCI_CFG_CH2_RX_BUFSIZ, &g_handles[SCI_CH2]->u_rx_data.que);
        break;
#endif
#if SCI_CFG_CH3_INCLUDED
    case (SCI_CH3):
        q_err1 = R_BYTEQ_Open(ch3_tx_buf, SCI_CFG_CH3_TX_BUFSIZ, &g_handles[SCI_CH3]->u_tx_data.que);
        q_err2 = R_BYTEQ_Open(ch3_rx_buf, SCI_CFG_CH3_RX_BUFSIZ, &g_handles[SCI_CH3]->u_rx_data.que);
        break;
#endif
#if SCI_CFG_CH4_INCLUDED
    case (SCI_CH4):
        q_err1 = R_BYTEQ_Open(ch4_tx_buf, SCI_CFG_CH4_TX_BUFSIZ, &g_handles[SCI_CH4]->u_tx_data.que);
        q_err2 = R_BYTEQ_Open(ch4_rx_buf, SCI_CFG_CH4_RX_BUFSIZ, &g_handles[SCI_CH4]->u_rx_data.que);
    break;
#endif
#if SCI_CFG_CH5_INCLUDED
    case (SCI_CH5):
        q_err1 = R_BYTEQ_Open(ch5_tx_buf, SCI_CFG_CH5_TX_BUFSIZ, &g_handles[SCI_CH5]->u_tx_data.que);
        q_err2 = R_BYTEQ_Open(ch5_rx_buf, SCI_CFG_CH5_RX_BUFSIZ, &g_handles[SCI_CH5]->u_rx_data.que);
        break;
#endif
#if SCI_CFG_CH6_INCLUDED
    case (SCI_CH6):
        q_err1 = R_BYTEQ_Open(ch6_tx_buf, SCI_CFG_CH6_TX_BUFSIZ, &g_handles[SCI_CH6]->u_tx_data.que);
        q_err2 = R_BYTEQ_Open(ch6_rx_buf, SCI_CFG_CH6_RX_BUFSIZ, &g_handles[SCI_CH6]->u_rx_data.que);
        break;
#endif
#if SCI_CFG_CH7_INCLUDED
    case (SCI_CH7):
        q_err1 = R_BYTEQ_Open(ch7_tx_buf, SCI_CFG_CH7_TX_BUFSIZ, &g_handles[SCI_CH7]->u_tx_data.que);
        q_err2 = R_BYTEQ_Open(ch7_rx_buf, SCI_CFG_CH7_RX_BUFSIZ, &g_handles[SCI_CH7]->u_rx_data.que);
        break;
#endif
#if SCI_CFG_CH8_INCLUDED
    case (SCI_CH8):
        q_err1 = R_BYTEQ_Open(ch8_tx_buf, SCI_CFG_CH8_TX_BUFSIZ, &g_handles[SCI_CH8]->u_tx_data.que);
        q_err2 = R_BYTEQ_Open(ch8_rx_buf, SCI_CFG_CH8_RX_BUFSIZ, &g_handles[SCI_CH8]->u_rx_data.que);
        break;
#endif
#if SCI_CFG_CH9_INCLUDED
    case (SCI_CH9):
        q_err1 = R_BYTEQ_Open(ch9_tx_buf, SCI_CFG_CH9_TX_BUFSIZ, &g_handles[SCI_CH9]->u_tx_data.que);
        q_err2 = R_BYTEQ_Open(ch9_rx_buf, SCI_CFG_CH9_RX_BUFSIZ, &g_handles[SCI_CH9]->u_rx_data.que);
        break;
#endif
#if SCI_CFG_CH10_INCLUDED
    case (SCI_CH10):
        q_err1 = R_BYTEQ_Open(ch10_tx_buf, SCI_CFG_CH10_TX_BUFSIZ, &g_handles[SCI_CH10]->u_tx_data.que);
        q_err2 = R_BYTEQ_Open(ch10_rx_buf, SCI_CFG_CH10_RX_BUFSIZ, &g_handles[SCI_CH10]->u_rx_data.que);
        break;
#endif
#if SCI_CFG_CH11_INCLUDED
    case (SCI_CH11):
        q_err1 = R_BYTEQ_Open(ch11_tx_buf, SCI_CFG_CH11_TX_BUFSIZ, &g_handles[SCI_CH11]->u_tx_data.que);
        q_err2 = R_BYTEQ_Open(ch11_rx_buf, SCI_CFG_CH11_RX_BUFSIZ, &g_handles[SCI_CH11]->u_rx_data.que);
        break;
#endif
#if SCI_CFG_CH12_INCLUDED
    case (SCI_CH12):
        q_err1 = R_BYTEQ_Open(ch12_tx_buf, SCI_CFG_CH12_TX_BUFSIZ, &g_handles[SCI_CH12]->u_tx_data.que);
        q_err2 = R_BYTEQ_Open(ch12_rx_buf, SCI_CFG_CH12_RX_BUFSIZ, &g_handles[SCI_CH12]->u_rx_data.que);
        break;
#endif
    default:
        err = SCI_ERR_QUEUE_UNAVAILABLE;
        break;
    }

    if ((BYTEQ_SUCCESS != q_err1) || (BYTEQ_SUCCESS != q_err2))
    {
        err = SCI_ERR_QUEUE_UNAVAILABLE;
    }
    return err;
}  /* End of function sci_init_queues() */
#endif

#if SCI_CFG_FIFO_INCLUDED
/*****************************************************************************
* Function Name: sci_init_fifo
* Description  : This function the setting of the FIFO mode, reset of the
*                TX/RX FIFO, and the threshold setting of the TX/RX FIFO.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
* Return Value : SCI_SUCCESS - 
*                    fifo initialized successfully
*                SCI_ERR_INVALID_ARG -
*                    element of hdl contains illegal value
******************************************************************************/
static sci_err_t sci_init_fifo(sci_hdl_t const hdl)
{
    /* CHECK ARGUMENTS */
#if SCI_CFG_PARAM_CHECKING_ENABLE
    if (hdl->tx_dflt_thresh > 15)
    {
        return SCI_ERR_INVALID_ARG;
    }
    if ((hdl->rx_dflt_thresh < 1) || (hdl->rx_dflt_thresh > 15))
    {
        return SCI_ERR_INVALID_ARG;
    }
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
    if (hdl->tx_dflt_thresh != hdl->rx_dflt_thresh)
    {
        return SCI_ERR_INVALID_ARG;
    }
#endif
#endif

    /* FIFO Mode Select (1:FIFO mode) */
    hdl->rom->regs->FCR.BIT.FM = 0x01;

    /* reset TX/RX FIFO */
    hdl->rom->regs->FCR.BIT.TFRST = 0x01;
    hdl->rom->regs->FCR.BIT.RFRST = 0x01;

    /* set TX/RX FIFO threshold initial value */
    hdl->rom->regs->FCR.BIT.TTRG = hdl->tx_dflt_thresh;
    hdl->rom->regs->FCR.BIT.RTRG = hdl->rx_dflt_thresh;

    return SCI_SUCCESS;
}  /* End of function sci_init_fifo() */
#endif

#if (SCI_CFG_ASYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_init_async
* Description  : This function initializes the control block and UART 
*                registers for an SCI channel.
*
* NOTE: p_cfg is checked to be non-NULL prior to this function.
*       The TE and RE bits in SCR must be 0 prior to calling this function.
*
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                p_cfg -
*                    ptr to Uart configuration argument structure
*                p_priority -
*                    pointer to location to load interrupt priority into
* Return Value : SCI_SUCCESS - 
*                    channel initialized successfully
*                SCI_ERR_INVALID_ARG -
*                    element of p_cfg contains illegal value
******************************************************************************/
static sci_err_t sci_init_async(sci_hdl_t const      hdl,
                                sci_uart_t * const   p_cfg,
                                uint8_t * const      p_priority)
{
    sci_err_t   err=SCI_SUCCESS;
    int32_t     bit_err;

    /* Check arguments */    

#if SCI_CFG_PARAM_CHECKING_ENABLE
    if (((SCI_DATA_8BIT != p_cfg->data_size) && (SCI_DATA_7BIT != p_cfg->data_size))
     || ((SCI_STOPBITS_1 != p_cfg->stop_bits) && (SCI_STOPBITS_2 != p_cfg->stop_bits))
     || ((p_cfg->int_priority < (BSP_MCU_IPL_MIN+1)) || (p_cfg->int_priority > BSP_MCU_IPL_MAX)))
    {
        return SCI_ERR_INVALID_ARG;
    }

    if (SCI_PARITY_ON == p_cfg->parity_en)
    {
        if ((SCI_EVEN_PARITY != p_cfg->parity_type) && (SCI_ODD_PARITY != p_cfg->parity_type))
        {
            return SCI_ERR_INVALID_ARG;
        }
    }
    else if (SCI_PARITY_OFF != p_cfg->parity_en)
    {
        return SCI_ERR_INVALID_ARG;
    }
    else
    {
        /* Do Nothing */
    }
    if (SCI_CLK_INT == p_cfg->clk_src)
    {
        if (0 == p_cfg->baud_rate)
        {
            return SCI_ERR_INVALID_ARG;
        }
    }
    else if ((SCI_CLK_EXT8X != p_cfg->clk_src) && (SCI_CLK_EXT16X != p_cfg->clk_src))
    {
        return SCI_ERR_INVALID_ARG;
    }
    else
    {
        /* Do Nothing */
    }
#endif


    /* Initialize channel control block flags */
    hdl->tx_idle = true;

        
    /* Configure SMR for asynchronous mode, single processor, and user settings */
    if (SCI_PARITY_OFF == p_cfg->parity_en)
    {
        p_cfg->parity_type = 0;         // ensure random value is not ORed into SMR
    }
    hdl->rom->regs->SMR.BYTE =
            (uint8_t)((p_cfg->data_size | p_cfg->stop_bits) | (p_cfg->parity_en | p_cfg->parity_type));
    
    /* SETUP CLOCK FOR BAUD RATE */

    if (SCI_CLK_INT == p_cfg->clk_src)
    {
        /* Use internal clock for baud rate */
        bit_err = sci_init_bit_rate(hdl, hdl->pclk_speed, p_cfg->baud_rate);
        if (1000 == bit_err)
        {
            err = SCI_ERR_INVALID_ARG;          // impossible baud rate; 100% error
        }
        else
        {
            hdl->baud_rate = p_cfg->baud_rate;   // save baud rate for break generation
        }
    }
    else
    {
        /* Use external clock for baud rate */
        hdl->rom->regs->SCR.BIT.CKE = 0x02;
        hdl->rom->regs->SEMR.BIT.ABCS = (SCI_CLK_EXT8X == p_cfg->clk_src) ? 1 : 0;
    }

    *p_priority = p_cfg->int_priority;
    return err;
}  /* End of function sci_init_async() */
#endif

#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_init_sync
* Description  : This function initializes the control block and SYNC/SSPI
*                registers for an SCI channel.
*
* NOTE: p_cfg is checked to be non-NULL prior to this function.
*       The TE and RE bits in SCR must be 0 prior to calling this function.
*
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                p_cfg -
*                    ptr to SSPI configuration argument structure
*                p_priority -
*                    pointer to location to load interrupt priority into
* Return Value : SCI_SUCCESS -
*                    channel initialized successfully
*                SCI_ERR_INVALID_ARG -
*                    element of p_cfg contains illegal value
******************************************************************************/
static sci_err_t sci_init_sync(sci_hdl_t const         hdl,
                               sci_sync_sspi_t * const p_cfg,
                               uint8_t * const         p_priority)
{
    sci_err_t   err=SCI_SUCCESS;
    int32_t     bit_err;


    /* Check arguments */

#if SCI_CFG_PARAM_CHECKING_ENABLE
    if ((SCI_MODE_SSPI == hdl->mode)
     && (SCI_SPI_MODE_0 != p_cfg->spi_mode) && (SCI_SPI_MODE_1 != p_cfg->spi_mode)
     && (SCI_SPI_MODE_2 != p_cfg->spi_mode) && (SCI_SPI_MODE_3 != p_cfg->spi_mode))
    {
        return SCI_ERR_INVALID_ARG;
    }
    else if ((SCI_MODE_SYNC == hdl->mode) && (SCI_SPI_MODE_OFF != p_cfg->spi_mode))
    {
        return SCI_ERR_INVALID_ARG;
    }

    if (0 == p_cfg->bit_rate)
    {
        return SCI_ERR_INVALID_ARG;
    }

    if ((0 == p_cfg->int_priority) || (p_cfg->int_priority > BSP_MCU_IPL_MAX))
    {
        return SCI_ERR_INVALID_ARG;
    }
#endif


    /* Initialize channel control block flags */
    hdl->tx_idle = true;
    hdl->tx_dummy = false;

    /* Configure SMR for SSPI/SYNC mode */
    hdl->rom->regs->SMR.BYTE = 0x80;
    hdl->rom->regs->SCMR.BIT.SMIF = 0;          /* default */
    hdl->rom->regs->SIMR1.BIT.IICM = 0;         /* default */

    /* Configure SPI register for clock polarity/phase and single master */
    if (SCI_MODE_SSPI == hdl->mode)
    {
        hdl->rom->regs->SPMR.BYTE = p_cfg->spi_mode;
    }
    else    /* synchronous operation */
    {
        hdl->rom->regs->SPMR.BYTE = 0;
    }

    /* Configure bit order and data inversion */
    hdl->rom->regs->SCMR.BIT.SINV = (uint8_t)((p_cfg->invert_data == true) ? 1 : 0);
    hdl->rom->regs->SCMR.BIT.SDIR = (uint8_t)((p_cfg->msb_first == true) ? 1 : 0);


    /* SETUP CLOCK FOR BIT RATE */

    /* Use internal clock for bit rate (master) */
    bit_err = sci_init_bit_rate(hdl, hdl->pclk_speed, p_cfg->bit_rate);
    if (1000 == bit_err)
    {
        err = SCI_ERR_INVALID_ARG;      /* impossible bit rate; 100% error */
    }

    *p_priority = p_cfg->int_priority;
    return err;
} /* End of function sci_init_sync() */
#endif

/*****************************************************************************
* Function Name: R_SCI_Send
* Description  : This function is used to start transmitting data. For Async,
*                the data is loaded into a queue and transmission started if
*                not already in progress. For SSPI/Sync, the channel is
*                checked to see if busy, and if not, transmission of the
*                source buffer is started.
*
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                p_src -
*                    ptr to data to transmit
*                length - 
*                    number of bytes to send
* Return Value : SCI_SUCCESS -
*                    requested number of bytes sent/loaded into tx queue
*                SCI_ERR_NULL_PTR -
*                    hdl or p_src is NULL
*                SCI_ERR_BAD_MODE -
*                    channel mode not currently supported
*                SCI_ERR_INSUFFICIENT_SPACE - 
*                    not enough space in tx queue to store data (Async)
*                SCI_ERR_XCVR_BUSY -
*                    channel currently busy
*                SCI_ERR_INVALID_ARG
*                    length is out of range
******************************************************************************/
sci_err_t R_SCI_Send(sci_hdl_t const    hdl,
                     uint8_t            *p_src,
                     uint16_t const     length)
{
    sci_err_t   err=SCI_SUCCESS;

    /* Check arguments */

#if SCI_CFG_PARAM_CHECKING_ENABLE
    if (((NULL == hdl)   || (FIT_NO_PTR == hdl))
     || ((NULL == p_src) || (FIT_NO_PTR == p_src)))
    {
        return SCI_ERR_NULL_PTR;
    }
    if ((SCI_MODE_OFF == hdl->mode) || (SCI_MODE_MAX <= hdl->mode))
    {
        return SCI_ERR_BAD_MODE;
    }
    if (0 == length)
    {
        return SCI_ERR_INVALID_ARG;
    }
#endif

    if (SCI_MODE_ASYNC == hdl->mode)
    {
#if (SCI_CFG_ASYNC_INCLUDED)
        err = sci_send_async_data(hdl, p_src, length);
#endif
    }
    else
    {   /* SSPI or SYNC */
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
        err = sci_send_sync_data(hdl, p_src, NULL, length, false);
#endif
    }

    return err;
} /* End of function R_SCI_Send() */


#if (SCI_CFG_ASYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_send_async_data
* Description  : This function determines if the tx byte queue of the channel
*                referenced by the handle is not full, and call the byte
*                transmission function.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                p_src -
*                    ptr to data to transmit
*                length -
*                    number of bytes to send and possibly receive
* Return Value : SCI_SUCCESS -
*                    data transfer started
*                SCI_ERR_XCVR_BUSY -
*                    channel currently busy
*                SCI_ERR_INSUFFICIENT_SPACE - 
*                    not enough space in tx queue to store data (Async)
******************************************************************************/
static sci_err_t sci_send_async_data(sci_hdl_t const hdl,
                                     uint8_t         *p_src,
                                     uint16_t const  length)
{
    sci_err_t   err=SCI_SUCCESS;
    uint16_t    cnt;
    byteq_err_t byteq_err=BYTEQ_ERR_QUEUE_FULL;

    if (hdl->tx_idle != true)
    {
        return SCI_ERR_XCVR_BUSY;
    }

#if SCI_CFG_FIFO_INCLUDED
    if (true == hdl->fifo_ctrl)
    {
        /* TX FIFO use check */
        if (0x00 < hdl->rom->regs->FDR.BIT.T)
        {
            return SCI_ERR_XCVR_BUSY;
        }

        /* reset TX FIFO */
        hdl->rom->regs->FCR.BIT.TFRST = 0x01;
    }
#endif

    /* Determine amount of space left in tx queue */
    R_BYTEQ_Unused(hdl->u_tx_data.que, &cnt);

    if (cnt < length)
    {
        /* If can't fit, return */
        return SCI_ERR_INSUFFICIENT_SPACE;
    }

    /* Else load bytes into tx queue for transmission */
    for (cnt=0; cnt < length; cnt++)
    {
        byteq_err = sci_put_byte(hdl, *p_src++);
        if (BYTEQ_SUCCESS != byteq_err)
        {
            /* If the return value is not BYTEQ_SUCCESS. */
            err = SCI_ERR_INSUFFICIENT_SPACE;
            break;
        }
    }

    if (SCI_SUCCESS == err)
    {
        hdl->tx_idle = false;
        ENABLE_TXI_INT;
    }

    return err;
} /* End of function sci_send_async_data() */

/*****************************************************************************
* Function Name: sci_put_byte
* Description  : Transmits byte if channel is not busy. Otherwise, byte is
*                stored in tx queue until can transmit. If buffer is full
*                and cannot store it, an error code is returned.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                byte -
*                    byte to transmit
* Return Value : SCI_SUCCESS -
*                    data transfer started
*                SCI_ERR_INSUFFICIENT_SPACE - 
*                    not enough space in tx queue to store data (Async)
******************************************************************************/
static byteq_err_t sci_put_byte(sci_hdl_t const   hdl,
                                uint8_t const     byte)
{
    byteq_err_t err = BYTEQ_ERR_QUEUE_FULL;

    /* else load next byte into tx queue (space checked in calling func) */
    err = R_BYTEQ_Put(hdl->u_tx_data.que, byte);

    return err;
} /* End of function sci_put_byte() */
#endif


#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_send_sync_data
* Description  : This function determines if the channel referenced by the
*                handle is not busy, and begins the data transfer process
*                (both sending and receiving data).
*
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                p_src -
*                    ptr to data to transmit
*                p_dst -
*                    ptr to buffer to store receive data (optional)
*                length -
*                    number of bytes to send and possibly receive
*                save_rx_data -
*                    true if data clocked in should be saved to p_dst.
* Return Value : SCI_SUCCESS -
*                    data transfer started
*                SCI_ERR_XCVR_BUSY -
*                    channel currently busy
******************************************************************************/
static sci_err_t sci_send_sync_data(sci_hdl_t const hdl,
                                    uint8_t         *p_src,
                                    uint8_t         *p_dst,
                                    uint16_t const  length,
                                    bool            save_rx_data)
{
#if SCI_CFG_FIFO_INCLUDED
    uint8_t cnt;
    uint8_t thresh_cnt;
#endif

    if (true == hdl->tx_idle)
    {
        if (true == save_rx_data)
        {
            hdl->u_rx_data.buf = p_dst;
        }
        hdl->save_rx_data  = save_rx_data;

        hdl->u_tx_data.buf = p_src;
        hdl->tx_cnt        = length;
        hdl->rx_cnt        = length;
        hdl->tx_idle       = false;
        hdl->tx_dummy      = false;

#if SCI_CFG_FIFO_INCLUDED
        if (true == hdl->fifo_ctrl)
        {
            /* reset TX FIFO */
            hdl->rom->regs->FCR.BIT.TFRST = 0x01;
            /* reset RX FIFO */
            hdl->rom->regs->FCR.BIT.RFRST = 0x01;

            /* If length is higher than SCI_CFG_CHXX_RX_FIFO_THRESH, FCR.BIT.RTRG register is set to length */
            if (length < hdl->rx_curr_thresh)
            {
                hdl->rom->regs->FCR.BIT.RTRG = length;
            }

            thresh_cnt = hdl->rom->regs->FCR.BIT.RTRG;
            
            hdl->tx_cnt -= thresh_cnt;

            /* Repeated FIFO RX threshold count */
            for (cnt=0; cnt<thresh_cnt; cnt++)
            {
                SCI_TDR(*hdl->u_tx_data.buf++);    /* start transmit */
            }
        }
        else
#endif
        {
            hdl->tx_cnt--;
            SCI_TDR(*hdl->u_tx_data.buf++);    /* start transmit */
        }

        return SCI_SUCCESS;
    }

    return SCI_ERR_XCVR_BUSY;
} /* End of function sci_send_sync_data() */
#endif


#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
/*****************************************************************************
* Function Name: R_SCI_SendReceive
* Description  : This function determines if the channel referenced by the
*                handle is not busy, and begins the data transfer process
*                (both sending and receiving data).
*
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                p_src -
*                    ptr to data to transmit
*                p_dst -
*                    ptr to buffer to store received data
*                length -
*                    number of bytes to send/receive
* Return Value : SCI_SUCCESS -
*                    data transfer started
*                SCI_ERR_NULL_PTR -
*                    hdl, p_src or p_dst is NULL
*                SCI_ERR_BAD_MODE -
*                    channel mode not currently supported
*                SCI_ERR_XCVR_BUSY -
*                    channel currently busy
*                SCI_ERR_INVALID_ARG
*                    length is out of range
******************************************************************************/
sci_err_t R_SCI_SendReceive(sci_hdl_t const hdl,
                            uint8_t         *p_src,
                            uint8_t         *p_dst,
                            uint16_t const  length)
{
sci_err_t   err;


    /* Check arguments */

#if SCI_CFG_PARAM_CHECKING_ENABLE
    if ((((NULL == hdl)   || (FIT_NO_PTR == hdl))
     ||  ((NULL == p_src) || (FIT_NO_PTR == p_src)))
     ||  ((NULL == p_dst) || (FIT_NO_PTR == p_dst)))
    {
        return SCI_ERR_NULL_PTR;
    }
    if ((hdl->mode != SCI_MODE_SSPI) && (hdl->mode != SCI_MODE_SYNC))
    {
        return SCI_ERR_BAD_MODE;
    }
    if (0 == length)
    {
        return SCI_ERR_INVALID_ARG;
    }
#endif

    err = sci_send_sync_data(hdl, p_src, p_dst, length, true);

    return err;
} /* End of function R_SCI_SendReceive() */
#endif

#if (SCI_CFG_ASYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_transfer
* Description  : Transfer for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void sci_transfer(sci_hdl_t const hdl)
{
    uint16_t    num;
    uint8_t     byte;

    /* Get bytes from tx queue */
    (void)R_BYTEQ_Get(hdl->u_tx_data.que, (uint8_t *)&byte);

    /* TDR/FTDR register write access */
    SCI_TDR(byte);

    /* Get data byte number from que and if the number of data bytes is 0, to disable the transfer */
    R_BYTEQ_Used(hdl->u_tx_data.que, &num);
    if (0 >= num)
    {
        /* Disable transmit interrupt */
        DISABLE_TXI_INT;
#if SCI_CFG_TEI_INCLUDED
        /* Enable transmit end interrupt */
        hdl->rom->regs->SCR.BIT.TEIE = 1;
        ENABLE_TEI_INT;
#endif
        hdl->tx_idle = true;    // set flag if queue empty
    }
} /* End of function sci_transfer() */

#if SCI_CFG_FIFO_INCLUDED
/*****************************************************************************
* Function Name: sci_fifo_transfer
* Description  : FIFO Transfer for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void sci_fifo_transfer(sci_hdl_t const hdl)
{
    uint8_t cnt;
    uint8_t fifo_num;

    /* Repeated empty FIFO buffer count */
    fifo_num = SCI_FIFO_FRAME_SIZE - hdl->rom->regs->FDR.BIT.T;

    for (cnt = 0; cnt < fifo_num; cnt++)
    {
        /* SCI Transfer */
        sci_transfer(hdl);

        /* If the queue is empty(true == hdl->tx_idle), exit from FIFO transfer loop */
        if (true == hdl->tx_idle)
        {
            break;
        }
    }

    /* When the settings of transmit data are completed, set the SSRFIFO.TDFE flag to 0. */
    if (1 == hdl->rom->regs->SSRFIFO.BIT.TDFE)
    {
        hdl->rom->regs->SSRFIFO.BYTE = (unsigned char)~SCI_SSRFIFO_TDFE_MASK;
    }
} /* End of function sci_fifo_transfer() */
#endif

/*****************************************************************************
* Function Name: txi_handler
* Description  : TXI interrupt handler for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
void txi_handler(sci_hdl_t const hdl)
{
#if SCI_CFG_FIFO_INCLUDED
    if (true == hdl->fifo_ctrl)
    {
        /* SCI FIFO Transfer */
        sci_fifo_transfer(hdl);
    }
    else
#endif
    {
        /* SCI Transfer */
        sci_transfer(hdl);
    }
} /* End of function txi_handler() */
#endif


#if SCI_CFG_TEI_INCLUDED
/*****************************************************************************
* Function Name: tei_handler
* Description  : TEI interrupt handler for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
void tei_handler(sci_hdl_t const hdl)
{
    sci_cb_args_t   args;

    /* Disable transmit end interrupt */
    DISABLE_TEI_INT;
    hdl->rom->regs->SCR.BIT.TEIE = 0;

    if ((NULL != hdl->callback) && (FIT_NO_FUNC != hdl->callback))
    {
        args.hdl = hdl;
        args.event = SCI_EVT_TEI;
        hdl->callback((void *)&args);
    }
} /* End of function tei_handler() */
#endif


/*****************************************************************************
* Function Name: R_SCI_Receive
* Description  : Gets data received on an SCI channel referenced by the handle 
*                from rx queue. Function does not block if the requested 
*                number of bytes is not available. If any errors occurred 
*                during reception by hardware, they are handled by the callback 
*                function specified in R_SCI_Open() and no corresponding error 
*                code is provided here.
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                p_dst -
*                    ptr to buffer to load data into
*                length - 
*                    number of bytes to read
* Return Value : SCI_SUCCESS -
*                    requested number of byte loaded into p_dst
*                SCI_ERR_NULL_PTR -
*                    hdl or p_dst is NULL
*                SCI_ERR_BAD_MODE -
*                    channel mode not currently supported
*                SCI_ERR_INSUFFICIENT_DATA -
*                    rx queue does not contain requested amount of data (Async)
*                SCI_ERR_XCVR_BUSY -
*                    channel currently busy (Sync)
*                SCI_ERR_INVALID_ARG
*                    length is out of range
******************************************************************************/
sci_err_t R_SCI_Receive(sci_hdl_t const hdl,
                        uint8_t         *p_dst,
                        uint16_t const  length)
{
sci_err_t   err=SCI_SUCCESS;


    /* Check arguments */

#if SCI_CFG_PARAM_CHECKING_ENABLE
    if (((NULL == hdl)   || (FIT_NO_PTR == hdl))
     || ((NULL == p_dst) || (FIT_NO_PTR == p_dst)))
    {
        return SCI_ERR_NULL_PTR;
    }
    if ((SCI_MODE_OFF == hdl->mode) || (SCI_MODE_MAX <= hdl->mode))
    {
        return SCI_ERR_BAD_MODE;
    }
    if (0 == length)
    {
        return SCI_ERR_INVALID_ARG;
    }
#endif

    if (SCI_MODE_ASYNC == hdl->mode)
    {
#if (SCI_CFG_ASYNC_INCLUDED)
        err = sci_receive_async_data(hdl, p_dst, length);
#endif
    }

    else
    {    /* mode is SSPI/SYNC */
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
        err = sci_receive_sync_data(hdl, p_dst, length);
#endif
    }

    return err;
} /* End of function R_SCI_Receive() */

#if (SCI_CFG_ASYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_receive_async_data
* Description  : This function determines if the rx byte queue of the channel 
*                referenced by the handle, the requested number of bytes 
*                is available, and get the data from the rx byte queue.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                p_dst -
*                    ptr to buffer to load data into
*                length - 
*                    number of bytes to read
* Return Value : SCI_SUCCESS -
*                    requested number of byte loaded into p_dst
*                SCI_ERR_INSUFFICIENT_DATA -
*                    rx queue does not contain requested amount of data
******************************************************************************/
static sci_err_t sci_receive_async_data(sci_hdl_t const hdl,
                                        uint8_t         *p_dst,
                                        uint16_t const  length)
{
    sci_err_t   err=SCI_SUCCESS;
    uint16_t    cnt;
    byteq_err_t byteq_err=BYTEQ_SUCCESS;

    /* CHECK FOR SUFFICIENT DATA IN QUEUE, AND FETCH IF AVAILABLE */
    R_BYTEQ_Used(hdl->u_rx_data.que, &cnt);

    if (cnt < length)
    {
        return SCI_ERR_INSUFFICIENT_DATA;
    }

    /* Get bytes from rx queue */
    for (cnt=0; cnt < length; cnt++)
    {
        DISABLE_RXI_INT;
        byteq_err = R_BYTEQ_Get(hdl->u_rx_data.que, p_dst++);
        ENABLE_RXI_INT;
        if (BYTEQ_SUCCESS != byteq_err)
        {
            err = SCI_ERR_INSUFFICIENT_DATA;
            break;
        }
    }

    return err;
} /* End of function sci_receive_async_data() */
#endif

#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_receive_sync_data
* Description  : This function determines if the channel referenced by the
*                handle is not busy, and dummy data send.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                p_dst -
*                    ptr to buffer to load data into
*                length - 
*                    number of bytes to read
* Return Value : SCI_SUCCESS -
*                    requested number of byte loaded into p_dst
*                SCI_ERR_XCVR_BUSY -
*                    channel currently busy
******************************************************************************/
static sci_err_t sci_receive_sync_data(sci_hdl_t const hdl,
                                       uint8_t         *p_dst,
                                       uint16_t const  length)
{
#if SCI_CFG_FIFO_INCLUDED
    uint8_t cnt;
    uint8_t thresh_cnt;
#endif

    /* IF TRANCEIVER NOT IN USE, START DUMMY TRANSMIT TO CLOCK IN DATA */
    if (true == hdl->tx_idle)
    {
        hdl->u_rx_data.buf = p_dst;
        hdl->save_rx_data  = true;               /* save the data clocked in */
        hdl->tx_idle       = false;
        hdl->tx_cnt        = length;
        hdl->rx_cnt        = length;
        hdl->tx_dummy      = true;

#if SCI_CFG_FIFO_INCLUDED
        if (true == hdl->fifo_ctrl)
        {
            /* reset TX FIFO */
            hdl->rom->regs->FCR.BIT.TFRST = 0x01;
            /* reset RX FIFO */
            hdl->rom->regs->FCR.BIT.RFRST = 0x01;

            if (length > SCI_FIFO_FRAME_SIZE)
            {
                thresh_cnt = SCI_FIFO_FRAME_SIZE;
            }
            else
            {
                /* If length is higher than SCI_CFG_CHXX_RX_FIFO_THRESH, FCR.BIT.RTRG register is set to length */
                if (length < hdl->rx_curr_thresh)
                {
                    hdl->rom->regs->FCR.BIT.RTRG = length;
                }
                thresh_cnt = length;
            }

            hdl->tx_cnt -= thresh_cnt;

            for (cnt=0; cnt<thresh_cnt; cnt++)
            {
                SCI_TDR(SCI_CFG_DUMMY_TX_BYTE);    /* start transmit */
            }
        }
        else
#endif
        {
            hdl->tx_cnt--;
            SCI_TDR(SCI_CFG_DUMMY_TX_BYTE);    /* start transfer */
        }

        return SCI_SUCCESS;
    }

    return SCI_ERR_XCVR_BUSY;
} /* End of function sci_receive_sync_data() */
#endif

/*****************************************************************************
* Function Name: sci_receive
* Description  : Receive for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void sci_receive(sci_hdl_t const hdl)
{
    sci_cb_args_t   args;
    uint8_t         byte;

    /* Read byte */
    SCI_RDR(byte);
    if (SCI_MODE_ASYNC == hdl->mode)
    {
#if (SCI_CFG_ASYNC_INCLUDED)

        /* Place byte in queue */
        if (BYTEQ_SUCCESS == R_BYTEQ_Put(hdl->u_rx_data.que, byte))
        {
            args.event = SCI_EVT_RX_CHAR;
        }
        else
        {
            args.event = SCI_EVT_RXBUF_OVFL;
        }

        /* Do callback if available */
        if ((NULL != hdl->callback) && (FIT_NO_FUNC != hdl->callback))
        {
            args.hdl = hdl;
            args.byte = byte;
            hdl->callback((void *)&args);
        }
#endif
    }
    else
    {
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
        hdl->rx_cnt--;

        /* Place byte in buffer if Receive() or SendReceive() */
        if (true == hdl->save_rx_data)
        {
            *hdl->u_rx_data.buf++ = byte;
        }

        /* See if more bytes to transfer */
        if (0 < hdl->rx_cnt)
        {
            if (0 < hdl->tx_cnt)
            {
                /* send another byte */
                if (true == hdl->tx_dummy)
                {
                    hdl->tx_cnt--;
                    SCI_TDR(SCI_CFG_DUMMY_TX_BYTE);
                }
                else
                {
                    hdl->tx_cnt--;
                    SCI_TDR(*hdl->u_tx_data.buf++);
                }
            }
        }
        else
        {
            hdl->tx_idle = true;
            /* Do callback if available */
            if ((NULL != hdl->callback) && (FIT_NO_FUNC != hdl->callback))
            {
                args.hdl = hdl;
                args.event = SCI_EVT_XFER_DONE;
                hdl->callback((void *)&args);
            }
        }
#endif
    }
} /* End of function sci_receive() */

#if SCI_CFG_FIFO_INCLUDED
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_fifo_receive_sync
* Description  : FIFO Receive for SCI mode is SYNC and SSPI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void sci_fifo_receive_sync(sci_hdl_t const hdl)
{
    uint8_t       cnt;
    uint8_t       fifo_num_rx;
    uint8_t       fifo_num_tx;
    sci_cb_args_t args;
    uint8_t       byte_rx[SCI_FIFO_FRAME_SIZE];

    fifo_num_rx = hdl->rom->regs->FDR.BIT.R;

    for (cnt=0; cnt<fifo_num_rx; cnt++)
    {
        SCI_RDR(byte_rx[cnt]);
    }

    hdl->rx_cnt -= fifo_num_rx;

    /* Place byte in buffer if Receive() or SendReceive() */
    if (true == hdl->save_rx_data)
    {
        for (cnt=0; cnt<fifo_num_rx; cnt++)
        {
            /* SCI Receive */
            *hdl->u_rx_data.buf++ = byte_rx[cnt];
        }
    }

    /* See if more bytes to transfer */
    if (0 < hdl->rx_cnt)
    {
        if (hdl->rom->regs->FCR.BIT.RTRG > hdl->rx_cnt)
        {
            hdl->rom->regs->FCR.BIT.RTRG = hdl->rx_cnt;
        }
        
        if (0 < hdl->tx_cnt)
        {
            if (hdl->tx_cnt > fifo_num_rx)
            {
                fifo_num_tx  = fifo_num_rx;
                hdl->tx_cnt -= fifo_num_rx;
            }
            else
            {
                fifo_num_tx  = hdl->tx_cnt;
                hdl->tx_cnt  = 0;
            }

            /* send another byte */
            if (true == hdl->tx_dummy)
            {
                for (cnt=0; cnt<fifo_num_tx; cnt++)
                {
                    SCI_TDR(SCI_CFG_DUMMY_TX_BYTE);
                }
            }
            else
            {
                for (cnt=0; cnt<fifo_num_tx; cnt++)
                {
                    SCI_TDR(*hdl->u_tx_data.buf++);
                }
            }
        }
    }
    else
    {
        hdl->rom->regs->FCR.BIT.RTRG = hdl->rx_curr_thresh;
        hdl->tx_idle = true;
        /* Do callback if available */
        if ((NULL != hdl->callback) && (FIT_NO_FUNC != hdl->callback))
        {
            args.hdl = hdl;
            args.event = SCI_EVT_XFER_DONE;
            hdl->callback((void *)&args);
        }
    }
} /* End of function sci_fifo_receive_sync() */
#endif
#endif

#if SCI_CFG_FIFO_INCLUDED
/*****************************************************************************
* Function Name: sci_fifo_receive
* Description  : FIFO Receive for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void sci_fifo_receive(sci_hdl_t const hdl)
{
#if (SCI_CFG_ASYNC_INCLUDED)
    uint16_t        cnt;
    uint16_t        fifo_num;
    sci_cb_args_t   args;
    uint8_t         byte_rx[SCI_FIFO_FRAME_SIZE];
#endif

    if (SCI_MODE_ASYNC == hdl->mode)
    {
#if (SCI_CFG_ASYNC_INCLUDED)
        fifo_num = (uint16_t)hdl->rom->regs->FDR.BIT.R;

        /* RX FIFO flush */
        for (cnt = 0; cnt < fifo_num; cnt++)
        {
            /* Read byte */
            SCI_RDR(byte_rx[cnt]);
        }

        /* Determine amount of space left in rx queue */
        (void)R_BYTEQ_Unused(hdl->u_rx_data.que, &cnt);
        if (cnt >= fifo_num)
        {
            /* free space is enough */
            args.event = SCI_EVT_RX_CHAR;
        }
        else
        {
            /* insufficient free space, store as much as possible */
            fifo_num = cnt;
            args.event = SCI_EVT_RXBUF_OVFL;
        }

        /* store bytes to rx queue for R_SCI_Receive */
        for (cnt = 0; cnt < fifo_num; cnt++)
        {
            (void)R_BYTEQ_Put(hdl->u_rx_data.que, byte_rx[cnt]);
        }

        /* Do callback if available */
        if ((NULL != hdl->callback) && (FIT_NO_FUNC != hdl->callback))
        {
            args.hdl = hdl;
            args.num = (uint8_t)fifo_num;   /* Number of bytes were stored to queue */
            hdl->callback((void *)&args);
        }
#endif
    }
    else
    {
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
        /* SCI Receive */
        sci_fifo_receive_sync(hdl);
#endif
    }

    /* When the readings of receive data are completed, set the SSRFIFO.RDF flag to 0. */
    if (1 == hdl->rom->regs->SSRFIFO.BIT.RDF)
    {
        hdl->rom->regs->SSRFIFO.BYTE = (unsigned char)~SCI_SSRFIFO_RDF_MASK;
    }

    if (SCI_MODE_ASYNC == hdl->mode)
    {
        if (1 == hdl->rom->regs->SSRFIFO.BIT.DR)
        {
            hdl->rom->regs->SSRFIFO.BYTE = (unsigned char)~SCI_SSRFIFO_DR_MASK;
        }
    }
} /* End of function sci_fifo_receive() */
#endif

/*****************************************************************************
* Function Name: rxi_handler
* Description  : RXI interrupt handler for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
void rxi_handler(sci_hdl_t const hdl,char chanel)
{
	uint16_t cnt;
	_uart_drv *pdrv;
	pdrv = &(uart_drv[chanel]);
#if SCI_CFG_FIFO_INCLUDED
    if (true == hdl->fifo_ctrl)
    {
        /* SCI FIFO Receive */
        sci_fifo_receive(hdl);
    }
    else
#endif
    {
        /* SCI Receive */
        sci_receive(hdl);
        R_BYTEQ_Used(hdl->u_rx_data.que, &cnt);
        if (cnt>0)
        {
        	aos_sem_signal( &pdrv->rx_semphr );
        }
    }
} /* End of function rxi_handler() */


/*****************************************************************************
* Function Name: sci_error
* Description  : Error for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void sci_error(sci_hdl_t const hdl)
{
    sci_cb_args_t   args;
    uint8_t         byte;
    uint8_t         reg;

    reg = SCI_SSR;
    if (0 != (reg & SCI_RCVR_ERR_MASK))
    {
        if (0 != (reg & SCI_SSR_ORER_MASK))
        {
            args.event = SCI_EVT_OVFL_ERR;
        }
#if (SCI_CFG_ASYNC_INCLUDED)
        else if (0 != (reg & SCI_SSR_PER_MASK))
        {
            args.event = SCI_EVT_PARITY_ERR;
        }
        else if (0 != (reg & SCI_SSR_FER_MASK))
        {
            args.event = SCI_EVT_FRAMING_ERR;
        }
#endif
        else
        {
            /* Do Nothing */
        }

        /* Flush register */
        SCI_RDR(byte);

        /* Clear error condition */
        while (0 != (SCI_SSR & SCI_RCVR_ERR_MASK))
        {
            SCI_RDR(byte);

            reg      = SCI_SSR;
            reg     &= (~SCI_RCVR_ERR_MASK);
            reg     |= SCI_SSR_CLR_MASK;
            SCI_SSR  = reg;

            if (0 != (SCI_SSR & SCI_RCVR_ERR_MASK))
            {
                nop(); /* read and Compare */
            }
        }

        /* Do callback for error */
        if ((NULL != hdl->callback) && (FIT_NO_FUNC != hdl->callback))
        {
            args.hdl = hdl;
            args.byte = byte;
            hdl->callback((void *)&args);
        }
    }

} /* End of function sci_error() */

#if SCI_CFG_FIFO_INCLUDED
/*****************************************************************************
* Function Name: sci_fifo_error
* Description  : FIFO Error for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void sci_fifo_error(sci_hdl_t const hdl)
{
    sci_cb_args_t       args;
    uint8_t             reg;
    volatile uint8_t    ssrfifo_data;
    volatile uint16_t   dummy;

    reg = SCI_SSRFIFO;
    if (0 != (reg & SCI_RCVR_ERR_MASK))
    {
        if (0 != (reg & SCI_SSR_ORER_MASK))
        {
            args.event = SCI_EVT_OVFL_ERR;
        }
#if (SCI_CFG_ASYNC_INCLUDED)
        else if (0 != (reg & SCI_SSR_PER_MASK))
        {
            args.event = SCI_EVT_PARITY_ERR;
        }
        else if (0 != (reg & SCI_SSR_FER_MASK))
        {
            args.event = SCI_EVT_FRAMING_ERR;
        }
#endif
        else
        {
            /* Do Nothing */
        }

        /* Do callback for error */
        if ((NULL != hdl->callback) && (FIT_NO_FUNC != hdl->callback))
        {
            args.hdl = hdl;
            args.byte = 0;
            hdl->callback((void *)&args);
        }

        /* if error condition don't clear in callback when it clear at here */
        reg = SCI_SSRFIFO;
        if (0 != (reg & SCI_RCVR_ERR_MASK))
        {
            /* Flush register */
            while (0 != hdl->rom->regs->FDR.BIT.R)
            {
                dummy = hdl->rom->regs->FRDR.WORD;              /* FRDR dummy read */
            }

            /* Clear error condition */
            while (0x00 != (SCI_SSRFIFO & SCI_RCVR_ERR_MASK))   /* Check PER, FER, ORER flags */
            {
                ssrfifo_data = SCI_SSRFIFO;                     /* SSRFIFO dummy read */
                SCI_SSRFIFO = (uint8_t)~SCI_RCVR_ERR_MASK;      /* PER, FER, ORER clear */
                if (0x00 != (SCI_SSRFIFO & SCI_RCVR_ERR_MASK))
                {
                    nop();                                      /* read and Compare */
                }
            }
        }
    }

    return;
} /* End of function sci_fifo_error() */
#endif

/*****************************************************************************
* Function Name: eri_handler
* Description  : ERI interrupt handler for SCI UART mode
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
void eri_handler(sci_hdl_t const hdl)
{
#if SCI_CFG_FIFO_INCLUDED
    if (true == hdl->fifo_ctrl)
    {
        /* SCI FIFO Error */
        sci_fifo_error(hdl);
    }
    else
#endif
    {
        /* SCI error */
        sci_error(hdl);
    }
} /* End of function eri_handler() */

/*****************************************************************************
* Function Name: R_SCI_Control
* Description  : This function configures non-standard UART hardware and
*                performs special software operations.
*
* WARNING: Some commands require the transmitter and receiver to be temporarily
*          disabled for the command to execute!
*          PFS and port pins must be configured prior to calling with an
*          SCI_EN_CTS_IN command.
*
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                cmd -
*                    command to run
*                p_args -
*                    pointer argument(s) specific to command
* Return Value : SCI_SUCCESS -
*                    Command completed successfully.
*                SCI_ERR_NULL_PTR -
*                    hdl or p_args is NULL
*                SCI_ERR_BAD_MODE -
*                    channel mode not currently supported
*                SCI_ERR_INVALID_ARG -
*                    The cmd value or p_args contains an invalid value.
*                    May be due to mode channel is operating in.
******************************************************************************/
sci_err_t R_SCI_Control(sci_hdl_t const     hdl,
                        sci_cmd_t const     cmd,
                        void                *p_args)
{
    sci_err_t   err=SCI_SUCCESS;
    sci_baud_t  *baud;
    int32_t     bit_err;


#if SCI_CFG_PARAM_CHECKING_ENABLE
    if ((NULL == hdl) || (FIT_NO_PTR == hdl))
    {
        return SCI_ERR_NULL_PTR;
    }
    if ((NULL == p_args) || (FIT_NO_PTR == p_args))
    {
        if (SCI_CMD_CHANGE_BAUD == cmd)
        {
            return SCI_ERR_NULL_PTR;
        }
#if SCI_CFG_FIFO_INCLUDED
        if ((SCI_CMD_CHANGE_TX_FIFO_THRESH == cmd) || (SCI_CMD_CHANGE_RX_FIFO_THRESH == cmd))
        {
            return SCI_ERR_NULL_PTR;
        }
#endif
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX65N)
        if ((SCI_CMD_SET_TXI_PRIORITY == cmd) || (SCI_CMD_SET_RXI_PRIORITY == cmd))
        {
            return SCI_ERR_NULL_PTR;
        }
#endif
    }
    if ((SCI_MODE_OFF == hdl->mode) || (SCI_MODE_MAX <= hdl->mode))
    {
        return SCI_ERR_BAD_MODE;
    }
#if SCI_CFG_FIFO_INCLUDED
    if (SCI_CMD_CHANGE_TX_FIFO_THRESH == cmd)
    {
        if (15 < (*(uint8_t *)p_args))
        {
            return SCI_ERR_INVALID_ARG;
        }
    }
    if (SCI_CMD_CHANGE_RX_FIFO_THRESH == cmd)
    {
        if ((1 > (*(uint8_t *)p_args)) || (15 < (*(uint8_t *)p_args)))
        {
            return SCI_ERR_INVALID_ARG;
        }
    }
#endif
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX65N)
    if ((SCI_CMD_SET_TXI_PRIORITY == cmd) || (SCI_CMD_SET_RXI_PRIORITY == cmd))
    {
        if ((1 > (*(uint8_t *)p_args)) || (BSP_MCU_IPL_MAX < (*(uint8_t *)p_args)))
        {
            return SCI_ERR_INVALID_ARG;
        }
    }
#endif
#endif
    
    /* COMMANDS COMMON TO ALL MODES */

    switch (cmd)
    {
    case (SCI_CMD_CHANGE_BAUD):
        baud = (sci_baud_t *)p_args;
#if (SCI_CFG_ASYNC_INCLUDED)
        hdl->pclk_speed = baud->pclk;           // save for break generation
#endif
        hdl->rom->regs->SCR.BYTE &= (~SCI_EN_XCVR_MASK);
        SCI_SCR_DUMMY_READ;
        bit_err = sci_init_bit_rate(hdl, baud->pclk, baud->rate);
        SCI_IR_TXI_CLEAR;
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
        if (1000 == bit_err)
        {
            err = SCI_ERR_INVALID_ARG;      // impossible baud rate; 100% error
        }
        else
        {
            hdl->baud_rate = baud->rate;    // save for break generation
        }
    break;

    case (SCI_CMD_EN_CTS_IN):
        if (hdl->mode != SCI_MODE_SSPI)
        {
            /* PFS & port pins must be configured for CTS prior to calling this */
            hdl->rom->regs->SCR.BYTE &= (~SCI_EN_XCVR_MASK);
            SCI_SCR_DUMMY_READ;
            hdl->rom->regs->SPMR.BIT.CTSE = 1;      // enable CTS input
            SCI_IR_TXI_CLEAR;
            hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
        }
        else
        {
            /* Can not use CTS in smart card interface mode, simple SPI mode, and simple I2C mode */
            err = SCI_ERR_INVALID_ARG;
        }
    break;

#if SCI_CFG_FIFO_INCLUDED
    case (SCI_CMD_CHANGE_TX_FIFO_THRESH):
        if (true == hdl->fifo_ctrl)
        {
            /* save current TX FIFO threshold */
            hdl->tx_curr_thresh = *((uint8_t *)p_args);

            /* change TX FIFO threshold */
            hdl->rom->regs->SCR.BYTE &= ~SCI_EN_XCVR_MASK;
            SCI_SCR_DUMMY_READ;
            hdl->rom->regs->FCR.BIT.TTRG = *((uint8_t *)p_args);
            SCI_IR_TXI_CLEAR;
            hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
        }
        else
        {
            err = SCI_ERR_INVALID_ARG;
        }
    break;

    case (SCI_CMD_CHANGE_RX_FIFO_THRESH):
        if (true == hdl->fifo_ctrl)
        {
            /* save current RX FIFO threshold */
            hdl->rx_curr_thresh = *((uint8_t *)p_args);

            /* change RX FIFO threshold */
            hdl->rom->regs->SCR.BYTE &= ~SCI_EN_XCVR_MASK;
            SCI_SCR_DUMMY_READ;
            hdl->rom->regs->FCR.BIT.RTRG = *((uint8_t *)p_args);
            SCI_IR_TXI_CLEAR;
            hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
        }
        else
        {
            err = SCI_ERR_INVALID_ARG;
        }
    break;
#endif

#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX65N)
    case (SCI_CMD_SET_TXI_PRIORITY):
        *hdl->rom->ipr_txi = *((uint8_t *)p_args);
    break;

    case (SCI_CMD_SET_RXI_PRIORITY):
        *hdl->rom->ipr_rxi = *((uint8_t *)p_args);
    break;
#endif

    default:
        /* ASYNC-SPECIFIC COMMANDS */
        if (SCI_MODE_ASYNC == hdl->mode)
        {
#if (SCI_CFG_ASYNC_INCLUDED)
            err = sci_async_cmds(hdl, cmd, p_args);
#endif
        }

        /* SSPI/SYNC-SPECIFIC COMMANDS */
        else
        {
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
            err = sci_sync_cmds(hdl, cmd, p_args);
#endif
        }
    break;
    }

    return err;
} /* End of function R_SCI_Control() */

/*****************************************************************************
* Function Name: R_SCI_Close
* Description  : Disables the SCI channel designated by the handle.
*
* WARNING: This will abort any xmt or rcv messages in progress.
* NOTE:    This does not disable the GROUP12 (rcvr err) interrupts.
*
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : SCI_SUCCESS -
*                    channel closed
*                SCI_ERR_NULL_PTR -
*                    hdl was NULL
******************************************************************************/
sci_err_t R_SCI_Close(sci_hdl_t const hdl)
{

#if SCI_CFG_PARAM_CHECKING_ENABLE
    if ((NULL == hdl) || (FIT_NO_PTR == hdl))
    {
        return SCI_ERR_NULL_PTR;
    }
#endif

    /* disable ICU interrupts */
    sci_disable_ints(hdl);

    /* free tx and rx queues */
#if (SCI_CFG_ASYNC_INCLUDED)
    if (SCI_MODE_ASYNC == hdl->mode)
    {
        R_BYTEQ_Close(hdl->u_tx_data.que);
        R_BYTEQ_Close(hdl->u_rx_data.que);
    }
#endif
#if SCI_CFG_FIFO_INCLUDED
    if (true == hdl->fifo_ctrl)
    {
        /* reset FIFO threshold */
        hdl->rx_curr_thresh = hdl->rx_dflt_thresh;
        hdl->tx_curr_thresh = hdl->tx_dflt_thresh;
    }
#endif

    /* mark the channel as not in use and power down */
    hdl->mode = SCI_MODE_OFF;
    power_off(hdl);

    return SCI_SUCCESS;
} /* End of function R_SCI_Close() */


/*****************************************************************************
* Function Name: R_SCI_GetVersion
* Description  : Returns the version of this module. The version number is 
*                encoded such that the top two bytes are the major version
*                number and the bottom two bytes are the minor version number.
* Arguments    : none
* Return Value : version number
******************************************************************************/
#pragma inline(R_SCI_GetVersion)
uint32_t  R_SCI_GetVersion(void)
{
uint32_t const version = (SCI_VERSION_MAJOR << 16) | SCI_VERSION_MINOR;

    return version;
} /* End of function R_SCI_GetVersion() */
