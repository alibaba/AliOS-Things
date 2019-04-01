#include <stdio.h>

#ifdef __LINUX_UB2__
#include <stdint.h>
#endif

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "joylink.h"
#include "joylink_utils.h"
#include "joylink_packets.h"
#include "joylink_crypt.h"
#include "joylink_dev_timer.h"
#include "joylink_json.h"

/**
 * brief: 
 *
 * @Param: req
 *
 * @Returns: 
 */
static E_JLRetCode_t
joylink_dev_time_task_add(JLTimeTaskReq_t *req)
{
    int ret = E_RET_OK;

    //FIXME: must to do
    int i;
    for(i=0; i<req->task_num; i++)
    {
        log_debug("joylink_dev_time_task_add req->tasks[%d].mode =%d, task_num = %d\n", i, req->tasks[i].mode, req->task_num);
        switch(req->tasks[i].mode)
        {
            case BASE_MODE:
                log_info("BASE_MODE\r\n");
            break;
            case PERIOD_MODE:
                log_info("PERIOD_MODE\r\n");
            break;

            case CYCLE_MODE:
                log_info("CYCLE_MODE\r\n");
            break;
            case COUNTDOWN_MODE:
                log_info("COUNTDOWN_MODE\r\n");
            break;
            
            default:
            ;      
        }

    }
    return ret;
}

/**
 * brief: 
 *
 * @Param: src
 *
 * @Returns: 
 */
static int
joylink_time_task_get_biz_code(char *src)
{
    int pret_code = *(int*)(src + 4);
    return pret_code; 
}

/**
 * brief: 
 *
 * @Returns: 
 */
//-------BIZ_CODE_TIME_TASK_CHECK_REQ ------ 
static E_JLBOOL_t
joylink_dev_is_support_time_task()
{
    /**
     *FIXME:must to do
     */
    int ret = E_RET_TRUE;
    return ret;
}

/**
 * brief: 
 *
 * @Returns: 
 */
static int
joylink_packet_is_support_time_task_rsp()
{
    int32_t support = E_RET_ERROR;

    JLTimeTaskRsp_t rsp;

    rsp.ch.timestamp =  xTaskGetTickCount();//time(NULL);
    rsp.ch.biz_code = BIZ_CODE_TIME_TASK_CHECK_RSP;
    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));

    memcpy(_g_pdev->dev_detail, &rsp, 12);

    if(E_RET_TRUE == joylink_dev_is_support_time_task()){
        support = 0;
    }

    memcpy(_g_pdev->dev_detail + 12, &support, 4);

    return 16;
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 *
 * @Returns: 
 */
static int
joylink_proc_is_support_time_task(char *src, int src_len)
{
    if(NULL == src){
        return E_RET_ERROR;
    }
    int ret = -1;
    int len;

    log_debug("Check is support time task\n");

    ret = joylink_packet_is_support_time_task_rsp();

    len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN, 
            PT_TIME_TASK, _g_pdev->jlp.sessionKey, 
            (uint8_t *)_g_pdev->dev_detail, ret);

    if(len > 0 &&  len < JL_MAX_PACKET_LEN){
        ret = send(_g_pdev->server_socket, _g_pdev->send_buff, len, 0);
        if(ret < 0){
            log_error("send error ret:%d", ret);
        }
        log_info("send to server len:%d:ret:%d\n", len, ret);
    }else{
        log_error("packet error ret:%d", ret);
    }

    return ret;
}

//-------BIZ_CODE_TIME_TASK_ADD_REQ ------ 
/**
 * brief: 
 *
 * @Param: req
 * @Param: ret_code
 *
 * @Returns: 
 */
int
joylink_packet_time_task_add_rsp(JLTimeTaskReq_t *req, int32_t ret_code)
{
    JLTimeTaskRsp_t rsp;
    rsp.ch.timestamp =  xTaskGetTickCount();//time(NULL);
    rsp.ch.biz_code = BIZ_CODE_TIME_TASK_ADD_RSP;
    rsp.ch.serial = req->ch.serial;
    rsp.code = ret_code;

    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));

    memcpy(_g_pdev->dev_detail, &rsp, sizeof(rsp));

    return sizeof(rsp);
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 *
 * @Returns: 
 */
