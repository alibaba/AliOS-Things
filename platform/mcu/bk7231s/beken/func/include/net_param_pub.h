#ifndef _CFG_INFO_PUB_H
#define _CFG_INFO_PUB_H


#define INFO_TLV_HEADER           (0x00564c54)   // ASCII TLV

typedef enum{
    AUTO_CONNECT_ITEM	= 0x11111111,
    WIFI_MODE_ITEM		= 0x22222222,
    DHCP_MODE_ITEM		= 0x33333333,
    WIFI_MAC_ITEM		= 0x44444444,
    SSID_KEY_ITEM		= 0x55555555,
    IP_CONFIG_ITEM		= 0x66666666
}NET_INFO_ITEM;

typedef struct info_item_st
{
    UINT32 type;
    UINT32 len;
}INFO_ITEM_ST,TLV_HEADER_ST,*INFO_ITEM_ST_PTR;

typedef struct item_common_st
{
	INFO_ITEM_ST head;
	UINT32 value;
}ITEM_COMM_ST,*ITEM_COMM_ST_PTR;

typedef struct item_mac_addr_st
{
	INFO_ITEM_ST head;
	char mac[6];
	char reserved[2];// 4bytes boundary
}ITEM_MAC_ADDR_ST,*ITEM_MAC_ADDR_ST_PTR;

typedef struct item_ssidkey_st
{
	INFO_ITEM_ST head;
	char wifi_ssid[32];
	char wifi_key[64]; 
}ITEM_SSIDKEY_ST,*ITEM_SSIDKEY_ST_PTR;

typedef struct item_ip_config_st
{
	INFO_ITEM_ST head;
	char local_ip_addr[16];
	char net_mask[16];
    char gateway_ip_addr[16];    
}ITEM_IP_CONFIG_ST,*ITEM_IP_CONFIG_ST_PTR;

UINT32 test_get_whole_tbl(UINT8 *ptr);
UINT32 save_info_item(NET_INFO_ITEM item,UINT8 *ptr0,UINT8*ptr1,UINT8 *ptr2);
UINT32 get_info_item(NET_INFO_ITEM item,UINT8 *ptr0,UINT8 *ptr1, UINT8 *ptr2);
#endif
