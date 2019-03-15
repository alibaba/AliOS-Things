/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
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

#include "objects.h"
#include "nu_modutil.h"
#include "nu_bitutil.h"
#include "hal_status.h"
#include <k_api.h>

#include "aos/hal/dac.h"
#include "board.h"

static const struct nu_modinit_s dac_modinit_tab[] = {
    {DAC_0, DAC_MODULE, 0, 0, DAC_RST, DAC_IRQn, NULL},
    {DAC_1, DAC_MODULE, 0, 0, DAC_RST, DAC_IRQn, NULL},
};

static uint32_t dac_modinit_mask = 0;

static int32_t platform_analogout_init(struct analogout_s *obj, PinName pin )
{
    struct nu_modinit_s *modinit;
    
    if ( obj->dac != (DACName) pinmap_peripheral(pin, PinMap_DAC))
        goto exit_platform_analogout_init;

    if ( (modinit = get_modinit(obj->dac, dac_modinit_tab)) == NULL )
        goto exit_platform_analogout_init;

    if ( modinit->modname != (int) obj->dac )
        goto exit_platform_analogout_init;

    DAC_T *dac_base = (DAC_T *) NU_MODBASE(obj->dac);

    if (! dac_modinit_mask) {
        // Reset this module if no channel enabled
        SYS_ResetModule(modinit->rsetidx);

        // Select clock source of paired channels
        CLK_SetModuleClock(modinit->clkidx, modinit->clksrc, modinit->clkdiv);
        // Enable clock of paired channels
        CLK_EnableModuleClock(modinit->clkidx);
    }

    /* This function make DAC module be ready to convert. */
    DAC_Open(dac_base, 0, DAC_SOFTWARE_TRIGGER );

    /* The DAC conversion settling time is 1us */
    DAC_SetDelayTime(dac_base, 1);
            
    DAC_ENABLE_RIGHT_ALIGN(dac_base);

    uint32_t modinx =  NU_MODINDEX(obj->dac);

    // Wire pinout
    pinmap_pinout(pin, PinMap_DAC);

        /* Disable digital input path of analog pin DAC0_OUT to prevent leakage */
    GPIO_DISABLE_DIGITAL_PATH( NU_PORT_BASE(NU_PINNAME_TO_PORT(pin)), (1ul << NU_PINNAME_TO_PIN(pin) ));
        
    dac_modinit_mask |= 1 << modinx;

    return HAL_OK;
        
exit_platform_analogout_init:
        
    return HAL_ERROR;
}

static void platform_analogout_finalize(struct analogout_s *obj )
{
    uint32_t modinx =  NU_MODINDEX(obj->dac);
    struct nu_modinit_s *modinit;
    DAC_T *dac_base;

    if ((modinit = get_modinit(obj->dac, dac_modinit_tab)) == NULL) {
        goto exit_platform_analogout_finalize;
    }

    dac_base = (DAC_T *)NU_MODBASE(obj->dac);
    DAC_Close(dac_base, 0);

    dac_modinit_mask &= ~(1 << modinx);
    if (!dac_modinit_mask) {
        // Disable clock of paired channels
        CLK_DisableModuleClock(modinit->clkidx);
    }

exit_platform_analogout_finalize:

    return;
}

static struct analogout_s* hal_get_analogout_s ( dac_dev_t *dac )
{
    if ( !(dac) || (dac->port >= i32BoardMaxDACNum) )
        goto exit_hal_get_analogout_s;

    return (struct analogout_s*)&board_analogout[dac->port];

exit_hal_get_analogout_s:
    return NULL;
}