int
joylink_proc_time_task_add(char *src, int src_len)
{
    if(NULL == src){
        return E_RET_ERROR;
    }
    int ret = -1;
    int len;
    int ret_code = E_RET_ERROR;

    JLTimeTaskReq_t *req = (JLTimeTaskReq_t *)src;

    log_debug("add task, sizeof(JLTimeTask_t) = %d\r\n", sizeof(JLTimeTask_t));
    log_info("add task, sizeof(JLTimeTask_u) = %d\r\n", sizeof(JLTimeTask_u));
    log_debug("add task, sizeof(JLTimeTask_base_t) = %d\r\n", sizeof(JLTimeTask_base_t));
    log_info("add task, sizeof(JLTimeTask_period_t) = %d\r\n", sizeof(JLTimeTask_period_t));
    log_debug("add task, sizeof(JLTimeTask_cycle_t) = %d\r\n", sizeof(JLTimeTask_cycle_t));
    log_info("add task, sizeof(JLTimeTask_countdown_t) = %d\r\n", sizeof(JLTimeTask_countdown_t));

    if(E_RET_OK == joylink_dev_time_task_add(req)){
        ret_code = E_RET_OK;
    }
     
    ret = joylink_packet_time_task_add_rsp(req, ret_code);
     log_info("add task 3, \r\n");
    len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN, 
            PT_TIME_TASK, _g_pdev->jlp.sessionKey, 
            (uint8_t *)_g_pdev->dev_detail, ret);

    if(len > 0 &&  len < JL_MAX_PACKET_LEN){
        ret = send(_g_pdev->server_socket, _g_pdev->send_buff, len, 0);
        if(ret < 0){
            log_error("send error ret:%d", ret);
        }
        log_info("send to server len:%d:ret:%d\n", len, ret);
    }else{
        log_error("packet error ret:%d", ret);
    }

    return ret;
}

//-------BIZ_CODE_TIME_TASK_UPD_REQ ------ 
/**
 * brief: 
 *
 * @Param: req
 *
 * @Returns: 
 */
static E_JLRetCode_t
joylink_dev_time_task_update(JLTimeTaskReq_t *req)
{
    //FIXME: must to do
    int i;
    for(i=0; i<req->task_num; i++)
    {
        switch(req->tasks[i].mode)
        {
            case BASE_MODE:
                //need hanfeng's code
                log_info("BASE_MODE\r\n");
            break;


            case PERIOD_MODE:
                log_info("PERIOD_MODE\r\n");
                //need hanfeng's code
            break;
            case CYCLE_MODE:
                log_info("CYCLE_MODE\r\n");
                //need hanfeng's code
            break;
            case COUNTDOWN_MODE:
                log_info("COUNTDOWN_MODE\r\n");
                //need hanfeng's code
            break;
            
            default:
            ;      
        }

    }
    int ret = E_RET_OK;


    return ret;
}

/**
 * brief: 
 *
 * @Param: req
 * @Param: ret_code
 *
 * @Returns: 
 */
int
joylink_packet_time_task_update_rsp(JLTimeTaskReq_t *req, int32_t ret_code)
{
    JLTimeTaskRsp_t rsp;
    rsp.ch.timestamp =  xTaskGetTickCount();//time(NULL);
    rsp.ch.biz_code = BIZ_CODE_TIME_TASK_UPD_RSP;
    rsp.ch.serial = req->ch.serial;
    rsp.code = ret_code;

    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));

    memcpy(_g_pdev->dev_detail, &rsp, sizeof(rsp));

    return sizeof(rsp);
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 *
 * @Returns: 
 */
int
joylink_proc_time_task_update(char *src, int src_len)
{
    if(NULL == src){
        return E_RET_ERROR;
    }
    int ret = -1;
    int len;
    int ret_code = E_RET_ERROR;
    JLTimeTaskReq_t *req;

    req = (JLTimeTaskReq_t *)src;

    log_debug("Check is support time task\n");

    if(E_RET_OK == joylink_dev_time_task_update(req)){
        ret_code = E_RET_OK;
    }

    ret = joylink_packet_time_task_update_rsp(req, ret_code);

    len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN, 
            PT_TIME_TASK, _g_pdev->jlp.sessionKey, 
            (uint8_t *)_g_pdev->dev_detail, ret);

    if(len > 0 &&  len < JL_MAX_PACKET_LEN){
        ret = send(_g_pdev->server_socket, _g_pdev->send_buff, len, 0);
        if(ret < 0){
            log_error("send error ret:%d", ret);
        }
        log_info("send to server len:%d:ret:%d\n", len, ret);
    }else{
        log_error("packet error ret:%d", ret);
    }

    return ret;
}

