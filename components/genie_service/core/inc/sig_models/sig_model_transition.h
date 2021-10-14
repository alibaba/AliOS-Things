#ifndef __SIG_MODEL_TRANSITION_H__
#define __SIG_MODEL_TRANSITION_H__

#define SIG_MODEL_TRANSITION_INTERVAL 20

#define TRANSITION_TIME_UNIT_1 (100)            //Unit:ms
#define TRANSITION_TIME_UNIT_2 (1000)           //Unit:ms
#define TRANSITION_TIME_UNIT_3 (10 * 1000)      //Unit:ms
#define TRANSITION_TIME_UNIT_4 (10 * 60 * 1000) //Unit:ms

#define TRANSITION_TIME_UNIT_BIT(unit_value) ((unit_value) << 6)
#define TRANSITION_TIME_VALUE_MIN (0)
#define TRANSITION_TIME_VALUE_MAX (0x3E)
#define TRANSITION_TIME_VALUE_INVALID (0x3F)

#define TRANSITION_TIME_VALUE_MASK (0x3F)

#define TRANSITION_DEFAULT_VALUE (0x41)
#define DELAY_DEFAULT_VAULE (100)
#define DELAY_TIME_UNIT (5)

void sig_model_transition_timer_stop(sig_model_element_state_t *p_elem);

bt_u32_t sig_model_transition_get_transition_time(uint8_t byte);

uint8_t sig_model_transition_get_remain_time_byte(sig_model_state_t *p_state, bool is_ack);

void sig_model_transition_state_reset(sig_model_element_state_t *p_elem);

uint8_t sig_model_transition_update(sig_model_element_state_t *p_elem);

#endif
