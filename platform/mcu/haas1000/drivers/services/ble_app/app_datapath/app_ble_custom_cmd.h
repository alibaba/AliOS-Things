/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_BLE_CUSTOM_CMD_H__
#define __APP_BLE_CUSTOM_CMD_H__

#ifdef __cplusplus
extern "C" {
#endif

	
/**
 * @brief The custom command code
 *
 */
typedef enum
{
	/* 0 */ OP_RESPONSE_TO_CMD = 0,	/**< the payload is: OP_RESPONSE_TO_CMD + paramLen + BLE_CUSTOM_CMD_RSP_T */
	/* 1 */ OP_START_RAW_DATA_XFER,	/**< raw data means no BLE_CUSTOM_CMD_PAYLOAD_T.cmdCode but just the data
								starting raw data xfer is done via:
									1. send OP_START_RAW_DATA_XFER to target peer device 
									2. get OP_RESPONSE_TO_CMD from the target device
									3. send raw data
							*/
	/* 2 */ OP_STOP_RAW_DATA_XFER,	/**< raw data means no BLE_CUSTOM_CMD_PAYLOAD_T.cmdCode but just the data
								stopping raw data xfer is done via:
									1. send OP_STOP_RAW_DATA_XFER to target peer device 
									2. get OP_RESPONSE_TO_CMD from the target device
									3. stop sending raw data
							*/
	/* 3 */ OP_TEST_NO_RESPONSE_PRINT,

	/* 4 */ OP_TEST_WITH_RESPONSE_PRINT,

	/* 5 */ OP_ENTER_OTA_MODE,
	// TO ADD: new custom command code

	/* 6 */ OP_SW_IIR_EQ,		

	/* 7 */ OP_GET_BT_ADDRESS,

	/* 8 */ OP_COMMAND_COUNT,
		
} BLE_CUSTOM_CMD_CODE_E;


#ifdef __cplusplus
	}
#endif


#endif // #ifndef __APP_BLE_CUSTOM_CMD_H__

