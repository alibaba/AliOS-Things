#include <string.h>
#include <trace_app.h>
#include <gap_bond_le.h>
#include <gap_scan.h>
#include <gap.h>
#include <gap_conn_le.h>
#include <gcs_client.h>
#include "ble_central_client_app.h"

//#include "user_cmd.h"
#include <platform/platform_stdlib.h>

#define BD_ADDR_LEN							6

static u8 ctoi(char c)
{
	if((c >= 'A') && (c <= 'F')) {
		return (c - 'A' + 0x0A);
	}

	if((c >= 'a') && (c <= 'f')) {
		return (c - 'a' + 0x0A);
	}

	if((c >= '0') && (c <= '9')) {
		return (c - '0' + 0x00);
	}

	return 0xFF;
}

static u8 hex_str_to_bd_addr(u32 str_len, s8 *str, u8 *num_arr)
{
	num_arr += str_len/2 -1;
	u32 n = 0;
	u8 num = 0;

	if (str_len < 2) {
		return FALSE;
	}
	while (n < str_len) {
		if ((num = ctoi(str[n++])) == 0xFF) {
			return FALSE;
		}
		*num_arr = num << 4;
		if ((num = ctoi(str[n++])) == 0xFF) {
			return FALSE;
		}
		*num_arr |= num;
		num_arr--;
	}
	return TRUE;
}

static int hex_str_to_int(u32 str_len, s8*str)
{
	int result = 0;
	int n = 2;
	if(str[0]!='0' && ((str[1] != 'x') && (str[1] != 'X'))){
		return -1;
	}
	while(n < str_len){
		result = (result << 4) | (ctoi(str[n++]));
	}
	return result;
}

static int dec_str_to_int(u32 str_len, s8*str)
{
	int result = 0;
	int n = 0;
	
	while(n < str_len){
		result = result*10 + (ctoi(str[n++]));
	}
	return result;
}

int ble_central_at_cmd_connect(int argc, char **argv)
{
    u8 DestAddr[6] = {0};
    u8 addr_len;
    u8 DestAddrType = GAP_REMOTE_ADDR_LE_PUBLIC;
    T_GAP_LE_CONN_REQ_PARAM conn_req_param;

	if(strcmp(argv[1], "P") == 0)
		DestAddrType = GAP_REMOTE_ADDR_LE_PUBLIC;
	else if(strcmp(argv[1],"R") == 0)
		DestAddrType = GAP_REMOTE_ADDR_LE_RANDOM;

	if (strlen(argv[2]) != 2*BD_ADDR_LEN)
		return -1;	
	
	hex_str_to_bd_addr(strlen(argv[2]), argv[2], DestAddr);
	//mtu = (argc==4)? atoi((const char *)(argv[3])): 256;

    conn_req_param.scan_interval = 0x10;
    conn_req_param.scan_window = 0x10;
    conn_req_param.conn_interval_min = 80;
    conn_req_param.conn_interval_max = 80;
    conn_req_param.conn_latency = 0;
    conn_req_param.supv_tout = 1000;
    conn_req_param.ce_len_min = 2 * (conn_req_param.conn_interval_min - 1);
    conn_req_param.ce_len_max = 2 * (conn_req_param.conn_interval_max - 1);
    le_set_conn_param(GAP_CONN_PARAM_1M, &conn_req_param);

    BLE_PRINT("cmd_con, DestAddr: 0x%2X:0x%2X:0x%2X:0x%2X:0x%2X:0x%2X\r\n", 
			DestAddr[0], DestAddr[1], DestAddr[2], DestAddr[3], DestAddr[4],DestAddr[5]);

    le_connect(0, DestAddr, (T_GAP_REMOTE_ADDR_TYPE)DestAddrType, GAP_LOCAL_ADDR_LE_PUBLIC,
               1000);

    return 0;
}

int ble_central_at_cmd_disconnect(int argc, char **argv)

{
	u8 conn_id = atoi(argv[1]);
	int ret;
    ret = le_disconnect(conn_id);
	return ret;
}

int ble_central_at_cmd_get_conn_info(int argc, char **argv)

