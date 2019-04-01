#ifndef _PACKETS_H
#define _PACKETS_H

#ifdef __LINUX_UB2__
#include <stdint.h>
#endif

#include "joylink.h"
#include "joylink_json.h"
#include "joylink_security.h"

/**
 * brief: 
 *
 * @Param: scan
 *
 * @Returns: 
 */
int 
joylink_packet_lan_scan_rsp(DevScan_t *scan);

/**
 * brief: 
 *
 * @Returns: 
 */
int 
joylink_packet_lan_write_key_rsp(int code, char *msg);

/**
 * brief: 
 *
 * @Returns: 
 */
int 
joylink_packet_server_auth_rsp(void);

/**
 * brief: 
 *
 * @Returns: 
 */
int 
joylink_packet_server_hb_req(void);

/**
 * brief: 
 *
 * @Returns: 
 */
int 
joylink_packet_server_upload_req(void);

/**
 * brief: 
 *
 * @Param: otaOrder
 *
 * @Returns: 
 */
int 
joylink_packet_server_ota_order_rsp(JLOtaOrder_t *otaOrder);

/**
 * brief: 
 *
 * @Param: otaUpload
 *
 * @Returns: 
 */
int 
joylink_package_ota_upload_req(JLOtaUpload_t *otaUpload);

/**
 * brief: 
 *
 * @Param: data
 * @Param: max
 * @Param: ctrl
 *
 * @Returns: 
 */
int 
joylink_packet_script_ctrl_rsp(char *data, int max, JLContrl_t *ctrl);

#endif