//-------BIZ_CODE_TIME_TASK_DEL_REQ ------ 
/**
 * brief: 
 *
 * @Param: req
 *
 * @Returns: 
 */
static E_JLRetCode_t
joylink_dev_time_task_del(JLTimeTaskIdsReq_t *req)
{
    //FIXME: 
    int ret = E_RET_OK;

    return ret;
}

/**
 * brief: 
 *
 * @Param: req
 * @Param: ret_code
 *
 * @Returns: 
 */
static int
joylink_packet_time_task_del_rsp(JLTimeTaskIdsReq_t *req, int32_t ret_code)
{
    JLTimeTaskIdsRsp_t rsp;
    rsp.ch.timestamp =  xTaskGetTickCount();//time(NULL);
    rsp.ch.biz_code = BIZ_CODE_TIME_TASK_DEL_RSP;
    rsp.code = ret_code;
    rsp.ch.serial = req->ch.serial;
    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));

    //FIXME: how to transfer ids?
    memcpy(_g_pdev->dev_detail, &rsp, sizeof(JLTimeTaskIdsRsp_t));

    return sizeof(JLTimeTaskIdsRsp_t);
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 *
 * @Returns: 
 */
static int
joylink_proc_time_task_del(char *src, int src_len)
{
    if(NULL == src){
        return E_RET_ERROR;
    }
    int ret = -1;
    int len;
    int ret_code = E_RET_ERROR;

    JLTimeTaskIdsReq_t *req = (JLTimeTaskIdsReq_t *)src;

    log_debug("del time task\n");

    if(E_RET_OK == joylink_dev_time_task_del(req)){
        ret_code = E_RET_OK;
    }

    ret = joylink_packet_time_task_del_rsp(req, ret_code);
    len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN, 
            PT_TIME_TASK, _g_pdev->jlp.sessionKey, 
            (uint8_t *)_g_pdev->dev_detail, ret);

    if(len > 0 &&  len < JL_MAX_PACKET_LEN){
        ret = send(_g_pdev->server_socket, _g_pdev->send_buff, len, 0);
        if(ret < 0){
            log_error("send error ret:%d", ret);
        }
        log_info("send to server len:%d:ret:%d\n", len, ret);
    }else{
        log_error("packet error ret:%d", ret);
    }
    return ret;
}

//-------BIZ_CODE_TIME_TASK_STOP_REQ ------ 
/**
 * brief: 
 *
 * @Param: req
 *
 * @Returns: 
 */
static E_JLRetCode_t
joylink_dev_time_task_stop(JLTimeTaskIdsReq_t *req)
{
    /**
     *FIXME:must to do
     */
    int ret = E_RET_OK;

    /**
     *debug
     */

    if(NULL == req->ids){
        return ret;
    }

    log_debug("req task num: 3 %d", req->ids_num);

    return ret;
}

/**
 * brief: 
 *
 * @Param: req
 * @Param: ret_code
 *
 * @Returns: 
 */
static int
joylink_packet_time_task_stop_rsp(JLTimeTaskIdsReq_t *req, int32_t ret_code)
{
    JLTimeTaskIdsRsp_t rsp;
    rsp.ch.timestamp =  xTaskGetTickCount();//time(NULL);
    rsp.ch.biz_code = BIZ_CODE_TIME_TASK_STOP_RSP;
    rsp.ch.serial = req->ch.serial;
    rsp.code = ret_code;
    rsp.ids_num = 0;

    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));

    memcpy(_g_pdev->dev_detail, &rsp, sizeof(rsp));

    return sizeof(rsp);
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 *
 * @Returns: 
 */
