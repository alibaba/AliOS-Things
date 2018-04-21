/******************************************************************************
 * @file   : maclib_task.h
 * @brief  : WiFi MACLib task header file
 * @version: V1.0
 * @date   : 6. May 2017
 *
 * @note   :
 *
 ******************************************************************************/

#ifndef _MACLIB_TASK_H_
#define _MACLIB_TASK_H_

#define MAC_LIB_MAX_FLEN    1536

/**
 * Enums
 */
typedef enum {
    MACLIB_MSG_EVNT_HNDL,
    MACLIB_MSG_WLAND_XMIT_PKT,
    MACLIB_MSG_LWIP_XMIT_PKT
} MACLIB_MSG_TYPE_T;

/**
 * Structures
 */
typedef struct {
    MACLIB_MSG_TYPE_T type;
    r_void *msg;
    r_s32 is_free;
} maclib_msg_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     : MACLib main task [MACLib api]
 * @param[in] : pvParameters(pointer to enet data)
 * @param[out]:
 * @return    :
 */
extern r_void maclib_task(r_void *pvParameters);
extern r_void event_handle_cb(r_u32 event);
extern r_void mltask_sleep_cb(r_void);
extern r_void mltask_wakeup_cb(r_void);

#if 0
/**
 * @brief     : pkt transmiting interface [MACLib api]
 * @param[in] : p(wland use memory pointer, lwip use pbuf)
 * @param[out]:
 * @return    : 0 for succsess, others for error
 */
extern int maclib_xmit_pkt(void *p, MACLIB_MSG_TYPE_T type);

/**
 * @brief     : pkt receiving callback interface [user implemented]
 * @param[in] : buf(pointer to maclib_buf_t structure)
 * @param[out]:
 * @return    : 0 for succsess, others for error
 */
extern int wland_recv_pkt(unsigned char *buf);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _MACLIB_TASK_H_ */
