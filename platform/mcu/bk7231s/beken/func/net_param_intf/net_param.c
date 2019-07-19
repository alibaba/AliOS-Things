#include "include.h"
#include "mem_pub.h"
#include "drv_model_pub.h"
#include "net_param_pub.h"
#include "flash_pub.h"
#if CFG_SUPPORT_ALIOS
#include "aos/hal/flash.h"
#else
#include "BkDriverFlash.h"
#include "BkDriverUart.h"
#endif

static UINT32 search_info_tbl(UINT8 *buf,UINT32 *cfg_len)
{
    UINT32 ret = 0, status;
    DD_HANDLE flash_handle;
    TLV_HEADER_ST head;
#if CFG_SUPPORT_ALIOS
    hal_logic_partition_t info;
    hal_logic_partition_t *pt = &info;

    hal_flash_info_get(HAL_PARTITION_PARAMETER_4, pt);
#else
	bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_NET_PARAM);
#endif

	*cfg_len = 0;
    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    ddev_read(flash_handle, (char *)&head, sizeof(TLV_HEADER_ST), pt->partition_start_addr);
    if(INFO_TLV_HEADER == head.type)
	{
	 	*cfg_len = head.len + sizeof(TLV_HEADER_ST);
        ret = 1;
		if(buf != NULL)
			ddev_read(flash_handle, buf, *cfg_len, pt->partition_start_addr);
    } 
    ddev_close(flash_handle);
    return ret;
}

static UINT32 search_info_item(NET_INFO_ITEM type, UINT32 start_addr)
{
    UINT32 status, addr, end_addr;
    DD_HANDLE flash_handle;
    INFO_ITEM_ST head;

    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    ddev_read(flash_handle, (char *)&head, sizeof(TLV_HEADER_ST), start_addr);
    addr = start_addr + sizeof(TLV_HEADER_ST);
    end_addr = addr + head.len;
    while(addr < end_addr) 
	{
        ddev_read(flash_handle, (char *)&head, sizeof(INFO_ITEM_ST), addr);
        if(type != head.type)
		{
		    addr += sizeof(INFO_ITEM_ST);
            addr += head.len;
        } 
		else 
		{
			break;
        }
    }

    if(addr >= end_addr) 
	{
        addr = 0;
    }
    ddev_close(flash_handle);

    return addr;
}

static UINT32 info_item_len(NET_INFO_ITEM item)
{
	UINT32 len = 0;
	switch(item)
	{
		case AUTO_CONNECT_ITEM:
		case WIFI_MODE_ITEM:
		case DHCP_MODE_ITEM:
			len = sizeof(ITEM_COMM_ST);
			break;
		case WIFI_MAC_ITEM:
			len = sizeof(ITEM_MAC_ADDR_ST);
			break;
			
		case SSID_KEY_ITEM:
			len = sizeof(ITEM_SSIDKEY_ST);
			break;
		case IP_CONFIG_ITEM:
			len = sizeof(ITEM_IP_CONFIG_ST);
			break;
		default:
			len = sizeof(ITEM_COMM_ST);
			break;
	}
	return len;
}

UINT32 get_info_item(NET_INFO_ITEM item,UINT8 *ptr0,UINT8 *ptr1, UINT8 *ptr2)
{
    UINT32 status, addr_start,len;
    DD_HANDLE flash_handle;
    INFO_ITEM_ST head;
#if CFG_SUPPORT_ALIOS
	hal_logic_partition_t info;
    hal_logic_partition_t *pt;
#else
	bk_logic_partition_t *pt;
#endif
	UINT32 ret = 0;
	
    if(!search_info_tbl(NULL,&len))
		return ret;

#if CFG_SUPPORT_ALIOS
    pt = &info;
    hal_flash_info_get(HAL_PARTITION_PARAMETER_4, pt);
#else
	pt = bk_flash_get_info(BK_PARTITION_NET_PARAM);
#endif
    addr_start = search_info_item(item, pt->partition_start_addr);
    if(!addr_start) 
        return ret;    

	flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    ddev_read(flash_handle, (char *)&head, sizeof(INFO_ITEM_ST), addr_start);
	addr_start += sizeof(INFO_ITEM_ST);

	switch(item)
	{
		case AUTO_CONNECT_ITEM:
		case WIFI_MODE_ITEM:
		case DHCP_MODE_ITEM:
		case WIFI_MAC_ITEM:
			if(ptr0 != NULL)
			{
				ddev_read(flash_handle, (char *)ptr0, head.len, addr_start);
				ret = 1;
			}
			break;
			
		case SSID_KEY_ITEM:
			if((ptr0 != NULL) && (ptr1 != NULL))
			{
				ddev_read(flash_handle, (char *)ptr0, 32, addr_start);
				addr_start += 32;
				ddev_read(flash_handle, (char *)ptr1, 64, addr_start);
				ret = 1;
			}
			break;
			
		case IP_CONFIG_ITEM:
			if((ptr0 != NULL) && (ptr1 != NULL) && (ptr2 != NULL))
			{
				ddev_read(flash_handle, (char *)ptr0, 16, addr_start);
				addr_start += 16;
				ddev_read(flash_handle, (char *)ptr1, 16, addr_start);
				addr_start += 16;
				ddev_read(flash_handle, (char *)ptr2, 16, addr_start);
				ret = 1;
			}
			break;
			
		default:
			ret = 0;
			break;
	}
    	
	
	ddev_close(flash_handle);
	return ret;
}

