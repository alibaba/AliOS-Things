/**
 * \file
 * \brief ATCA Hardware abstraction layer for SWI bit banging.
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include <asf.h>
#include <string.h>
#include <stdio.h>
#include "atca_hal.h"
#include "hal_swi_bitbang.h"
#include "atca_device.h"

/**
 * \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief These methods define the hardware abstraction layer for
 *        communicating with a CryptoAuth device using SWI bit banging.
   @{ */

/**
 * \brief Logical to physical bus mapping structure.
 */
static ATCASWIMaster_t swi_hal_data[MAX_SWI_BUSES];   //!< map logical, 0-based bus number to index

/** \brief discover swi buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application.This function is currently not supported.
 * of the a-priori knowledge
 * \param[in] swi_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * \return ATCA_UNIMPLEMENTED
 */
ATCA_STATUS hal_swi_discover_buses(int swi_buses[], int max_buses)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief discover any CryptoAuth devices on a given logical bus number.This function is curently not supported.
 * \param[in] bus_num - logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg[] - pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] *found - number of devices found on this bus
 * \return ATCA_UNIMPLEMENTED
 */
ATCA_STATUS hal_swi_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int *found)
{
    return ATCA_UNIMPLEMENTED;
}

/**
 * \brief hal_swi_init manages requests to initialize a physical
 *        interface. It manages use counts so when an interface has
 *        released the physical layer, it will disable the interface for
 *        some other use. You can have multiple ATCAIFace instances using
 *        the same bus, and you can have multiple ATCAIFace instances on
 *        multiple swi buses, so hal_swi_init manages these things and
 *        ATCAIFace is abstracted from the physical details.
 */

/**
 * \brief Initialize an SWI interface using given config.
 *
 * \param[in] hal  opaque pointer to HAL data
 * \param[in] cfg  interface configuration
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_swi_init(void *hal, ATCAIfaceCfg *cfg)
{
    if (cfg->atcaswi.bus >= MAX_SWI_BUSES)
    {
        return ATCA_COMM_FAIL;
    }
    ATCASWIMaster_t* data = &swi_hal_data[cfg->atcaswi.bus];

    if (data->ref_ct <= 0)
    {
        // Bus isn't being used, enable it

        //! assign GPIO pin
        data->pin_sda = swi_buses_default.pin_sda[cfg->atcaswi.bus];
        swi_set_pin(data->pin_sda);
        swi_enable();

        // store this for use during the release phase
        data->bus_index = cfg->atcaswi.bus;
        // buses are shared, this is the first instance
        data->ref_ct = 1;
    }
    else
    {
        // Bus is already is use, increment reference counter
        data->ref_ct++;
    }

    ((ATCAHAL_t*)hal)->hal_data = data;

    return ATCA_SUCCESS;
}

/**
 * \brief HAL implementation of SWI post init.
 *
 * \param[in] iface  ATCAIface instance
 *
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_swi_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

/**
 * \brief Send byte(s) via SWI.
 *
 * \param[in] iface     interface of the logical device to send data to
 * \param[in] txdata    pointer to bytes to send
 * \param[in] txlength  number of bytes to send
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_swi_send(ATCAIface iface, uint8_t *txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int bus     = cfg->atcaswi.bus;

    //! Skip the Word Address data as SWI doesn't use it
    txdata++;

    //! Set SWI pin
    swi_set_pin(swi_hal_data[bus].pin_sda);
    //! Send Command Flag
    swi_send_byte(SWI_FLAG_CMD);
    //! Send the remaining bytes
    swi_send_bytes(txlength, txdata);

    return ATCA_SUCCESS;
}

/**
 * \brief Receive byte(s) via SWI.
 * \param[in]    iface     Device to interact with.
 * \param[out]   rxdata    Data received will be returned here.
 * \param[inout] rxlength  As input, the size of the rxdata buffer.
 *                         As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_swi_receive(ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    ATCA_STATUS status = !ATCA_SUCCESS;
    int bus     = cfg->atcaswi.bus;
    int retries = cfg->rx_retries;
    uint16_t rxdata_max_size = *rxlength;

    *rxlength = 0;
    if (rxdata_max_size < 1)
    {
        return ATCA_SMALL_BUFFER;
    }

    //! Set SWI pin
    swi_set_pin(swi_hal_data[bus].pin_sda);

    while (retries-- > 0 && status != ATCA_SUCCESS)
    {
        swi_send_byte(SWI_FLAG_TX);
        status = swi_receive_bytes(rxdata_max_size, rxdata);
    }

    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    *rxlength = rxdata[0];

    return ATCA_SUCCESS;
}

/**
 * \brief Send Wake flag via SWI.
 *
 * \param[in] iface  interface of the logical device to wake up
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_swi_wake(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    ATCA_STATUS status = ATCA_WAKE_FAILED;
    int bus     = cfg->atcaswi.bus;
    uint8_t data[4] = { 0x00, 0x00, 0x00, 0x00 };
    uint16_t rxlength  = sizeof(data);

    //! Set SWI pin
    swi_set_pin(swi_hal_data[bus].pin_sda);
    //! Generate Wake Token
    swi_send_wake_token();

    //! Wait tWHI + tWLO
    atca_delay_us(cfg->wake_delay);

    status = hal_swi_receive(iface, data, &rxlength);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    return hal_check_wake(data, 4);
}

/**
 * \brief Send Idle flag via SWI.
 *
 * \param[in] iface  interface of the logical device to idle
 *
 * \return ATCA_SUCCES
 */
ATCA_STATUS hal_swi_idle(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int bus     = cfg->atcaswi.bus;

    //! Set SWI pin
    swi_set_pin(swi_hal_data[bus].pin_sda);
    swi_send_byte(SWI_FLAG_IDLE);

    return ATCA_SUCCESS;
}

/**
 * \brief Send Sleep flag via SWI.
 *
 * \param[in] iface  interface of the logical device to sleep
 *
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_swi_sleep(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int bus     = cfg->atcaswi.bus;

    //! Set SWI pin
    swi_set_pin(swi_hal_data[bus].pin_sda);
    swi_send_byte(SWI_FLAG_SLEEP);

    return ATCA_SUCCESS;
}

/**
 * \brief Manages reference count on given bus and releases resource if
 *        no more reference(s) exist.
 *
 * \param[in] hal_data  opaque pointer to hal data structure - known only
 *                      to the HAL implementation
 *
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_swi_release(void *hal_data)
{
    ATCASWIMaster_t *hal = (ATCASWIMaster_t*)hal_data;

    //! if the use count for this bus has gone to 0 references, disable it.  protect against an unbracketed release
    if (hal && --(hal->ref_ct) <= 0)
    {
        swi_set_pin(swi_hal_data[hal->bus_index].pin_sda);
        swi_disable();
        hal->ref_ct = 0;
    }

    return ATCA_SUCCESS;
}

/** @} */