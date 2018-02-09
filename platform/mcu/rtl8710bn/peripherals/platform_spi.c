/**
******************************************************************************
* @file    paltform_spi.c 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provide SPI driver functions.
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy 
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/ 


//#include "MICORTOS.h"
//#include "MICOPlatform.h"

#include "board.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "debug.h"

#include "pinmap.h"

#include "spi_ex_api.h"
/******************************************************
*                    Constants
******************************************************/

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/

/******************************************************
*               Static Function Declarations
******************************************************/

/******************************************************
*               Variables Definitions
******************************************************/

/******************************************************
*               Function Definitions
******************************************************/
OSStatus platform_spi_init( platform_spi_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_config_t* config )
{
  	OSStatus          err = kNoErr;
	u8 polarity;
	u8 phase;
	
  	platform_mcu_powersave_disable();
  
  	require_action_quiet( ( driver != NULL ) && ( peripheral != NULL ) && ( config != NULL ), exit, err = kParamErr);
  	//require_action( !(config->mode & SPI_USE_DMA), exit, err = kUnsupportedErr);

  	require_noerr(err, exit);

  	spi_init((spi_t*)&peripheral->spi_obj, peripheral->mosi, peripheral->miso, peripheral->sclk, peripheral->ssel);
    	spi_frequency((spi_t*)&peripheral->spi_obj, config->speed);

  	if ( config->mode & SPI_CLOCK_IDLE_HIGH )
    		polarity = SPI_SCLK_IDLE_HIGH;
  	else
    		polarity = SPI_SCLK_IDLE_LOW;
  
  	if ( config->mode & SPI_CLOCK_RISING_EDGE )
    		phase = ( config->mode & SPI_CLOCK_IDLE_HIGH ) ? SPI_SCLK_TOGGLE_START : SPI_SCLK_TOGGLE_MIDDLE;
  	else
    		phase = ( config->mode & SPI_CLOCK_IDLE_HIGH ) ? SPI_SCLK_TOGGLE_MIDDLE : SPI_SCLK_TOGGLE_START;

	spi_format((spi_t*)&peripheral->spi_obj, config->bits, (polarity|phase) , 0);
  
exit:
  	platform_mcu_powersave_enable();
  	return err;
}



OSStatus platform_spi_deinit( platform_spi_driver_t* driver )
{
  UNUSED_PARAMETER( driver );
  /* TODO: unimplemented */
  return kUnsupportedErr;
}


OSStatus platform_spi_transfer( platform_spi_driver_t* driver, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
  OSStatus err    = kNoErr;
  uint32_t count  = 0;
  uint16_t i;
  
  platform_mcu_powersave_disable();
  
  require_action_quiet( ( driver != NULL ) && ( config != NULL ) && ( segments != NULL ) && ( number_of_segments != 0 ), exit, err = kParamErr);
  
  /* Activate chip select */
  
  for ( i = 0; i < number_of_segments; i++ )
  {
    {
      count = segments[i].length;
      
      /* in interrupt-less mode */
      if ( config->bits == 8 )
      {
        const uint8_t* send_ptr = ( const uint8_t* )segments[i].tx_buffer;
        uint8_t*       rcv_ptr  = ( uint8_t* )segments[i].rx_buffer;
        
        while ( count-- )
        {
          uint16_t data = 0xFF;
          
          if ( send_ptr != NULL )
          {
            data = *send_ptr++;
          }
          
          data = spi_master_write( (spi_t*)&driver->peripheral->spi_obj, (int)data );
          
          if ( rcv_ptr != NULL )
          {
            *rcv_ptr++ = (uint8_t)data;
          }
        }
      }
      else if ( config->bits == 16 )
      {
        const uint16_t* send_ptr = (const uint16_t *) segments[i].tx_buffer;
        uint16_t*       rcv_ptr  = (uint16_t *) segments[i].rx_buffer;
        
        /* Check that the message length is a multiple of 2 */
        
        require_action_quiet( ( count % 2 ) == 0, cleanup_transfer, err = kSizeErr);
        
        /* Transmit/receive data stream, 16-bit at time */
        while ( count != 0 )
        {
          uint16_t data = 0xFFFF;
          
          if ( send_ptr != NULL )
          {
            data = *send_ptr++;
          }
          
          data = spi_master_write( (spi_t*)&driver->peripheral->spi_obj, (int)data );
          
          if ( rcv_ptr != NULL )
          {
            *rcv_ptr++ = data;
          }
          
          count -= 2;
        }
      }
    }
  }
  
cleanup_transfer:
  ///* Deassert chip select */
  //platform_gpio_output_high( config->chip_select );
  
exit:
  platform_mcu_powersave_enable( );
  return err;
}

