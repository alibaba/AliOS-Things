#ifndef UMESH_H_
#define UMESH_H_


void *umesh_network_init();
int umesh_recv_ip_data(struct umesh_state *state, const uint8_t *data, uint16_t data_len, uint8_t *dst_addr);
void umesh_network_deinit(void *handle);

#endif /* UMESH_SCHEDULE_H_ */
