#ifndef __GENIE_TRANSPORT_H__
#define __GENIE_TRANSPORT_H__

#include "misc/dlist.h"

#define RECV_MSG_TID_QUEUE_SIZE 5
#define GENIE_TRANSPORT_DEDUPLICATE_DURATION (6000) //unit ms

#define TMALL_GENIE_UADDR_START 0x0001
#define TMALL_GENIE_UADDR_END 0x0010

#define GENIE_TRANSPORT_TID_MIN (0x80)
#define GENIE_TRANSPORT_TID_MAX (0xBF)

#define GENIE_TRANSPORT_DEFAULT_TTL (3)

#ifdef CONFIG_GENIE_MESH_GLP
#define GENIE_TRANSPORT_DEFAULT_RETRY_COUNT (5) //Total six times
#else
#define GENIE_TRANSPORT_DEFAULT_RETRY_COUNT (2) //Total three times
#endif

#define GENIE_TRANSPORT_EACH_PDU_TIMEOUT (600) //Unit ms
#define GENIE_TRANSPORT_EACH_PDU_SIZE (12)

#ifndef CONFIG_VENDOR_SEND_MSG_MAX
#define CONFIG_VENDOR_SEND_MSG_MAX 8
#endif

typedef struct _genie_transport_tid_queue_s
{
    uint8_t tid;
    uint8_t elemid;
    uint16_t addr;
    uint32_t time;
} genie_transport_tid_queue_t;

typedef enum
{
    MESH_SUCCESS = 0,
    MESH_TID_REPEAT,
    MESH_ANALYZE_SIZE_ERROR,
    MESH_ANALYZE_ARGS_ERROR,
    MESH_SET_TRANSTION_ERROR,
} E_MESH_ERROR_TYPE;

/**
 * p_elem: pointer to the element which the messsage want to be sent to
 * retry: retry counts before desired confirm message received
 * * fill negative value if retransmission is not necessary
 * * fill positive value if retransmission is needed
 * * * will be round in this scope - [VENDOR_MODEL_MSG_DFT_RETRY_TIMES, VENDOR_MODEL_MSG_MAX_RETRY_TIMES]
 * retry_period: wait for retry_period before retransmit the message, in unit of ms
 * opid: hightest byte in Opcode defined in the vendor model spec designed by Alibaba IoT Group
 * * e.g. for Vendor Message Attr Get message, Opcode is 0xD001A8, corresponding opid is 0xD0
 * * refer to the marco named VENDOR_OP_ATTR_xxxx
 * tid:
 * * if the message is with type of VENDOR_OP_ATTR_CONFIME or VENDOR_OP_ATTR_CONFIME_TG,
 * * tid should be filled with the replied message's tid
 * len: payload length
 * data: pointer to the vendor message's payload
 * */
typedef enum _transport_result_e
{
    SEND_RESULT_SUCCESS = 0,
    SEND_RESULT_TIMEOUT
} transport_result_e;

typedef int (*transport_result_cb)(transport_result_e result);

typedef struct _genie_transport_model_param_s
{
    struct bt_mesh_elem *p_elem;
    struct bt_mesh_model *p_model; //It is current model
    transport_result_cb result_cb;
    uint8_t retry;
    uint8_t opid;
    uint8_t tid;
    uint16_t dst_addr;
    uint16_t retry_period;
    uint16_t len;
    uint8_t *data;
} genie_transport_model_param_t;

typedef struct _genie_transport_node_s
{
    sys_dnode_t node;
    uint8_t left_retry;
    long long timeout;
    genie_transport_model_param_t msg;
} genie_transport_node_t;

typedef struct _genie_transport_payload_s
{
    uint8_t opid;
    uint8_t tid;
    uint8_t retry_cnt;
    uint16_t dst_addr;
    uint8_t *p_payload;
    uint16_t payload_len;
    transport_result_cb result_cb;
} genie_transport_payload_param_t;

/**
 * @brief check whether there is this tid in record, and record it if not.
 * @param[in] src_addr indicates the device which hold this tid.
 * @param[in] tid
 * @return MESH_SUCCESS means successed, otherwise failed.
 */
E_MESH_ERROR_TYPE
genie_transport_check_tid(uint16_t src_addr, uint8_t tid, uint8_t elem_id);

void genie_transport_src_addr_set(uint16_t src_addr);

uint16_t genie_transport_src_addr_get();

/**
 * @brief send the vendor model message
 * @param[in] p_vendor_msg refers to the message to be sent
 * @return 0 for success; negative for failure
 */
int genie_transport_send_model(genie_transport_model_param_t *p_vendor_msg);

int genie_transport_send_payload(genie_transport_payload_param_t *payload_param);

uint8_t genie_transport_gen_tid(void);

uint8_t genie_transport_get_seg_count(uint16_t msg_len);

bool genie_transport_tx_in_progress(void);

/** @def genie_transport_ack
 *
 *  @brief check received vendor message's tid
 *
 *  @param pointer to send_list, tid of the received vendor model message
 *
 *  @return 0 for success; negative for failure
 */
int genie_transport_ack(uint8_t tid);

void genie_transport_init(void);

#endif
