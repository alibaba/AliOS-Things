/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include "activation.h"

#ifndef NULL
#define NULL    ((void *)0)
#endif

#define ENCYPT_FAIL 1
#define ENCYPT_SUCCESS 0

#define PLAIN_FLAG "PLAIN"

/* Don't do anything in release builds */
#define DEBUG_PRINTF(fmt, ...)
//#define DEBUG_PRINTF printf 

//num is 0~0xF
UINT8 tans_num2char( UINT8 num )
{
	if( num < 0xa)
	{
		return '0'+num;
	}
	else
	{
		return 'A'+(num-0xa);
	}
}

//trans_byte2hex, 1 byte to 2 hex(store in 2 byte)
void trans_byte2hex( UINT8* poutput, UINT8 input )
{
	UINT8 num;
	num = (input>>4);
	poutput[0] = tans_num2char(num);

	num = (((UINT8)(input<<4))>>4);
	poutput[1] = tans_num2char(num);
}


//input argument: output_buffer 外部传入
UINT32 aos_get_version_info (UINT8 version_num[VERSION_NUM_SIZE], UINT8 random_num[RANDOM_NUM_SIZE], UINT8 mac_address[MAC_ADDRESS_SIZE], UINT8 chip_code[CHIP_CODE_SIZE], UINT8 *output_buffer, UINT32 output_buffer_size)
{
    if( NULL == version_num || NULL == mac_address || NULL == chip_code || NULL == output_buffer )
	{
		return ENCYPT_FAIL;
	}
	
	if( output_buffer_size < OUTPUT_SPACE_SIZE )
	{
		printf("output_buffer_size not enough, need %d\n", OUTPUT_SPACE_SIZE);
		return ENCYPT_FAIL;
	}

	DEBUG_PRINTF("in aos_get_version_info!!\n");
	
	UINT8 encrypt_input[INPUT_SIZE];
	memcpy( encrypt_input, version_num, VERSION_NUM_SIZE);
	memcpy( encrypt_input+VERSION_NUM_SIZE, random_num, RANDOM_NUM_SIZE);
	memcpy( encrypt_input+VERSION_NUM_SIZE+RANDOM_NUM_SIZE,mac_address, MAC_ADDRESS_SIZE);
	memcpy( encrypt_input+VERSION_NUM_SIZE+RANDOM_NUM_SIZE+MAC_ADDRESS_SIZE, chip_code, CHIP_CODE_SIZE);
	DEBUG_PRINTF(" after memcpy!!\n");
    
    UINT32 count;
    for( count=0; count<INPUT_SIZE; count++ )
	{
		trans_byte2hex( &output_buffer[2*count], encrypt_input[count]);
	}
    memcpy( &output_buffer[2*count], PLAIN_FLAG, strlen(PLAIN_FLAG) );
    output_buffer[2*count+strlen(PLAIN_FLAG)]=0;
    
    DEBUG_PRINTF("output_buffer:%s\n", output_buffer );
    return ENCYPT_SUCCESS;    
}


