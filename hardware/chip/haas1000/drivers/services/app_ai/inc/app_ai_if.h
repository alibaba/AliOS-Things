#ifndef AI_IF_H_
#define AI_IF_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */


#ifdef __cplusplus
extern "C" {
#endif

//the ai type now use
typedef enum {
    AI_SPEC_INIT,
    AI_SPEC_GSOUND,
    AI_SPEC_AMA,
    AI_SPEC_BES,
    AI_SPEC_BAIDU,
    AI_SPEC_TENCENT,
    AI_SPEC_ALI,
    AI_SPEC_COUNT,
} AI_SPEC_TYPE_E;

typedef struct{
    uint8_t connPathType;
    uint8_t connPathState;
    uint8_t connBtState;
    uint8_t connBdAddr[6];
}AI_CONNECTION_STATE_T;

const char *ai_spec_type2str(AI_SPEC_TYPE_E ai_spec);

/*---------------------------------------------------------------------------
 *            ai_if_tws_role_switch_prepare
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    AI interface controlled tws role switch API
 *
 * Parameters:
 *    wait_ms - time delay before role switch started for AI module
 *
 * Return:
 *    true - need delay
 *    false - not need delay
 */
uint32_t ai_if_tws_role_switch_prepare(uint32_t *wait_ms);

/*---------------------------------------------------------------------------
 *            ai_if_slave_reuqest_master_do_roleswitch
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    slave request master to role switch API for AI module
 *
 * Parameters:
 *    void
 *
 * Return:
 *    void
 */
void ai_if_slave_reuqest_master_do_roleswitch(void);

/*---------------------------------------------------------------------------
 *            ai_if_master_role_switch_prepare
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    master role switch API for AI module
 *
 * Parameters:
 *    void
 *
 * Return:
 *    void
 */
void ai_if_master_role_switch_prepare(void);

/*---------------------------------------------------------------------------
 *            ai_if_role_switch_prepare_done
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    master role switch prepare done, slave receive a masegge from master
 *
 * Parameters:
 *    void
 *
 * Return:
 *    void
 */
void ai_if_role_switch_prepare_done(void);

/*---------------------------------------------------------------------------
 *            ai_if_tws_role_switch
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    role switch API for AI module
 *
 * Parameters:
 *    void
 *
 * Return:
 *    void
 */
void ai_if_tws_role_swtich(void);

/*---------------------------------------------------------------------------
 *            ai_if_tws_sync_init
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    tws related environment initialization for AI interface
 *
 * Parameters:
 *    void
 *
 * Return:
 *    void
 */
void ai_if_tws_sync_init(void);

/*---------------------------------------------------------------------------
 *            app_ai_key_event_handle
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    for AI handle key event
 *
 * Parameters:
 *    void *param1
 *    uint32_t param2
 *
 * Return:
 *    void
 */
void app_ai_key_event_handle(void *param1, uint32_t param2);


void ai_if_send_cmd_to_peer(uint8_t *p_buff, uint16_t length);
void ai_if_rev_peer_cmd_hanlder(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
bool ai_if_is_ai_stream_mic_open(void);

#ifdef __cplusplus
    }
#endif


#endif //AI_IF_H_