static int
joylink_proc_time_task_stop(char *src, int src_len)
{
    if(NULL == src){
        return E_RET_ERROR;
    }
    int ret = -1;
    int len;
    int ret_code = E_RET_ERROR;

    JLTimeTaskIdsReq_t *req = (JLTimeTaskIdsReq_t *)src;

    log_debug("stop time task\n");

    if(E_RET_OK == joylink_dev_time_task_stop(req)){
        ret_code = E_RET_OK;
    }

    ret = joylink_packet_time_task_stop_rsp(req, ret_code);

    len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN, 
            PT_TIME_TASK, _g_pdev->jlp.sessionKey, 
            (uint8_t *)_g_pdev->dev_detail, ret);

    if(len > 0 &&  len < JL_MAX_PACKET_LEN){
        ret = send(_g_pdev->server_socket, _g_pdev->send_buff, len, 0);
        if(ret < 0){
            log_error("send error ret:%d", ret);
        }
        log_info("send to server len:%d:ret:%d\n", len, ret);
    }else{
        log_error("packet error ret:%d", ret);
    }

    return ret;
}

//-------BIZ_CODE_TIME_TASK_RESTART_REQ ------ 
/**
 * brief: 
 *
 * @Param: req
 *
 * @Returns: 
 */
static E_JLRetCode_t
joylink_dev_time_task_restart(JLTimeTaskIdsReq_t *req)
{
    /**
     *FIXME:must to do
     */
    int ret = E_RET_OK;

    if(NULL == req->ids){
        return ret;
    }

    log_debug("req task num 2:%d", req->ids_num);

    return ret;
}

/**
 * brief: 
 *
 * @Param: req
 * @Param: ret_code
 *
 * @Returns: 
 */
static int
joylink_packet_time_task_restart_rsp(JLTimeTaskIdsReq_t *req, int32_t ret_code)
{
    JLTimeTaskIdsRsp_t rsp;

    rsp.ch.timestamp =  xTaskGetTickCount();//time(NULL);
    rsp.ch.biz_code = BIZ_CODE_TIME_TASK_RESTART_RSP;
    rsp.ch.serial = req->ch.serial;
    rsp.code = 0;
    rsp.ids_num = 0;

    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));

    memcpy(_g_pdev->dev_detail, &rsp, sizeof(rsp));

    return sizeof(rsp);
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 *
 * @Returns: 
 */
static int
joylink_proc_time_task_restart(char *src, int src_len)
{
    if(NULL == src){
        return E_RET_ERROR;
    }
    int ret = -1;
    int len;
    int ret_code = E_RET_ERROR;
    JLTimeTaskIdsReq_t *req = (JLTimeTaskIdsReq_t *)src;

    log_debug("restart time task\n");

    if(E_RET_OK == joylink_dev_time_task_restart(req)){
        ret_code = E_RET_OK;
    }

    ret = joylink_packet_time_task_restart_rsp(req, ret_code);

    len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN, 
            PT_TIME_TASK, _g_pdev->jlp.sessionKey, 
            (uint8_t *)_g_pdev->dev_detail, ret);

    if(len > 0 &&  len < JL_MAX_PACKET_LEN){
        ret = send(_g_pdev->server_socket, _g_pdev->send_buff, len, 0);
        if(ret < 0){
            log_error("send error ret:%d", ret);
        }
        log_info("send to server len:%d:ret:%d\n", len, ret);
    }else{
        log_error("packet error ret:%d", ret);
    }

    return ret;
}

/**
 * brief: 
 *
 * @Param: req
 * @Param: task_detail
 *
 * @Returns: 
 */
int 
joylink_dev_time_task_get(JLTimeTaskIdsReq_t *req, JLTimeTaskReq_t* task_detail)
{
    //FIXME: must to do
    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));
    task_detail->ch.timestamp =  xTaskGetTickCount();//time(NULL);
    task_detail->ch.biz_code = BIZ_CODE_TIME_TASK_GET_RSP;
    task_detail->ch.serial = req->ch.serial;
    task_detail->task_num = 0;

    return sizeof(JLTimeTaskReq_t);
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 *
 * @Returns: 
 */
static int
joylink_proc_time_task_get(char *src, int src_len)
{
    if(NULL == src){
        return E_RET_ERROR;
    }
    int ret = -1;
    int len;

    JLTimeTaskIdsReq_t *req = (JLTimeTaskIdsReq_t *)src;

    log_debug("get time task \n");

    ret = joylink_dev_time_task_get(req, (JLTimeTaskReq_t*)(_g_pdev->dev_detail));

    len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN, 
            PT_TIME_TASK, _g_pdev->jlp.sessionKey, 
            (uint8_t *)_g_pdev->dev_detail, ret);

    if(len > 0 &&  len < JL_MAX_PACKET_LEN){
        ret = send(_g_pdev->server_socket, _g_pdev->send_buff, len, 0);
        if(ret < 0){
            log_error("send error ret:%d", ret);
        }
        log_info("send to server len:%d:ret:%d\n", len, ret);
    }else{
        log_error("packet error ret:%d", ret);
    }

    return ret;
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 *
 * @Returns: 
 */
