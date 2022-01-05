/**
 * \file
 *
 * \brief  Microchip CryptoAuthLib hardware interface object
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

#include <stdlib.h>
#include "atca_iface.h"
#include "hal/atca_hal.h"

/** \defgroup interface ATCAIface (atca_)
 *  \brief Abstract interface to all CryptoAuth device types.  This interface
 *  connects to the HAL implementation and abstracts the physical details of the
 *  device communication from all the upper layers of CryptoAuthLib
   @{ */


#ifndef ATCA_POST_DELAY_MSEC
/* \brief How long to wait after an initial wake failure for the POSt to
 *         complete.
 * If Power-on self test (POST) is enabled, the self test will run on waking
 * from sleep or during power-on, which delays the wake reply.
 */
#define ATCA_POST_DELAY_MSEC 25
#endif

ATCA_STATUS _atinit(ATCAIface ca_iface, ATCAHAL_t *hal);

/** \brief Initializer for ATCAIface objects
 * \param[in] cfg       Logical configuration for the interface
 * \param[in] ca_iface  Interface structure to initialize.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS initATCAIface(ATCAIfaceCfg *cfg, ATCAIface ca_iface)
{
    ATCA_STATUS status;

    if (cfg == NULL || ca_iface == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    ca_iface->mType = cfg->iface_type;
    ca_iface->mIfaceCFG = cfg;

    status = atinit(ca_iface);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    return ATCA_SUCCESS;
}

#ifndef ATCA_NO_HEAP
/** \brief Constructor for ATCAIface objects
 * \param[in] cfg  Logical configuration for the interface
 * \return New interface instance on success. NULL on failure.
 */
ATCAIface newATCAIface(ATCAIfaceCfg *cfg)
{
    ATCAIface ca_iface;
    ATCA_STATUS status;

    ca_iface = (ATCAIface)malloc(sizeof(struct atca_iface));
    status = initATCAIface(cfg, ca_iface);
    if (status != ATCA_SUCCESS)
    {
        free(ca_iface);
        ca_iface = NULL;
        return NULL;
    }

    return ca_iface;
}
#endif

/** \brief Performs the HAL initialization by calling intermediate HAL wrapper
 *         function. If using the basic API, the atcab_init() function should
 *         be called instead.
 * \param[in] ca_iface  Device to interact with.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atinit(ATCAIface ca_iface)
{
    ATCA_STATUS status = ATCA_COMM_FAIL;
    ATCAHAL_t hal;

    _atinit(ca_iface, &hal);

    status = ca_iface->atinit(&hal, ca_iface->mIfaceCFG);
    if (status == ATCA_SUCCESS)
    {
        ca_iface->hal_data = hal.hal_data;

        // Perform the post init
        status = ca_iface->atpostinit(ca_iface);
    }

    return status;
}

/** \brief Sends the data to the device by calling intermediate HAL wrapper
 *         function.
 * \param[in] ca_iface  Device to interact with.
 * \param[in] txdata    Data to be transmitted to the device.
 * \param[in] txlength  Number of bytes to be transmitted to the device.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atsend(ATCAIface ca_iface, uint8_t *txdata, int txlength)
{
    return ca_iface->atsend(ca_iface, txdata, txlength);
}

/**\brief Receives data from the device by calling intermediate HAL wrapper
 *        function.
 * \param[in]    ca_iface  Device to interact with.
 * \param[out]   rxdata    Data received will be returned here.
 * \param[inout] rxlength  As input, the size of the rxdata buffer.
 *                         As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atreceive(ATCAIface ca_iface, uint8_t *rxdata, uint16_t *rxlength)
{
    return ca_iface->atreceive(ca_iface, rxdata, rxlength);
}

/** \brief Wakes up the device by calling intermediate HAL wrapper function.
 *         If using the basic API, the atcab_wakeup() function should be used
 *         instead.
 * \param[in] ca_iface  Device to interact with.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atwake(ATCAIface ca_iface)
{
    ATCA_STATUS status = ca_iface->atwake(ca_iface);

    if (status == ATCA_WAKE_FAILED)
    {
        // The device might be performing a POST. Wait for it to complete
        // and try again.
        atca_delay_ms(ATCA_POST_DELAY_MSEC);

        status = ca_iface->atwake(ca_iface);
    }

    return status;
}


/** \brief Puts the device into idle state by calling intermediate HAL wrapper
 *         function. If using the basic API, the atcab_idle() function should
 *         be used instead.
 * \param[in] ca_iface  Device to interact with.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atidle(ATCAIface ca_iface)
{
    ATCA_STATUS status;

    status = ca_iface->atidle(ca_iface);
    atca_delay_ms(1);
    return status;
}

/** \brief Puts the device into sleep state by calling intermediate HAL wrapper
 *         function. If using the basic API, the atcab_sleep() function should
 *         be used instead.
 * \param[in] ca_iface  Device to interact with.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atsleep(ATCAIface ca_iface)
{
    ATCA_STATUS status;

    status = ca_iface->atsleep(ca_iface);
    atca_delay_ms(1);
    return status;
}


/** \brief Returns the logical interface configuration for the device.
 * \param[in] ca_iface  Device interface.
 * \return Logical interface configuration.
 */
ATCAIfaceCfg * atgetifacecfg(ATCAIface ca_iface)
{
    return ca_iface->mIfaceCFG;
}


/** \brief Returns the HAL data pointer for the device.
 * \param[in] ca_iface  Device interface.
 * \return HAL data pointer.
 */
void* atgetifacehaldat(ATCAIface ca_iface)
{
    return ca_iface->hal_data;
}

/** \brief Instruct the HAL driver to release any resources associated with
 *         this interface.
 *  \param[in] ca_iface  Device interface.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS releaseATCAIface(ATCAIface ca_iface)
{
    ATCA_STATUS ret = ATCA_BAD_PARAM;

    if (ca_iface)
    {
        ret = hal_iface_release(ca_iface->mType, ca_iface->hal_data);
        ca_iface->hal_data = NULL;
    }

    return ret;
}

#ifndef ATCA_NO_HEAP
/** \brief Instruct the HAL driver to release any resources associated with
 *         this interface, then delete the object.
 * \param[in] ca_iface  Device interface.
 */
void deleteATCAIface(ATCAIface *ca_iface)
{
    if (ca_iface == NULL)
    {
        return;
    }

    releaseATCAIface(*ca_iface);
    free(*ca_iface);
    *ca_iface = NULL;
}
#endif

ATCA_STATUS _atinit(ATCAIface ca_iface, ATCAHAL_t *hal)
{
    // get method mapping to HAL methods for this interface
    hal_iface_init(ca_iface->mIfaceCFG, hal);
    ca_iface->atinit     = hal->halinit;
    ca_iface->atpostinit = hal->halpostinit;
    ca_iface->atsend     = hal->halsend;
    ca_iface->atreceive  = hal->halreceive;
    ca_iface->atwake     = hal->halwake;
    ca_iface->atsleep    = hal->halsleep;
    ca_iface->atidle     = hal->halidle;
    ca_iface->hal_data   = hal->hal_data;

    return ATCA_SUCCESS;
}
/** @} */
