#include "ble_pub.h"
#include <stddef.h>
#include <stdint.h>
#include "breeze_hal_ble.h"

extern struct bd_addr common_default_bdaddr;
static ais_bt_init_t ais_bt_init_info;
static void (*g_indication_txdone)(uint8_t res);
static stack_init_done_t stack_init_done;

void ble_event_callback(ble_event_t event, void *param)
{
	switch(event)	  
	{ 	   
		case BLE_STACK_OK:		 
			{	
				bk_printf("BLE_STACK_OK\r\n");
				if(stack_init_done)
				{
					stack_init_done(AIS_BT_REASON_REMOTE_USER_TERM_CONN);
				}
			}		  
			break;		
		case BLE_STACK_FAIL:		
			{	
				bk_printf("BLE_STACK_FAIL\r\n");
				if(stack_init_done)
				{
					stack_init_done(AIS_BT_REASON_UNSPECIFIED);
				}
			} 	   
			break;		 
		case BLE_CONNECT:		  
			{ 		   
				bk_printf("BLE CONNECT\r\n");			
				ais_bt_init_info.on_connected();		
			}		 
			break; 	   
		case BLE_DISCONNECT: 	   
			{			
				bk_printf("BLE DISCONNECT\r\n");			
				ais_bt_init_info.on_disconnected(); 	   
			}		
			break;		  
		case BLE_MTU_CHANGE:		  
			{ 		   
				bk_printf("BLE_MTU_CHANGE:%d\r\n", *(uint16_t *)param);		  
			} 	   
			break;		 
		case BLE_CFG_NOTIFY:		 
			{			  
				bk_printf("BLE_CFG_NTFIND:0x%x\r\n", *(uint8_t *)param);			  
				ais_bt_init_info.nc.on_ccc_change(*(uint8_t *)param); 	   
			}		
			break;		  
		case BLE_CFG_INDICATE:		
			{			 
				bk_printf("BLE_CFG_NTFIND:0x%x\r\n", *(uint8_t *)param);			 
				ais_bt_init_info.ic.on_ccc_change(*(uint8_t *)param);		  
			} 	   
			break;	
		case BLE_TX_DONE:
			{
				bk_printf("BLE_TX_DONE\r\n");
				if(g_indication_txdone)
				{
					g_indication_txdone(AIS_ERR_SUCCESS);
				}
			}
			break;
		default:			 
			bk_printf("UNKNOW EVENT\r\n"); 	   
			break;	 
	}
}

void ble_write_callback(uint16_t char_id, uint8_t *data, uint8_t len)
{	 
	if(char_id == 0xFED5)	  
	{ 	   
		ais_bt_init_info.wc.on_write(data, len);    
	}	
	else if(char_id == 0xFED7)	  
	{ 	   
		ais_bt_init_info.wwnrc.on_write(data, len);	  
	}    
	else    
	{		
		bk_printf("ERROR UUID\r\n");	
	}
}

void ble_read_callback(uint16_t char_id, uint8_t *data, uint8_t len)
{    
	if(char_id == 0xFED4)	
	{		 
		ais_bt_init_info.rc.on_read(data, len);	
	}	 
	else	 
	{		  
		bk_printf("ERROR UUID\r\n");	  
	}
}

ais_err_t ble_stack_init(ais_bt_init_t *info, stack_init_done_t init_done)
{
    memcpy((uint8_t *)&ais_bt_init_info,(uint8_t *)info,sizeof(ais_bt_init_t));
    
    ble_activate(NULL);
	stack_init_done = init_done;
    ble_set_write_cb(ble_write_callback);    
	ble_set_event_cb(ble_event_callback);    
	ble_set_read_cb(ble_read_callback);    
	return AIS_ERR_SUCCESS;
}

ais_err_t ble_stack_deinit()
{
    memset((uint8_t *)&ais_bt_init_info,0,sizeof(ais_bt_init_t));

    ble_send_msg(BLE_MSG_EXIT);
    return AIS_ERR_SUCCESS;
}

ais_err_t ble_send_notification(uint8_t *p_data, uint16_t length)
{
    ais_err_t status = AIS_ERR_SUCCESS;
	
    status = feb3_send_fed8_ntf_value(length,p_data,0xff);

    bk_printf("status:%d\r\n", status);
	
    return status;
}

ais_err_t ble_send_indication(uint8_t *p_data, uint16_t length, void (*txdone)(uint8_t res))
{
    ais_err_t status = AIS_ERR_SUCCESS;

    status = feb3_send_fed6_ind_value(length,p_data,0xff);
	
	g_indication_txdone = txdone;
	
    return status;
}

void ble_disconnect(uint8_t reason)
{
    bk_printf("ble_disconnect\r\n");
    appm_disconnect(reason);
}

ais_err_t ble_advertising_start(ais_adv_init_t *adv)
{
    ais_err_t status = AIS_ERR_SUCCESS;
    uint8_t adv_idx, adv_name_len;
    
    adv_idx = 0;
    adv_info.advData[adv_idx] = 0x02; adv_idx++;
    adv_info.advData[adv_idx] = 0x01; adv_idx++;
    adv_info.advData[adv_idx] = 0x06; adv_idx++;

    adv_info.advData[adv_idx] = adv->vdata.len + 1; adv_idx +=1;
    adv_info.advData[adv_idx] = 0xFF; adv_idx +=1;
    memcpy(&adv_info.advData[adv_idx], adv->vdata.data, adv->vdata.len); adv_idx += adv->vdata.len;

    adv_name_len = strlen(adv->name.name) + 1;
    adv_info.advData[adv_idx] = adv_name_len; adv_idx +=1;
    adv_info.advData[adv_idx] = adv->name.ntype + 0x08; adv_idx +=1; //name
    memcpy(&adv_info.advData[adv_idx], adv->name.name, strlen(adv->name.name)); adv_idx +=strlen(adv->name.name);

    adv_info.advDataLen = adv_idx;

    adv_idx = 0;
        
    adv_info.respData[adv_idx] = adv_name_len; adv_idx +=1;
    adv_info.respData[adv_idx] = adv->name.ntype + 0x07; adv_idx +=1; //name
    memcpy(&adv_info.respData[adv_idx], adv->name.name, strlen(adv->name.name)); adv_idx +=strlen(adv->name.name);
    adv_info.respDataLen = adv_idx;
    
    status = appm_start_advertising();

    return status;
}

ais_err_t ble_advertising_stop()
{
    bk_printf(__func__);
    ais_err_t status = AIS_ERR_SUCCESS;
    
    status = appm_stop_advertising();

    return status;
}

ais_err_t ble_get_mac(uint8_t *mac)
{
    bk_printf(__func__);
    ais_err_t status = AIS_ERR_SUCCESS;
    
    memcpy(mac, &common_default_bdaddr, AIS_BT_MAC_LEN);

    return status;
}
