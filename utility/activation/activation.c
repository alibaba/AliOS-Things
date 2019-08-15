/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include "activation.h"
#include "linkkit/infra/infra_report.h"
#include "linkkit/wrappers/wrappers_defs.h"
#include "linkkit/wrappers/wrappers_os.h"
#include "ulog/ulog.h"

#ifndef NULL
#define NULL    ((void *)0)
#endif

#define ENCYPT_FAIL 1
#define ENCYPT_SUCCESS 0

#define PLAIN_FLAG "PLAIN"

#define TAG "activation"
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

static const char *DEVICE_INFO_UPDATE_FMT = "{\"id\":\"%d\",\"version\":\"1.0\",\"params\":["
                         "{\"attrKey\":\"SYS_ALIOS_ACTIVATION\",\"attrValue\":\"%s\",\"domain\":\"SYSTEM\"}"
                         "],\"method\":\"thing.deviceinfo.update\"}";
                                     
int iotx_report_ext_msg(info_report_func_pt info_report_func, void * handle)
{
    int ret = 0;
    int i;
    char mac[MAC_ADDRESS_SIZE] = {0};
    char version[VERSION_NUM_SIZE] = {0};
    char random_num[RANDOM_NUM_SIZE];
    char chip_code[CHIP_CODE_SIZE] = {0};
    char output[AOS_ACTIVE_INFO_LEN] = {0};
    char topic_name[IOTX_URI_MAX_LEN + 1] = {0};
    char *msg = NULL;
    int  msg_len = 0;
    char _product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char _device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};

    if (info_report_func == NULL) {
        LOGE(TAG,"report func not register!");
        return -1;
    }
    HAL_GetProductKey(_product_key);
    HAL_GetDeviceName(_device_name);

    LOGD(TAG,"devinfo report");

    /* Construct aos activation data */
    aos_get_version_hex((unsigned char *)version);
    aos_get_mac_hex((unsigned char *)mac);
    mac[6] = ACTIVE_SINGLE_GW;
#ifdef BUILD_AOS
    mac[7] = ACTIVE_LINKKIT_AOS;
#else
    mac[7] = ACTIVE_LINKKIT_ONLY;
#endif

    aos_srand(HAL_UptimeMs());
    for (i = 0; i < 4; i ++) {
        random_num[i] = (char)(aos_rand()&0xFF);
    }
    aos_get_chip_code((unsigned char *)chip_code);
    /*
    input: version 4byte + random 4 byte + mac 4byte + chip_code 4byte
    output: output_buffer store the version info process. length at least OUTPUT_SPACE_SIZE
    return: 0 success, 1 failed
    */
    ret = aos_get_version_info((unsigned char *)version, (unsigned char *)random_num, (unsigned char *)mac,
                               (unsigned char *)chip_code, (unsigned char *)output, AOS_ACTIVE_INFO_LEN);
    if (ret) {
        LOGE(TAG,"aos_get_version_info failed");
        return FAIL_RETURN;
    }
    LOGD(TAG,"get aos avtive info: %s", output);

    /* devinfo update topic name */
    ret = snprintf(topic_name,
                       IOTX_URI_MAX_LEN,
                       "/sys/%s/%s/thing/deviceinfo/update",
                       _product_key,
                       _device_name);
    if (ret <= 0) {
        LOGE(TAG,"topic generate err");
        return FAIL_RETURN;
    }
    LOGD(TAG,"devinfo report topic: %s", topic_name);

    msg_len = strlen(DEVICE_INFO_UPDATE_FMT) + 10 + AOS_ACTIVE_INFO_LEN + 1;
    msg = (char *)aos_malloc(msg_len);
    if (msg == NULL) {
        LOGE(TAG,"malloc err");
        return FAIL_RETURN;
    }
    memset(msg, 0, msg_len);

    /* devinfo update message */
    ret = snprintf(msg,
                       msg_len,
                       DEVICE_INFO_UPDATE_FMT,
                       iotx_report_id(),
                       output
                      );
    if (ret <= 0) {
        LOGE(TAG,"topic msg generate err");
        aos_free(msg);
        return FAIL_RETURN;
    }
    LOGD(TAG,"devinfo report data: %s", msg);

    if (info_report_func != NULL) {
        info_report_func(handle, topic_name, 1, msg, strlen(msg));
    }

    aos_free(msg);
    if (ret < 0) {
        LOGE(TAG,"publish failed, ret = %d", ret);
        return FAIL_RETURN;
    }
    LOGD(TAG,"devinfo report succeed");

    return SUCCESS_RETURN;
}                                     
