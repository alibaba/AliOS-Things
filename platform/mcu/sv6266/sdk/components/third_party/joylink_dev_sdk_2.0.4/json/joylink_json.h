#ifndef __JOYLINK_JSON__
#define __JOYLINK_JSON__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdio.h>
#include "joylink.h"

/**
 * brief: 
 *
 * @Param: scan
 * @Param: pMsg
 *
 * @Returns: 
 */
int 
joylink_parse_scan(DevScan_t *scan, const char * pMsg);

/**
 * brief: 
 *
 * @Param: retMsg
 * @Param: retCode
 * @Param: scan_type
 * @Param: dv
 *
 * @Returns: 
 */
char *
joylink_package_scan(const char *retMsg, const int retCode, 
        int scan_type, JLDevice_t *dv);

/**
 * brief: 
 *
 * @Param: de
 * @Param: pMsg
 *
 * @Returns: 
 */
int 
joylink_parse_lan_write_key(DevEnable_t *de, const char * pMsg);

/**
 * brief: 
 *
 * @Param: pMsg
 * @Param: out_num
 *
 * @Returns: 
 */
JLDevInfo_t *
joylink_parse_sub_add(const uint8_t* pMsg, int *out_num);

/**
 * brief: 
 *
 * @Param: de
 * @Param: num
 * @Param: out
 *
 * @Returns: 
 */
int 
joylink_packet_sub_add_rsp(const JLDevInfo_t *de, int num, char *out);

/**
 * brief: 
 *
 * @Param: pMsg
 * @Param: dev
 *
 * @Returns: 
 */
int
joylink_parse_sub_auth(const uint8_t* pMsg, JLDevInfo_t *dev);

/**
 * brief: 
 *
 * @Param: de
 * @Param: out
 *
 * @Returns: 
 */
int 
joylink_packet_sub_auth_rsp(const JLDevInfo_t *de, char *out);

/**
 * brief: 
 *
 * @Param: sdev
 * @Param: count
 *
 * @Returns: 
 */
char * 
joylink_package_subdev(JLDevInfo_t *sdev, int count);

/**
 * brief: 
 *
 * @Param: feedid
 * @Param: pMsg
 *
 * @Returns: 
 */
int 
joylink_parse_json_ctrl(char *feedid, const char * pMsg);

/**
 * brief: 
 *
 * @Param: otaOrder
 * @Param: pMsg
 *
 * @Returns: 
 */
int 
joylink_parse_server_ota_order_req(JLOtaOrder_t *otaOrder, const char * pMsg);

/**
 * brief: 
 *
 * @Param: otaUpload
 * @Param: pMsg
 *
 * @Returns: 
 */
int
joylink_parse_server_ota_upload_req(JLOtaUploadRsp_t* otaUpload, const char* pMsg);

/**
 * brief: 
 *
 * @Param: otaUpload
 *
 * @Returns: 
 */
char *
joylink_package_ota_upload(JLOtaUpload_t *otaUpload);

/**
 * brief: 
 *
 * @Param: pMsg
 * @Param: num
 *
 * @Returns: 
 */
char ** 
joylink_parse_ids(const char * pMsg, int32_t *num);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
