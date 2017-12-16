/////////////////////////////////////////////
//
//

#include "type.h"

#ifndef __BLE_API_H__
#define __BLE_API_H__

//if this transaction mode, short value writed by client (Write Request)
#define TRANSACTION_MODE_NONE      0x0
//if this transaction mode, prepare write command send by client (Prepare Write Request)
#define TRANSACTION_MODE_ACTIVE    0x1
//if this transaction mode, excute write command send by client (Execute Write Request)
#define TRANSACTION_MODE_EXECUTE   0x2
//excute write command end
#define TRANSACTION_MODE_CANCEL    0x3


typedef enum{
	CENTRAL_DEVICE,
	PERIPHERAL_DEVICE
}BLE_DEVICE_ROLE;

typedef enum{
	BLE_STACK_INIT_OK = 0,				// 协议栈初始化成功
	BLE_STACK_CONNECTED,				// 连接成功
	BLE_STACK_DISCONNECTED,				// 断开连接

	GATT_SERVER_INDICATION_TIMEOUT,
	GATT_SERVER_INDICATION_COMPLETE,
} BLE_STACK_CALLBACK;


/*
* 根据handle获取uuid，如果是自己定义的uuid，则需要返回 >= 0 的值，否则返回 -1
*/
typedef int16_t (*att_read_f)(uint16_t /*con_handle*/, uint16_t /*attribute_handle*/, uint16_t /*offset*/, uint8_t * /*buffer*/, uint16_t /*buffer_size*/);

/*
* 根据handle获取uuid，如果是自己定义的uuid，则需要返回 >= 0 的值，否则返回 -1
*/
typedef int16_t (*att_write_f)(uint16_t /*con_handle*/, uint16_t /*attribute_handle*/, uint16_t /*transaction_mode*/, uint16_t /*offset*/, uint8_t * /*buffer*/, uint16_t /*buffer_size*/);


typedef struct{
	uint16_t			flags;//if current is characteristic value, need add ATT_PROPERTY_DYNAMIC flag, is is 128 uuid, need add ATT_PROPERTY_UUID128
	uint16_t			handle;    
	uint16_t			uuid;//all use 16bit uuid
	uint8_t *			uuid128;
	uint8_t *			value;
	uint8_t				value_len;
}attribute_item_t;


typedef struct  _GATT_SERVER_PROFILE
{
	attribute_item_t * 	attrs;		// attributes
	uint32_t			attrs_size;	// size of attrs in bytes

	att_read_f			attr_read;
	att_write_f			attr_write;
	
//	uint8_t *			read_buf;
//	uint32_t			read_buf_len;
//	uint8_t *			write_buf;
//	uint32_t			write_buf_len;
}GATT_SERVER_PROFILE;

typedef struct _BLE_APP_CONTEXT
{
	BLE_DEVICE_ROLE		ble_device_role;
	uint8_t *			ble_device_name;
}BLE_APP_CONTEXT;


/*
* 初始化BLE协议栈
* 
* 返回值
* 	0 : success
*	negative : failed (error code : see BLE_ERROR)
*/
int8_t	InitBleStack(BLE_APP_CONTEXT * ble_app_context, GATT_SERVER_PROFILE * gatt_server_profile);


/*
* BLE协议栈回调函数
*/
void BLEStackCallBackFunc(uint8_t event);

/*
* 根据handle获取uuid
*/
uint16_t GetUuidForHandle(uint16_t handle);


/*
 * 以notify方式将数据传递数据给client端，client端不需要response
 * returns:
 *		0 		: OK
 *		0x57	: BT Stack ACL buffer is full
 */
int GattServerNotify(uint16_t handle, uint8_t *value, uint16_t value_len);


/*
 * 以indicate方式将数据传递数据给client端，client端需要response
 * 在BLECallback中会收到相应事件
 *	GATT_SERVER_INDICATION_TIMEOUT	: response 超时
 *	GATT_SERVER_INDICATION_COMPLETE	: response OK
 * 
 * returns:
 *		0 		: OK
 * 		0x90	: GATT_SERVER_INDICATION_IN_PORGRESS
 *		0x57	: BT Stack ACL buffer is full
 */
int GattServerIndicate(uint16_t handle, uint8_t *value, uint16_t value_len);


#endif