{
    u8 conn_id;
    T_GAP_CONN_INFO conn_info;
    for (conn_id = 0; conn_id < BLE_CENTRAL_APP_MAX_LINKS; conn_id++)
    {
        if (le_get_conn_info(conn_id, &conn_info))
        {
            BLE_PRINT("ShowCon conn_id %d state 0x%x role %d\r\n", conn_id,
                            conn_info.conn_state, conn_info.role);
            BLE_PRINT("RemoteBd = [%02x:%02x:%02x:%02x:%02x:%02x] type = %d\r\n",
                            conn_info.remote_bd[5], conn_info.remote_bd[4],
                            conn_info.remote_bd[3], conn_info.remote_bd[2],
                            conn_info.remote_bd[1], conn_info.remote_bd[0],
                            conn_info.remote_bd_type);
        }
    }
    BLE_PRINT("active link num %d,  idle link num %d\r\n",
                    le_get_active_link_num(), le_get_idle_link_num());
    return 0;

}

int ble_central_at_cmd_update_conn_request(int argc, char **argv)
{
	int ret;
    u8 conn_id = atoi(argv[1]);
	u16 conn_interval_min = hex_str_to_int(strlen(argv[2]), argv[2]);
	u16 conn_interval_max = hex_str_to_int(strlen(argv[3]), argv[3]);
	u16 conn_latency = hex_str_to_int(strlen(argv[4]), argv[4]);
    u16 supervision_timeout = hex_str_to_int(strlen(argv[5]), argv[5]);

    ret = le_update_conn_param(conn_id,
                               conn_interval_min,
                               conn_interval_max,
                               conn_latency,
                               supervision_timeout,
                               2 * (conn_interval_min - 1),
                               2 * (conn_interval_max - 1));
    return ret;

}

int ble_central_at_cmd_bond_information(int argc, char **argv)
{
	int ret = 0;
	if(strcmp(argv[1],"CLEAR") == 0) {
		le_bond_clear_all_keys();
	}else if(strcmp(argv[1], "INFO") == 0){
    	u8 i;
    	T_LE_KEY_ENTRY *p_entry;
    	for (i = 0; i < bond_storage_num; i++) {
        	p_entry = le_find_key_entry_by_idx(i);
        	if (p_entry != NULL) {
            BLE_PRINT("bond_dev[%d]: bd 0x%02x%02x%02x%02x%02x%02x, addr_type %d, flags 0x%x\r\n",
                            p_entry->idx,
                            p_entry->remote_bd.addr[5],
                            p_entry->remote_bd.addr[4],
                            p_entry->remote_bd.addr[3],
                            p_entry->remote_bd.addr[2],
                            p_entry->remote_bd.addr[1],
                            p_entry->remote_bd.addr[0],
                            p_entry->remote_bd.remote_bd_type,
                            p_entry->flags);
        	}
    	}
	}else{
		return -1;
	}

	return 0;
}

#define UUID_16_LEN		2
#define UUID_128_LEN	16
int ble_central_at_cmd_get(int argc, char **argv) 
{
	u8 conn_id;
	u8 uuid_type;
	u8 uuid[16] = {0};
	u16 start_handle;
	u16 end_handle;
	u16 uuid16;

	conn_id = atoi(argv[2]);
	
	if(strcmp(argv[1],"ALL") == 0) {
		gcs_all_primary_srv_discovery(conn_id);
	}else if(strcmp(argv[1], "SRV") == 0){
		if(argc !=5){
			BLE_PRINT("ERROR:input parameter error!\n\r");
			return -1;
		}
		uuid_type = atoi(argv[3]);
		if(uuid_type == 0){
			if (strlen(argv[4]) != 2 * UUID_16_LEN){
				BLE_PRINT("ERROR:uuid length error!\n\r");
				return -1;
			}
			hex_str_to_bd_addr(strlen(argv[4]), argv[4], uuid);
			uuid16 = (uuid[1]<<8) | uuid[0];
        	gcs_by_uuid_srv_discovery(conn_id, uuid16);
		}else if(uuid_type == 1){
			if (strlen(argv[4]) != 2 * UUID_128_LEN){
				BLE_PRINT("ERROR:uuid length error!\n\r");
				return -1;
			}
			hex_str_to_bd_addr(strlen(argv[4]), argv[4], uuid);
			gcs_by_uuid128_srv_discovery(conn_id, uuid);
		}else{
			BLE_PRINT("ERROR:uuid type error!\n\r");
			return -1;
		}
	}	else if(strcmp(argv[1], "CHARDIS") == 0){
		if(argc !=5 ){
			BLE_PRINT("ERROR:input parameter error!\n\r");
			return -1;
		}
		start_handle = hex_str_to_int(strlen(argv[3]), argv[3]);
		end_handle = hex_str_to_int(strlen(argv[4]), argv[4]);
		gcs_all_char_discovery(conn_id, start_handle, end_handle);
	}else if(strcmp(argv[1], "CHARUUID") == 0){
		if(argc != 7){
			BLE_PRINT("ERROR:input parameter error!\n\r");
			return -1;
		}
		
		start_handle = hex_str_to_int(strlen(argv[3]), argv[3]);
		end_handle = hex_str_to_int(strlen(argv[4]), argv[4]);
		uuid_type = atoi(argv[5]);
		if(uuid_type == 0){
			if (strlen(argv[6]) != 2 * UUID_16_LEN){
				BLE_PRINT("ERROR:uuid length error!\n\r");
				return -1;
			}
			hex_str_to_bd_addr(strlen(argv[6]), argv[6], uuid);
			uuid16 = (uuid[1]<<8) | uuid[0];
        	gcs_by_uuid_char_discovery(conn_id, start_handle, end_handle, uuid16);
		}else if(uuid_type == 1){
			if (strlen(argv[6]) != 2 * UUID_128_LEN){
				BLE_PRINT("ERROR:uuid length error!\n\r");
				return -1;
			}
			hex_str_to_bd_addr(strlen(argv[6]), argv[6], uuid);
			gcs_by_uuid128_char_discovery(conn_id, start_handle, end_handle, uuid);
		}else{
			BLE_PRINT("ERROR:uuid type error!\n\r");
			return -1;
		}

	}else if(strcmp(argv[1], "CHARDDIS") == 0){
		if(argc != 5)
			return -1;
		start_handle = hex_str_to_int(strlen(argv[3]), argv[3]);
		end_handle = hex_str_to_int(strlen(argv[4]), argv[4]);
		gcs_all_char_descriptor_discovery(conn_id, start_handle, end_handle);
	}
	return 0;

}

