/**
 * \file
 * \brief low-level HAL - methods used to setup indirection to physical layer interface.
 * this level does the dirty work of abstracting the higher level ATCAIFace methods from the
 * low-level physical interfaces.  Its main goal is to keep low-level details from bleeding into
 * the logical interface implemetation.
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


/* when incorporating ATCA HAL into your application, you need to adjust the #defines in atca_hal.h to include
 * and exclude appropriate interfaces - this optimizes memory use when not using a specific iface implementation in your application */

#include "cryptoauthlib.h"
#include "atca_hal.h"

/** \brief Standard HAL API for ATCA to initialize a physical interface
 * \param[in] cfg pointer to ATCAIfaceCfg object
 * \param[in] hal pointer to ATCAHAL_t intermediate data structure
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_iface_init(ATCAIfaceCfg *cfg, ATCAHAL_t *hal)
{
    // Because C isn't a real object oriented language or dynamically typed, some switch in the overall system is unavoidable
    // The key here is to provide the flexibility to include just the types of interfaces you want/need without breaking the
    // object model.  The former is needed so in an embedded, constrained memory environment, you don't have to pay the price
    // (in terms of memory) for interfaces you don't use in your application.
    ATCA_STATUS status = ATCA_COMM_FAIL;

    switch (cfg->iface_type)
    {
    case ATCA_I2C_IFACE:
        #ifdef ATCA_HAL_I2C
        hal->halinit = &hal_i2c_init;
        hal->halpostinit = &hal_i2c_post_init;
        hal->halreceive = &hal_i2c_receive;
        hal->halsend = &hal_i2c_send;
        hal->halsleep = &hal_i2c_sleep;
        hal->halwake = &hal_i2c_wake;
        hal->halidle = &hal_i2c_idle;
        hal->halrelease = &hal_i2c_release;
        hal->hal_data = NULL;

        status = ATCA_SUCCESS;
        #endif
        break;
    case ATCA_SWI_IFACE:
        #ifdef ATCA_HAL_SWI
        hal->halinit = &hal_swi_init;
        hal->halpostinit = &hal_swi_post_init;
        hal->halreceive = &hal_swi_receive;
        hal->halsend = &hal_swi_send;
        hal->halsleep = &hal_swi_sleep;
        hal->halwake = &hal_swi_wake;
        hal->halidle = &hal_swi_idle;
        hal->halrelease = &hal_swi_release;
        hal->hal_data = NULL;

        status = ATCA_SUCCESS;
        #endif
        break;
    case ATCA_UART_IFACE:
        #ifdef ATCA_HAL_UART
        // TODO - initialize UART iface
        #endif
        #ifdef ATCA_HAL_KIT_CDC
        hal->halinit = &hal_kit_cdc_init;
        hal->halpostinit = &hal_kit_cdc_post_init;
        hal->halreceive = &hal_kit_cdc_receive;
        hal->halsend = &hal_kit_cdc_send;
        hal->halsleep = &hal_kit_cdc_sleep;
        hal->halwake = &hal_kit_cdc_wake;
        hal->halidle = &hal_kit_cdc_idle;
        hal->halrelease = &hal_kit_cdc_release;
        hal->hal_data = NULL;

        status = ATCA_SUCCESS;
        #endif
        break;
    case ATCA_SPI_IFACE:
        #ifdef ATCA_HAL_SPI
        // TODO - initialize SPI iface
        #endif
        break;
    case ATCA_HID_IFACE:
        #ifdef ATCA_HAL_KIT_HID
        hal->halinit = &hal_kit_hid_init;
        hal->halpostinit = &hal_kit_hid_post_init;
        hal->halreceive = &hal_kit_hid_receive;
        hal->halsend = &hal_kit_hid_send;
        hal->halsleep = &hal_kit_hid_sleep;
        hal->halwake = &hal_kit_hid_wake;
        hal->halidle = &hal_kit_hid_idle;
        hal->halrelease = &hal_kit_hid_release;
        hal->hal_data = NULL;

        status = ATCA_SUCCESS;
        #endif
        break;
    case ATCA_CUSTOM_IFACE:
        #ifdef ATCA_HAL_CUSTOM
        hal->halinit = cfg->atcacustom.halinit;
        hal->halpostinit = cfg->atcacustom.halpostinit;
        hal->halreceive = cfg->atcacustom.halreceive;
        hal->halsend = cfg->atcacustom.halsend;
        hal->halsleep = cfg->atcacustom.halsleep;
        hal->halwake = cfg->atcacustom.halwake;
        hal->halidle = cfg->atcacustom.halidle;
        hal->halrelease = cfg->atcacustom.halrelease;
        hal->hal_data = NULL;

        status = ATCA_SUCCESS;
        #endif
        break;
    default:
        break;
    }
    return status;
}

/** \brief releases a physical interface, HAL knows how to interpret hal_data
 * \param[in] iface_type - the type of physical interface to release
 * \param[in] hal_data - pointer to opaque hal data maintained by HAL implementation for this interface type
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_iface_release(ATCAIfaceType iface_type, void *hal_data)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;

    switch (iface_type)
    {
    case ATCA_I2C_IFACE:
#ifdef ATCA_HAL_I2C
        status = hal_i2c_release(hal_data);
#endif
        break;
    case ATCA_SWI_IFACE:
 #ifdef ATCA_HAL_SWI
        status = hal_swi_release(hal_data);
#endif
        break;
    case ATCA_UART_IFACE:
 #ifdef ATCA_HAL_UART
        // TODO - release HAL UART
#endif
#ifdef ATCA_HAL_KIT_CDC
        status = hal_kit_cdc_release(hal_data);
#endif
        break;
    case ATCA_SPI_IFACE:
#ifdef ATCA_HAL_SPI
        // TODO - release HAL SPI
#endif
        break;
    case ATCA_HID_IFACE:
#ifdef ATCA_HAL_KIT_HID
        status = hal_kit_hid_release(hal_data);
#endif
        break;
    case ATCA_CUSTOM_IFACE:
#ifdef ATCA_HAL_CUSTOM
        // Current architecture/API prevents us from accessing the custom
        // release function. So we just assume success at this point.
        status = ATCA_SUCCESS;
#endif
        break;
    default:
        break;
    }

    return status;
}

/** \brief Utility function for hal_wake to check the reply.
 * \param[in] response       Wake response to be checked.
 * \param[in] response_size  Size of the response to check.
 * \return ATCA_SUCCESS for expected wake, ATCA_STATUS_SELFTEST_ERROR if the
 *         power on self test failed, ATCA_WAKE_FAILED for other failures.
 */
ATCA_STATUS hal_check_wake(const uint8_t* response, int response_size)
{
    const uint8_t expected_response[4] = { 0x04, 0x11, 0x33, 0x43 };
    uint8_t selftest_fail_resp[4] = { 0x04, 0x07, 0xC4, 0x40 };

    if (response_size != 4)
    {
        return ATCA_WAKE_FAILED;
    }
    if (memcmp(response, expected_response, 4) == 0)
    {
        return ATCA_SUCCESS;
    }
    if (memcmp(response, selftest_fail_resp, 4) == 0)
    {
        return ATCA_STATUS_SELFTEST_ERROR;
    }
    return ATCA_WAKE_FAILED;
}