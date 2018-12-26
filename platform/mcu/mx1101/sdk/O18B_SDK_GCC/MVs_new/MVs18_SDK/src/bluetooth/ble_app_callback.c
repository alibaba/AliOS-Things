////////////////////////////////////////////////
//
//
#include "debug.h"
#include "app_config.h"

#include "ble_api.h"
#include "ble_app_example.h"

void BLEStackCallBackFunc(uint8_t event)
{
	switch(event)
	{
		case BLE_STACK_INIT_OK:
			APP_DBG("BLE_STACK_INIT_OK\n");
			break;
		case BLE_STACK_CONNECTED:
			APP_DBG("BLE_STACK_CONNECTED\n");
			break;
			
		case BLE_STACK_DISCONNECTED:
			APP_DBG("BLE_STACK_DISCONNECTED\n");
			break;

		case GATT_SERVER_INDICATION_TIMEOUT:
			APP_DBG("GATT_SERVER_INDICATION_TIMEOUT\n");
			break;

		case GATT_SERVER_INDICATION_COMPLETE:
			APP_DBG("GATT_SERVER_INDICATION_COMPLETE\n");
			break;

		default:
			break;

	}
}

