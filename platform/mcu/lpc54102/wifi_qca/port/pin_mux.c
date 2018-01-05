/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v3.0
processor: LPC54102J512
package_id: LPC54102J512BD64
mcu_data: ksdk2_0
processor_version: 0.0.5
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

#include "fsl_common.h"
#include "fsl_iocon.h"
#include "pin_mux.h"



#define IOCON_PIO_DIGITAL_EN          0x80u   /*!< Enables digital function */
#define IOCON_PIO_FUNC1               0x01u   /*!< Selects pin function 1 */
#define IOCON_PIO_INPFILT_OFF       0x0100u   /*!< Input filter disabled */
#define IOCON_PIO_INPFILT_ON          0x00u   /*!< Input filter enabled */
#define IOCON_PIO_INV_DI              0x00u   /*!< Input function is not inverted */
#define IOCON_PIO_MODE_INACT          0x00u   /*!< No addition pin function */
#define IOCON_PIO_OPENDRAIN_DI        0x00u   /*!< Open drain is disabled */
#define IOCON_PIO_SLEW_STANDARD       0x00u   /*!< Standard mode, output slew rate control is enabled */
#define PIN0_IDX                         0u   /*!< Pin number for pin 0 in a port 0 */
#define PIN1_IDX                         1u   /*!< Pin number for pin 1 in a port 0 */
#define PORT0_IDX                        0u   /*!< Port index */


void BOARD_InitGT202Shield(void)
{
    /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */
    CLOCK_EnableClock(kCLOCK_Iocon);
    
  const uint32_t port1_pin6_config = (
    2 |                                        
    IOCON_PIO_MODE_INACT |                                   
    IOCON_PIO_INV_DI |                                       
    IOCON_PIO_DIGITAL_EN |                                   
    IOCON_PIO_INPFILT_OFF |                                  
    IOCON_PIO_SLEW_STANDARD |                                
    IOCON_PIO_OPENDRAIN_DI
  );
  IOCON_PinMuxSet(IOCON, 1, 6, port1_pin6_config);
  
  const uint32_t port1_pin14_config = (
    4 |                                        
    IOCON_PIO_MODE_INACT |                                   
    IOCON_PIO_INV_DI |                                       
    IOCON_PIO_DIGITAL_EN |                                   
    IOCON_PIO_INPFILT_OFF |                                  
    IOCON_PIO_SLEW_STANDARD |                                
    IOCON_PIO_OPENDRAIN_DI
  );
  IOCON_PinMuxSet(IOCON, 1, 14, port1_pin14_config);
  
  const uint32_t port1_pin7_config = (
    2 |                                        
    IOCON_PIO_MODE_INACT |                                   
    IOCON_PIO_INV_DI |                                       
    IOCON_PIO_DIGITAL_EN |                                   
    IOCON_PIO_INPFILT_OFF |                                  
    IOCON_PIO_SLEW_STANDARD |                                
    IOCON_PIO_OPENDRAIN_DI
  );
  IOCON_PinMuxSet(IOCON, 1, 7, port1_pin7_config);
#if 0
  const uint32_t port1_pin15_config = (
    4 |                                        
    IOCON_PIO_MODE_INACT |                                   
    IOCON_PIO_INV_DI |                                       
    IOCON_PIO_DIGITAL_EN |                                   
    IOCON_PIO_INPFILT_OFF |                                  
    IOCON_PIO_SLEW_STANDARD |                                
    IOCON_PIO_OPENDRAIN_DI
  );
#else
  const uint32_t port1_pin15_config = (
    0 |                                        
    IOCON_PIO_MODE_INACT |                                   
    IOCON_PIO_INV_DI |                                       
    IOCON_PIO_DIGITAL_EN |                                   
    IOCON_PIO_INPFILT_OFF |                                  
    IOCON_PIO_SLEW_STANDARD |                                
    IOCON_PIO_OPENDRAIN_DI
  );
#endif
  IOCON_PinMuxSet(IOCON, 1, 15, port1_pin15_config);
  
  const uint32_t port0_pin4_config = (
    0 |                                        
    IOCON_PIO_MODE_INACT |                                   
    IOCON_PIO_INV_DI |                                       
    IOCON_PIO_DIGITAL_EN |                                   
    IOCON_PIO_INPFILT_OFF |   
    IOCON_PIO_MODE(1) |   
    IOCON_PIO_SLEW_STANDARD |                                
    IOCON_PIO_OPENDRAIN_DI
  );
  IOCON_PinMuxSet(IOCON, 0, 4, port0_pin4_config);
  
  const uint32_t port0_pin6_config = (
    0 |                                        
    IOCON_PIO_MODE_INACT |                                   
    IOCON_PIO_INV_DI |                                       
    IOCON_PIO_DIGITAL_EN |                                   
    IOCON_PIO_INPFILT_OFF |                                  
    IOCON_PIO_SLEW_STANDARD |  
    IOCON_PIO_MODE(2) | 
    IOCON_PIO_OPENDRAIN_DI
  );
  IOCON_PinMuxSet(IOCON, 0, 6, port0_pin6_config);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
