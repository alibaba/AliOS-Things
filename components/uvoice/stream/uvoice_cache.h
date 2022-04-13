/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef __NET_CACHE_H__
#define __NET_CACHE_H__


enum cache_type {
	CACHE_NONE = 0,
	CACHE_FILE,
	CACHE_MEM,
};

enum cache_event {
	CACHE_EV_CACHE_CPLT = 0,
	CACHE_EV_SEEK_DONE,
	CACHE_EV_READ_BLOCK,
};

struct cache_file {
	os_file_t fp;
	uint32_t wr_pos;
	uint32_t rd_pos;
	uint32_t rd_len;
	uint8_t rd_waiting:1;
	uint8_t rd_start:1;
	os_sem_t rd_sem;
};

struct cache_mem {
	uvoice_ringbuff_t rb;
	uint8_t *pool;
	int pool_size;
	int wr_len;
	int rd_len;
	uint8_t wr_waiting:1;
	uint8_t rd_waiting:1;
	uint8_t rd_start:1;
	os_sem_t wr_sem;
	os_sem_t rd_sem;
};

typedef struct {
	uint8_t *buffer;
	int32_t buffer_size;
	int32_t head_data_size;
	int32_t offset;
	cache_config_t cache_config;
	uint8_t cache_running:1;
	uint8_t cache_stop:1;
	uint8_t cache_cplt:1;
	uint8_t cplt_waiting:1;
	uint8_t rebuild:1;
	uint8_t download:1;
	uint8_t sequence:1;
	char filename[64];
	int32_t content_length;
	int32_t load_length;
	int32_t seek_offset;
	void *cache;
	int (*cache_load)(void *priv, uint8_t *buffer, int nbytes);
	int (*cache_read)(void *priv, uint8_t *buffer, int nbytes);
	int (*event)(void *priv, enum cache_event event, void *arg);
	void *priv;
	os_sem_t cplt_sem;
	os_mutex_t lock;
	os_task_t cache_task;
} net_cache_t;

int net_get_cacheinfo(net_cache_t *nc, cache_info_t *info);
int net_get_mediainfo(net_cache_t *nc, media_info_t *info, media_format_t format);
int net_cache_reset(net_cache_t *nc);
int net_cache_seek(net_cache_t *nc, int offset);
int net_cache_pause(net_cache_t *nc);
int net_cache_resume(net_cache_t *nc);
int net_cache_start(net_cache_t *nc);
int net_cache_stop(net_cache_t *nc);
net_cache_t *net_cache_create(int read_size, cache_config_t *config, bool sequence);
int net_cache_release(net_cache_t *nc);


#endif /* __NET_CACHE_H__ */