/**
 * Initialises an dac interface
 *
 * @param[in]  dac  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_dac_init(dac_dev_t *dac)
{
    struct analogout_s* obj = hal_get_analogout_s ( dac );
    
    if ( !obj )
        goto exit_hal_dac_init;
    
    return platform_analogout_init(obj, obj->pin );

exit_hal_dac_init:
    return HAL_ERROR;
}

/**
 * Start output dac
 *
 * @param[in]   dac      the interface which should be started
 * @param[out]  channel  the channel to output dac
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_dac_start(dac_dev_t *dac, uint32_t channel)
{
    struct analogout_s* obj = hal_get_analogout_s ( dac );
    struct nu_modinit_s *modinit;   
    DAC_T *dac_base = NULL;
    
    if ( !obj || !(modinit = get_modinit(obj->dac, dac_modinit_tab)) )
        goto exit_hal_dac_start;

    dac_base = (DAC_T *) NU_MODBASE(obj->dac);
    
    /* Start A/D conversion */
    dac_base->SWTRG = DAC_SWTRG_SWTRG_Msk;
    
    return HAL_OK;

exit_hal_dac_start:
    
    return HAL_ERROR;
}

/**
 * Stop output dac
 *
 * @param[in]   dac      the interface which should be stopped
 * @param[out]  channel  the channel to output dac
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_dac_stop(dac_dev_t *dac, uint32_t channel)
{
    struct analogout_s* obj = hal_get_analogout_s ( dac );
    struct nu_modinit_s *modinit;   
    DAC_T *dac_base = NULL;
    
    if ( !obj || !(modinit = get_modinit(obj->dac, dac_modinit_tab)) )
        goto exit_hal_dac_stop;

    dac_base = (DAC_T *) NU_MODBASE(obj->dac);
    
    /* Start A/D conversion */
    dac_base->SWTRG = ~DAC_SWTRG_SWTRG_Msk;
    
    return HAL_OK;

exit_hal_dac_stop:
    
    return HAL_ERROR;   
}

/**
 * Output a value to an dac interface
 *
 * @param[in]   dac      the interface to set value
 * @param[out]  channel  the channel to output dac
 * @param[in]   data     the value to output
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_dac_set_value(dac_dev_t *dac, uint32_t channel, uint32_t data)
{
    struct analogout_s* obj = hal_get_analogout_s ( dac );
    struct nu_modinit_s *modinit;   
    DAC_T *dac_base = NULL;
        
    if ( !obj || !(modinit = get_modinit(obj->dac, dac_modinit_tab)) )
        goto exit_hal_dac_set_value;
    
    dac_base = (DAC_T *) NU_MODBASE(obj->dac);
    
    /* Set DAC 12-bit holding data */
    DAC_WRITE_DATA(dac_base, 0, (uint16_t)(data&0xffff));

    /* Start A/D conversion */
    DAC_START_CONV(dac_base);
    
    return HAL_OK;

exit_hal_dac_set_value:
    return HAL_ERROR;   
}

/**
 * Returns the last data output value of the selected dac channel
 *
 * @param[in]   dac      the interface to get value
 * @param[out]  channel  channel  the channel to output dac
 *
 * @return  dac output value
 */
int32_t hal_dac_get_value(dac_dev_t *dac, uint32_t channel)
{
    struct analogout_s* obj = hal_get_analogout_s ( dac );
    DAC_T *dac_base = NULL;
    struct nu_modinit_s *modinit;   

    if ( !obj || !(modinit = get_modinit(obj->dac, dac_modinit_tab)) )
        goto exit_hal_dac_get_value;
    
    dac_base = (DAC_T *) NU_MODBASE(obj->dac);

    return DAC_READ_DATA(dac_base, 0);

exit_hal_dac_get_value:
    
    return HAL_ERROR;
}

/**
 * De-initialises an dac interface, Turns off an dac hardware interface
 *
 * @param[in]  dac  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_dac_finalize(dac_dev_t *dac)
{
    struct analogout_s* obj = hal_get_analogout_s ( dac );
    if ( !obj )
        goto exit_hal_dac_finalize;
    
    platform_analogout_finalize (obj);

    return HAL_OK;
    
exit_hal_dac_finalize:

    return HAL_ERROR;
}