UINT32 save_info_item(NET_INFO_ITEM item,UINT8 *ptr0,UINT8*ptr1,UINT8 *ptr2)
{
	UINT32 status, addr_offset,cfg_tbl_len,item_len,tmp;
	UINT8 *tmpptr;
	UINT8 *item_buf;
	UINT32 *wrbuf;
	DD_HANDLE flash_handle;
    INFO_ITEM_ST head;
	INFO_ITEM_ST_PTR item_head_ptr;
#if CFG_SUPPORT_ALIOS
	UINT32 offset;
    hal_logic_partition_t info;
    hal_logic_partition_t *pt = &info;

    hal_flash_info_get(HAL_PARTITION_PARAMETER_4, pt);
#else
	bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_NET_PARAM);
#endif

	item_len = info_item_len(item);
	
	head.type = INFO_TLV_HEADER;
	
    if(!search_info_tbl(NULL,&cfg_tbl_len))//no TLV
    {
    	cfg_tbl_len = sizeof(INFO_ITEM_ST)+item_len;
		addr_offset = sizeof(INFO_ITEM_ST);
		head.len = item_len;
		wrbuf = os_zalloc(cfg_tbl_len);
		if(wrbuf == NULL)
			return 0;
    }
	else
	{
	    addr_offset = search_info_item(item, pt->partition_start_addr);
		
		if(!addr_offset)//no item
		{
			addr_offset = cfg_tbl_len;
			cfg_tbl_len += item_len;
		}
		else
		{
			addr_offset -= pt->partition_start_addr;
		}
		wrbuf = os_zalloc(cfg_tbl_len);
		if(wrbuf == NULL)
			return 0;
		search_info_tbl((UINT8*)wrbuf,&tmp);
		head.len = cfg_tbl_len - sizeof(TLV_HEADER_ST);
	}

	tmpptr = (UINT8*)wrbuf;
	item_head_ptr = (INFO_ITEM_ST_PTR)(tmpptr + addr_offset);
	item_buf =(UINT8*)(tmpptr + addr_offset + sizeof(INFO_ITEM_ST));
	switch(item)
	{
		case AUTO_CONNECT_ITEM:
		case WIFI_MODE_ITEM:
		case DHCP_MODE_ITEM:
			os_memcpy(item_buf,ptr0,4);
			break;
			
		case WIFI_MAC_ITEM:
			os_memcpy(item_buf,ptr0,6);
			break;
			
		case SSID_KEY_ITEM:
			os_memcpy(item_buf,ptr0,32);
			os_memcpy(item_buf+32,ptr1,64);
			break;
			
		case IP_CONFIG_ITEM:
			os_memcpy(item_buf,ptr0,16);
			os_memcpy(item_buf+16,ptr1,16);
			os_memcpy(item_buf+32,ptr2,16);
			break;
			
		default:
			os_memcpy(item_buf,ptr0,4);
			break;
	}
	item_head_ptr->type = item;
	item_head_ptr->len = item_len- sizeof(INFO_ITEM_ST);

//set TLV header
	os_memcpy(tmpptr,&head,sizeof(TLV_HEADER_ST));
//assume info cfg tbl size is less than 4k
#if CFG_SUPPORT_ALIOS
	offset = 0;
	hal_flash_dis_secure(HAL_PARTITION_PARAMETER_4, 0, 0);
	hal_flash_erase(HAL_PARTITION_PARAMETER_4,0,cfg_tbl_len);
	hal_flash_write(HAL_PARTITION_PARAMETER_4,&offset,tmpptr,cfg_tbl_len);
	hal_flash_enable_secure(HAL_PARTITION_PARAMETER_4, 0, 0);
#else
	bk_flash_enable_security(FLASH_PROTECT_NONE);
	bk_flash_erase(BK_PARTITION_NET_PARAM,0,cfg_tbl_len);
	bk_flash_write(BK_PARTITION_NET_PARAM,0,tmpptr,cfg_tbl_len);
	bk_flash_enable_security(FLASH_UNPROTECT_LAST_BLOCK);
#endif
	
	os_free(wrbuf);
	
	return 1;
}
/////////////////////for test purpose/////////////////
UINT32 test_get_whole_tbl(UINT8 *ptr)
{
	UINT32 len;
	return search_info_tbl(ptr,&len);
}
