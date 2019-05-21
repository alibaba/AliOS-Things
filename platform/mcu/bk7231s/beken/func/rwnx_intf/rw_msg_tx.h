#ifndef _RW_MSG_TX_H_
#define _RW_MSG_TX_H_

#include "include.h"
#include "scan.h"
#include "mac.h"
#include "rw_pub.h"

#define BEACON_INTERVAL                       (100)

struct scan_cancel_req
{
};

extern void mt_reset(void);
extern void mt_me_config(void);
extern void mt_channel_config(void);
extern void mt_start(void);
extern void mt_add_if(UINT32 type);
extern void mt_apm_start(void);
extern void mt_key_add(KEY_PARAM_T *key_param);
extern void mt_key_del(KEY_PARAM_T *key_param);
extern void mt_set_ctrl_port(void);
extern void mt_sm_connect(CONNECT_PARAM_T *sme);
extern void mt_scan_start(SCAN_PARAM_T *scan_param);
#endif // _RW_MSG_TX_H_
// eof