static int
joylink_proc_time_task_report_result_rsp(char *src, int src_len)
{
    JLTimeTaskResultReportRsp_t *rsp = (JLTimeTaskResultReportRsp_t *)src;
    rsp+=4;
    //FIXME: logic:
    //(1)judge if rsp equals to send
    //(2)if does not receive rsp, then create a new send.
    //
    int ret = -1;
    return ret;
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 *
 * @Returns: 
 */
static int
joylink_proc_time_task_report_new_task_rsp(char *src, int src_len)
{
    JLTimeTaskNewReportRsp_t *rsp = (JLTimeTaskNewReportRsp_t *)src;
    rsp+=4;
    //FIXME: must to do
    int ret = -1;
    return ret;
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 *
 * @Returns: 
 */
static int
joylink_proc_time_task_report_del_task_rsp(char *src, int src_len)
{
    JLTimeTaskIdsRsp_t *rsp = (JLTimeTaskIdsRsp_t *)src;
    rsp+=4;
    //FIXME: must to do
    int ret = -1;
    return ret;
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 *
 * @Returns: 
 */
static int
joylink_proc_time_task_report_update_task_rsp(char *src, int src_len)
{
    JLTimeTaskRsp_t *rsp = (JLTimeTaskRsp_t *)src;
    rsp+=4;
    //FIXME: must to do
    int ret = -1;
    return ret;
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 *
 * @Returns: 
 */
static int
joylink_proc_time_task_report_snapshot_task_rsp(char *src, int src_len)
{

    JLTimeTaskIdsRsp_t *rsp = (JLTimeTaskIdsRsp_t *)src;
    rsp+=4;
    //FIXME: must to do
    int ret = -1;
    return ret;
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 *
 * @Returns: 
 */
int
joylink_proc_time_task(char *src, int src_len)
{
    if(NULL == src){
        return E_RET_ERROR;
    }
    
    {
        int i=0;
        for(i=0; i<src_len; i++)
        {
             if(i%16 == 0)
             {
                printf("\r\n");
             }
             printf("%02x ", *(unsigned char*)(src+i));

        }

    }
    switch (joylink_time_task_get_biz_code(src)){
        case BIZ_CODE_TIME_TASK_CHECK_REQ: 
            joylink_proc_is_support_time_task(src, src_len);
            break;
        case BIZ_CODE_TIME_TASK_ADD_REQ: 
            joylink_proc_time_task_add(src, src_len);
            break;
        case BIZ_CODE_TIME_TASK_DEL_REQ: 
            joylink_proc_time_task_del(src, src_len);
            break;
        case BIZ_CODE_TIME_TASK_UPD_REQ: 
            joylink_proc_time_task_update(src, src_len);
            break;          
        case BIZ_CODE_TIME_TASK_GET_REQ: 
            joylink_proc_time_task_get(src, src_len);
            break;
        case BIZ_CODE_TIME_TASK_STOP_REQ: 
            joylink_proc_time_task_stop(src, src_len);
            break;
        case BIZ_CODE_TIME_TASK_RESTART_REQ: 
            joylink_proc_time_task_restart(src, src_len);
            break;

        case BIZ_CODE_TIME_TASK_REPORT_RESULT_RSP: 
            joylink_proc_time_task_report_result_rsp(src, src_len);
            break;
        case BIZ_CODE_TIME_TASK_REPORT_NEW_TASK_RSP: 
            joylink_proc_time_task_report_new_task_rsp(src, src_len);
            break;          
        case BIZ_CODE_TIME_TASK_REPORT_DEL_TASK_RSP: 
            joylink_proc_time_task_report_del_task_rsp(src, src_len);
            break;
        case BIZ_CODE_TIME_TASK_REPORT_UPDATE_TASK_RSP: 
            joylink_proc_time_task_report_update_task_rsp(src, src_len);
            break;
        case BIZ_CODE_TIME_TASK_REPORT_SNAPSHOT_TASK_RSP: 
            joylink_proc_time_task_report_snapshot_task_rsp(src, src_len);
            break;

        default:
            log_error("No found biz_code!\n");
            break;
    }

    return E_RET_OK;
}

//------- BIZ_CODE_TIME_TASK_REPORT_NEW_TASK_REQ =1098 ------
/**
 * brief: 
 *
 * @Param: task_req
 * @Param: task_num
 *
 * @Returns: 
 */
int
joylink_time_task_new_task_report_req(JLTimeTaskReq_t *task_req, int task_num)
{
    int ret = E_RET_OK;
    int len;

    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));

    memcpy(_g_pdev->dev_detail, task_req, 20+sizeof(JLTimeTask_u)*task_num); 

    len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN, 
            PT_TIME_TASK, _g_pdev->jlp.sessionKey, 
            (uint8_t *)_g_pdev->dev_detail, 20+sizeof(JLTimeTask_u)*task_num);

    if(len > 0 &&  len < JL_MAX_PACKET_LEN){
        ret = send(_g_pdev->server_socket, _g_pdev->send_buff, len, 0);
        if(ret < 0){
            log_error("send error ret:%d", ret);
        }
        log_info("send to server len:%d:ret:%d\n", len, ret);
    }else{
        log_error("packet error ret:%d", ret);
    }

    return ret;
}

/**
 * brief: 
 *
 * @Param: task_req
 *
 * @Returns: 
 */
int
joylink_time_task_del_task_report_req(JLTimeTaskIdsReq_t *task_req)
{
    int ret = E_RET_OK;
    int len;

    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));

    memcpy(_g_pdev->dev_detail, task_req, 20+4*task_req->ids_num); 

    len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN, 
            PT_TIME_TASK, _g_pdev->jlp.sessionKey, 
            (uint8_t *)_g_pdev->dev_detail, 20+4*task_req->ids_num);

    if(len > 0 &&  len < JL_MAX_PACKET_LEN){
        ret = send(_g_pdev->server_socket, _g_pdev->send_buff, len, 0);
        if(ret < 0){
            log_error("send error ret:%d", ret);
        }
        log_info("send to server len:%d:ret:%d\n", len, ret);
    }else{
        log_error("packet error ret:%d", ret);
    }

    return ret;
}

