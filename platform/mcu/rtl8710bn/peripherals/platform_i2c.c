/**
******************************************************************************
* @file    paltform_i2c.c 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provide I2C driver functions.
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
#include "platform_peripheral.h"
#include "platform_logging.h"

#include "pinmap.h"

/******************************************************
*                    Constants
******************************************************/
//#define I2C_USE_DMA           
#define  I2C_Direction_Transmitter      ((uint8_t)0x00)
#define  I2C_Direction_Receiver         ((uint8_t)0x01)
#define IS_I2C_DIRECTION(DIRECTION) (((DIRECTION) == I2C_Direction_Transmitter) || \
                                     ((DIRECTION) == I2C_Direction_Receiver))

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
*               Variables Definitions
******************************************************/
/******************************************************
*               Function Declarations
******************************************************/
OSStatus platform_i2c_init( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
	OSStatus err = kNoErr;
	int hz = 100000;
  	platform_mcu_powersave_disable( );
  	require_action_quiet( i2c != NULL, exit, err = kParamErr);

  	if(config->address_width == I2C_ADDRESS_WIDTH_10BIT || config->address_width == I2C_ADDRESS_WIDTH_16BIT)
  	{
  		err = kUnsupportedErr;
		goto exit;
  	}

  	switch ( config->speed_mode )
  	{
    		case I2C_LOW_SPEED_MODE:  
				platform_log("Speed mode is not supported"); 
				err = kUnsupportedErr; 
				goto exit; 
				break;
    		case I2C_STANDARD_SPEED_MODE: 
				hz = 100000; 
				break;
    		case I2C_HIGH_SPEED_MODE: 
				hz = 500000; 
				break;
    		default: 
				platform_log("Speed mode is not supported"); 
				err = kUnsupportedErr; 
				goto exit; 
				break;
  	}

	i2c_init((i2c_t *)&i2c->i2c_obj, i2c->sda, i2c->scl);
	i2c_frequency((i2c_t *)&i2c->i2c_obj, hz);

exit:
  	platform_mcu_powersave_enable( );
  	return err;
}


bool platform_i2c_probe_device( const platform_i2c_t* i2c, const platform_i2c_config_t* config, int retries )
{
  	OSStatus err = kNoErr;
  
  	platform_mcu_powersave_disable();

  	require_action_quiet( i2c != NULL, exit, err = kParamErr);

exit:
  	platform_mcu_powersave_enable();
  	return ( err == kNoErr) ? true : false;
}

OSStatus platform_i2c_init_tx_message( platform_i2c_message_t* message, const void* tx_buffer, uint16_t tx_buffer_length, uint16_t retries )
{
  	OSStatus err = kNoErr;

  	require_action_quiet( ( message != NULL ) && ( tx_buffer != NULL ) && ( tx_buffer_length != 0 ), exit, err = kParamErr);

  	memset(message, 0x00, sizeof(platform_i2c_message_t));
  	message->tx_buffer = tx_buffer;
  	message->retries = retries;
  	message->tx_length = tx_buffer_length;
  
exit:  
  	return err;
}

OSStatus platform_i2c_init_rx_message( platform_i2c_message_t* message, void* rx_buffer, uint16_t rx_buffer_length, uint16_t retries )
{
  	OSStatus err = kNoErr;

  	require_action_quiet( ( message != NULL ) && ( rx_buffer != NULL ) && ( rx_buffer_length != 0 ), exit, err = kParamErr);

  	memset(message, 0x00, sizeof(platform_i2c_message_t));
  	message->rx_buffer = rx_buffer;
  	message->retries = retries;
  	message->rx_length = rx_buffer_length;
  
exit:
  	return err;
}

OSStatus platform_i2c_init_combined_message( platform_i2c_message_t* message, const void* tx_buffer, void* rx_buffer, uint16_t tx_buffer_length, uint16_t rx_buffer_length, uint16_t retries )
{
  	OSStatus err = kNoErr;

  	require_action_quiet( ( message != NULL ) && ( tx_buffer != NULL ) && ( tx_buffer_length != 0 ) && ( rx_buffer != NULL ) && ( rx_buffer_length != 0 ), exit, err = kParamErr);

  	memset(message, 0x00, sizeof(platform_i2c_message_t));
  	message->rx_buffer = rx_buffer;
  	message->tx_buffer = tx_buffer;
  	message->retries = retries;
  	message->tx_length = tx_buffer_length;
  	message->rx_length = rx_buffer_length;
  
exit:
  	return err;
}

OSStatus platform_i2c_transfer( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* messages, uint16_t number_of_messages )
{
  	OSStatus err = kNoErr;
  	int      i   = 0;

  	platform_mcu_powersave_disable();
  
  	require_action_quiet( i2c != NULL, exit, err = kParamErr);
  
  	for( i=0; i < number_of_messages; i++ )
  	{
  		if((&messages[i])->tx_buffer != NULL)
			i2c_write((i2c_t *)&i2c->i2c_obj, (int)config->address, (char *)(&messages[i])->tx_buffer, (&messages[i])->tx_length, 1);

  		if((&messages[i])->rx_buffer != NULL)
			i2c_read((i2c_t *)&i2c->i2c_obj, (int)config->address, (char *)(&messages[i])->rx_buffer, (&messages[i])->rx_length, 1);
  	}

 exit: 
  	platform_mcu_powersave_enable();
  	return err;
}

OSStatus platform_i2c_deinit( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
  	UNUSED_PARAMETER( config );
  	OSStatus err = kNoErr;
  
  	platform_mcu_powersave_disable();

  	require_action_quiet( i2c != NULL, exit, err = kParamErr);
  
exit:
  	platform_mcu_powersave_enable();
  	return err;
}

