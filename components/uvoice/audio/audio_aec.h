/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef __AUDIO_AEC_H__
#define __AUDIO_AEC_H__


struct aec_handler {
	void *aec_inst;
	uint8_t bypass:1;
	uint8_t start:1;
	uint8_t rd_waiting:1;
	uint8_t wr_waiting:1;
	uint8_t *refer_pool;
	uint8_t *echo_buffer;
	int refer_pool_size;
	int echo_buffer_size;
	int wr_len;
	int rd_len;
	os_mutex_t lock;
	os_sem_t wr_sem;
	os_sem_t rd_sem;
	uvoice_ringbuff_t rb;
};


int audio_aec_refer_conserve(struct aec_handler *aec, uint8_t *buffer, int nbytes);
int audio_aec_process(struct aec_handler *aec, uint8_t *buffer, int nbytes);
int audio_aec_init(struct in_stream *in);
int audio_aec_deinit(struct in_stream *in);


#endif /* __AUDIO_AEC_H__ */
