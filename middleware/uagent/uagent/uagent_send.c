/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include "uagent/uagent.h"
#include "uagent_inner_api.h"
#include "aos/kernel.h"
#include "aos/errno.h"
#include "k_config.h"
static int pub_info(const unsigned short len, void *str, const bool trans_guarantee);

static char           msg_pub[UAGENT_INFO_PAYLOAD_SIZE];
static char          *msg_delay_pub = NULL;
static unsigned short msg_delay_pub_len = 0;
static unsigned long  out_msg_index = 0;

static int delay_send(const ua_mod_t mod, const ua_func_t func,
    const unsigned short len, const void *data, const ua_send_policy_t policy)
{
    int rc = -1;
    bool send_directly = false;
    bool append_new = false;
    char *out = (char*)data;
    UAGENT_DEBUG("call %s policy 0x%x len %d, old %d\n",__func__,policy, len,msg_delay_pub_len);
    if(NULL==msg_delay_pub){
        msg_delay_pub = (char*)aos_malloc(UAGENT_INFO_PAYLOAD_SIZE);
        if(NULL==msg_delay_pub){
            UAGENT_ERR("[uA]allock memory for delay send fail\n");
            return -ENOMEM;
        }
    }
    if (POLICY_SET(policy, send_policy_delay_dump)) {
        if (msg_delay_pub_len != 0) {
            out = msg_delay_pub;
            send_directly = true;
        }
    } else {
        if (0 == msg_delay_pub_len) {

            if (len < UAGENT_INFO_NEAR_FULL) {
                memset(msg_delay_pub, 0, UAGENT_INFO_PAYLOAD_SIZE);
                append_new = true;
            } else {
                /* send out right now */
                send_directly = true;
            }
        } else {
            if ((len + msg_delay_pub_len) >= UAGENT_INFO_PAYLOAD_SIZE) {
                send_directly = true;
                out = msg_delay_pub;
                append_new = true;

            } else if ((len + msg_delay_pub_len) >= UAGENT_INFO_NEAR_FULL) {
                send_directly = true;
                out = msg_delay_pub;
                memcpy(&msg_delay_pub[msg_delay_pub_len], data, len);
                msg_delay_pub_len += len;
            } else {
                append_new = true;
            }
        }
    }

    if (send_directly) {
        UAGENT_DEBUG("call %s will pop out (%d)\n",__func__,msg_delay_pub_len);
        if (msg_delay_pub_len != 0) {
            /* stop the alarm */
            stop_monitor_delay_out();
        }
        int msg_len = snprintf(msg_pub, sizeof(msg_pub), UAGENT_INFO_STR, out_msg_index++,
            uagent_service_attr.dn, uagent_service_attr.to_console, mod, func, out);
        if (msg_len > 0 && msg_len < UAGENT_INFO_PAYLOAD_SIZE) {
            rc = pub_info(msg_len, msg_pub, POLICY_SET(policy,send_policy_trans_guarantee));
        } else {
            UAGENT_ERR("[uA]miss pub as payload over flow %d\n", msg_len);
        }
        msg_delay_pub_len = 0;
        memset(msg_delay_pub, 0, UAGENT_INFO_PAYLOAD_SIZE);
    }
    if (append_new) {
        if (msg_delay_pub_len == 0) {
            UAGENT_DEBUG("call %s create new alarm (%d)\n",__func__,len);
            /* 0->1 create alarm */
            start_monitor_delay_out();
        }
        UAGENT_DEBUG("call %s save len %d, old %d\n",__func__, len,msg_delay_pub_len);
        memcpy(&msg_delay_pub[msg_delay_pub_len], data, len);
        msg_delay_pub_len += len;
        rc = 0;
    }
    return rc;
}

int uagent_send(const ua_mod_t mod, const ua_func_t type,
                const unsigned short len, const void *info_str,
                const ua_send_policy_t policy);
{
    int rc = -1;
    if (UAGENT_INITED_FINISH) {
        rc = -ENOMEM;
        if (len < UAGENT_INFO_PAYLOAD_SIZE) {

            if (uagent_get_mutex()) {

                if (POLICY_SET(policy,send_policy_delay) && !POLICY_SET(policy,send_policy_object)) {
                    rc = delay_send(mod, func, len, data,policy);

                }else{
                    char* format_str = NULL;
                    if (POLICY_SET(policy,send_policy_object)) {
                        format_str = UAGENT_INFO_OBJ;
                    } else {
                        format_str = UAGENT_INFO_STR;
                    }
                    int msg_len = snprintf(msg_pub, sizeof(msg_pub), format_str, out_msg_index++,
                        uagent_service_attr.dn, uagent_service_attr.to_console, mod, func, (char*)data);
                    if (msg_len > 0 && msg_len < UAGENT_INFO_PAYLOAD_SIZE) {
                        rc = pub_info(msg_len, msg_pub, POLICY_SET(policy,send_policy_trans_guarantee));
                    } else {
                        UAGENT_ERR("[uA]miss pub as payload over flow %d\n", msg_len);
                    }
                }

                uagent_release_mutex();
            }
        }
    }else{
        UAGENT_ERR("[uA]uagent send may fail from 0x%x 0x%x as not inited",mod,func);
    }
    return rc;
}

int uagent_ack(const service_agent_rlt_t rlt, const unsigned long id,
               const ua_mod_t mod, const ua_func_t func, const void *data)
{
    int rc = -1;
    if (UAGENT_INITED_FINISH) {
        int msg_len = 0;
        char msg_pub_ack[128];
        if (SERVICE_AGENT_OK == rlt) {
            msg_len = snprintf(msg_pub_ack, sizeof(msg_pub_ack), UAGENT_INFO_ACK_OK, id,
            uagent_service_attr.dn, uagent_service_attr.to_console, mod, func,
                SERVICE_AGENT_OK);
        } else {
            msg_len = snprintf(msg_pub_ack, sizeof(msg_pub_ack), UAGENT_INFO_ACK_FAIL, id,
            uagent_service_attr.dn, uagent_service_attr.to_console, mod, func,
                SERVICE_AGENT_FAIL,
                (NULL == data) ? "unknown reason" : (char*)data);
        }
        UAGENT_DEBUG("call %s ack %d mod 0x%x func 0x%x id %ld\n", __func__,rlt, mod, func,id);
        if (msg_len > 0 && msg_len < UAGENT_INFO_PAYLOAD_SIZE) {
            rc = pub_info(msg_len, msg_pub_ack, 0);
        } else {
            UAGENT_ERR("[uA]miss pub as payload over flow %d\n", msg_len);
        }
    }
    return rc;
}

static int pub_info(const unsigned short len, void *str, const bool trans_guarantee)
{
    int rc = -EINVAL;
    if (len > 0 && NULL != str) {
        UAGENT_DEBUG("call %s Prepare pub (%d)\n", __func__, len);
        rc = uagent_ext_comm_publish(len, str, trans_guarantee);
    }
    return rc;
}
