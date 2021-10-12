#ifndef __SIG_MODEL_EVENT_H__
#define __SIG_MODEL_EVENT_H__

#define SIG_MODEL_INDICATE_PAYLOAD_MAX_LEN (20)

typedef enum _indicate_flag
{
    SIG_MODEL_INDICATE_GEN_ONOFF,
    SIG_MODEL_INDICATE_GEN_LIGHTNESS,
    SIG_MODEL_INDICATE_GEN_CTL,
    SIG_MODEL_INDICATE_GEN_SCENE,
    SIG_MODEL_INDICATE_FLAGS
} sig_model_indicate_flag_e;

typedef enum
{
    SIG_MODEL_EVT_NONE = 0,
    SIG_MODEL_EVT_ANALYZE_MSG,
    SIG_MODEL_EVT_TIME_OUT,
    SIG_MODEL_EVT_DOWN_MSG,
    SIG_MODEL_EVT_ACTION_DONE,
    SIG_MODEL_EVT_INDICATE,

    SIG_MODEL_EVT_DELAY_START = 10,
    SIG_MODEL_EVT_DELAY_END,

#ifdef CONFIG_MESH_MODEL_TRANS
    SIG_MODEL_EVT_TRANS_START,
    SIG_MODEL_EVT_TRANS_CYCLE,
    SIG_MODEL_EVT_TRANS_END,
#endif

    SIG_MODEL_EVT_GENERIC_MESG = 20,
} sig_model_event_e;

void sig_model_event_set_indicate(int indicate);

void sig_model_event(sig_model_event_e event, void *p_arg);

#endif
