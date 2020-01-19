

#ifndef UMESH_PACK_H_
#define UMESH_PACK_H_

#include "frame.h"

int umesh_send_frame_discovery(umesh_state_t *state);

int umesh_send_frame_discovery_resp(umesh_state_t *state, const uint8_t *dst);

int umesh_send_frame_join_request(umesh_state_t *state);

int umesh_send_frame_join_resp(umesh_state_t *state, const uint8_t *dst);

int umesh_send_frame_join_finish(umesh_state_t *state, const uint8_t *dst);

int umesh_send_frame_heart_beat(umesh_state_t *state);

int umesh_send_frame_data(umesh_state_t *state, const uint8_t *ra, const uint8_t *sa, const uint8_t *da,
                          const uint8_t *data, int data_len);
int umesh_send_frame_zero_req(umesh_state_t *state, const uint8_t *da);

int umesh_send_frame_zero_resp(umesh_state_t *state, const uint8_t *da);

int umesh_send_frame_zero_finish(umesh_state_t *state, const uint8_t *da);

#endif /* UMESH_PACK_H_ */