int ble_central_at_cmd_scan(int argc, char **argv)
{
	T_GAP_CAUSE cause;
    u8 scan_filter_policy = GAP_SCAN_FILTER_ANY;
    uint8_t scan_filter_duplicate = GAP_SCAN_FILTER_DUPLICATE_ENABLE;
	u8 scan_enable = 0;
	static u8 scan_is_processing = 0;

	if((argc != 0) && (argc != 2) && (argc != 3) && (argc !=4)){
		BLE_PRINT("[BLE_PRINT] ERROR: input parameter error!\n\r");
		return 0;
	}
	
	if(argc >= 2){
		scan_enable = atoi(*(argv+1));
		if(scan_enable == 1){
			if(argc == 3)
				scan_filter_policy = atoi(*(argv+2));
			else if (argc == 4){
				scan_filter_policy = atoi(*(argv+2));
				scan_filter_duplicate = atoi(*(argv+3));
			}

		}
		
	}

	if(scan_enable){
		if(scan_is_processing){
			BLE_PRINT("[BLE_PRINT] Scan is processing, please stop it first\n\r");
		}else{
			BLE_PRINT("[BLE_PRINT] Start scan\n\r");
			BLE_PRINT("[BLE_PRINT] scan_filter_policy = %d, scan_filter_duplicate=%d\n\r", scan_filter_policy, scan_filter_duplicate);
			scan_is_processing = 1;
		    le_scan_set_param(GAP_PARAM_SCAN_FILTER_POLICY, sizeof(scan_filter_policy),
		                      &scan_filter_policy);
		    le_scan_set_param(GAP_PARAM_SCAN_FILTER_DUPLICATES, sizeof(scan_filter_duplicate),
		                      &scan_filter_duplicate);
		    cause = le_scan_start();
		}
	}else{
		if(scan_is_processing){
			BLE_PRINT("[BLE_PRINT] Start stop scan\n\r");
			le_scan_stop();
			scan_is_processing = 0;
		}else
			BLE_PRINT("[BLE_PRINT] There is no scan\n\r");
	}
}