/**
 * brief: 
 *
 * @Param: task_req
 * @Param: task_num
 *
 * @Returns: 
 */
int
joylink_time_task_update_task_report_req(JLTimeTaskReq_t *task_req, int task_num)
{
    int ret = E_RET_OK;
    int len;

    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));

    memcpy(_g_pdev->dev_detail, task_req, 20+sizeof(JLTimeTask_u)*task_num); 

    len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN, 
            PT_TIME_TASK, _g_pdev->jlp.sessionKey, 
            (uint8_t *)_g_pdev->dev_detail, 20+sizeof(JLTimeTask_u)*task_num);

    if(len > 0 &&  len < JL_MAX_PACKET_LEN){
        ret = send(_g_pdev->server_socket, _g_pdev->send_buff, len, 0);
        if(ret < 0){
            log_error("send error ret:%d", ret);
        }
        log_info("send to server len:%d:ret:%d\n", len, ret);
    }else{
        log_error("packet error ret:%d", ret);
    }

    return ret;
}

/**
 * brief: 
 *
 * @Param: task_req
 * @Param: task_num
 *
 * @Returns: 
 */
int
joylink_time_task_snapshot_task_report_req(JLTimeTaskReq_t *task_req, int task_num)
{
    int ret = E_RET_OK;
    int len;

    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));

    memcpy(_g_pdev->dev_detail, task_req, 20+sizeof(JLTimeTask_u)*task_num); 

    len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN, 
            PT_TIME_TASK, _g_pdev->jlp.sessionKey, 
            (uint8_t *)_g_pdev->dev_detail, 20+sizeof(JLTimeTask_u)*task_num);

    if(len > 0 &&  len < JL_MAX_PACKET_LEN){
        ret = send(_g_pdev->server_socket, _g_pdev->send_buff, len, 0);
        if(ret < 0){
            log_error("send error ret:%d", ret);
        }
        log_info("send to server len:%d:ret:%d\n", len, ret);
    }else{
        log_error("packet error ret:%d", ret);
    }

    return ret;
}