int ble_central_at_cmd_auth(int argc, char **argv)
{
	u8 conn_id;
	u32 passcode;
    T_GAP_CFM_CAUSE confirm;
	int ret;

	if(strcmp(argv[1],"SEND") == 0) {
		if(argc != 3){
			BLE_PRINT("ERROR:input parameter error!\n\r");
		}
		conn_id = atoi(argv[2]);
		le_bond_pair(conn_id);
	}else if(strcmp(argv[1], "KEY") == 0){
		if(argc !=4){
			BLE_PRINT("ERROR:input parameter error!\n\r");
			return -1;
		}
		conn_id = atoi(argv[2]);
		if(strlen(argv[3]) > 6){
			confirm = GAP_CFM_CAUSE_REJECT;
		}else{
			confirm = GAP_CFM_CAUSE_ACCEPT;
		}

		passcode = dec_str_to_int(strlen(argv[3]), argv[3]);
		le_bond_passkey_input_confirm(conn_id, passcode, confirm);
	}else if(strcmp(argv[1], "MODE") == 0){	
		u8  auth_pair_mode = GAP_PAIRING_MODE_PAIRABLE;
	    u16 auth_flags = GAP_AUTHEN_BIT_BONDING_FLAG;
	    u8  auth_io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
	    u8  oob_enable = false;
	    u8  auth_sec_req_enable = false;
	    u16 auth_sec_req_flags = GAP_AUTHEN_BIT_BONDING_FLAG;

		if (argc >= 3) {
	        auth_flags = hex_str_to_int(strlen(argv[2]), argv[2]);
	        auth_sec_req_flags =  auth_flags;
    	}		
	    if (argc >= 4) {
	        auth_io_cap = atoi(argv[3]);
	    }
		if (argc >= 5) {
	        auth_sec_req_enable = atoi(argv[4]);
	    }
		if (argc >= 6) {
	        oob_enable = atoi(argv[5]);
	    }

		gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(auth_pair_mode), &auth_pair_mode);
		gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(auth_flags), &auth_flags);
    	gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(auth_io_cap), &auth_io_cap);
    	gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(uint8_t), &oob_enable);
    	le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enable), &auth_sec_req_enable);
    	le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags),
                      &auth_sec_req_flags);
    	ret = gap_set_pairable_mode();
	}else{
		BLE_PRINT("ERROR:input parameter error!\n\r");
		return -1;
	}
	
	return ret;
}

int ble_central_at_cmd_send_userconf(int argc, char **argv)
{
    u8 conn_id;
	u8 conf;
	int ret;
	T_GAP_CFM_CAUSE confirm;

	conn_id = atoi(argv[1]);
	conf = atoi(argv[2]);   
    if (conf == 0){
        confirm = GAP_CFM_CAUSE_REJECT;
    }else{
		confirm = GAP_CFM_CAUSE_ACCEPT;
	}
	
    ret = le_bond_user_confirm(conn_id, confirm);
    return ret;
}

int ble_central_at_cmd_read(int argc, char **argv)
{
	u8 conn_id;
	u16 handle;
	u16 start_handle;
	u16 end_handle;
	u8 uuid_type;
	u8 uuid[16] = {0};
	u16 uuid16;
	
	conn_id = atoi(argv[1]);
	
	if(argc == 3){
		handle =  hex_str_to_int(strlen(argv[2]), argv[2]);
		gcs_attr_read(conn_id, handle);
	}
	
	if(argc == 6){
		start_handle = hex_str_to_int(strlen(argv[2]), argv[2]);
		end_handle = hex_str_to_int(strlen(argv[3]), argv[3]);
		uuid_type = atoi(argv[4]);
		BLE_PRINT("conn_id = %d, start_handle = 0x%x, end_handle = 0x%x, uuid_type = %d\n\r", conn_id, start_handle, end_handle, uuid_type);
		if(uuid_type == 0){
			if (strlen(argv[5]) != 2 * UUID_16_LEN){
				BLE_PRINT("ERROR:uuid length error!\n\r");
				return -1;
			}
			hex_str_to_bd_addr(strlen(argv[5]), argv[5], uuid);
			uuid16 = uuid[1]<<8 | uuid[0];
			BLE_PRINT("uuid16 = 0x%x\n\r", uuid16);
        	gcs_attr_read_using_uuid16(conn_id, start_handle, end_handle, uuid16);
		
		}else if(uuid_type == 1){
			if (strlen(argv[5]) != 2 * UUID_128_LEN){
				BLE_PRINT("ERROR:uuid length error!\n\r");
				return -1;
			}
			hex_str_to_bd_addr(strlen(argv[5]), argv[5], uuid);
			gcs_attr_read_using_uuid128(conn_id, start_handle, end_handle, uuid);
		}else{
			BLE_PRINT("ERROR:uuid type error!\n\r");
			return -1;
		}		

	}	
	return 0;
}

int ble_central_at_cmd_write(int argc, char **argv)
{
	u8 conn_id;
	u8 write_type;
	u16 handle;
	u16 length;
	u8 data[512];

	conn_id = atoi(argv[1]);
	write_type = atoi(argv[2]);
	handle = hex_str_to_int(strlen(argv[3]), argv[3]);
	length = hex_str_to_int(strlen(argv[4]), argv[4]);

	if(argc == 6){	
		hex_str_to_bd_addr(strlen(argv[5]), argv[5], data);
	}

	gcs_attr_write(conn_id, (T_GATT_WRITE_TYPE)write_type, handle,
                                     length, data);
	return 0;
}
