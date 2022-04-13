/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef __HTTP_LOAD_H__
#define __HTTP_LOAD_H__


#ifdef VOICELOUDER_APP
#define HTTP_URL_LEN				1024
#else
#define HTTP_URL_LEN				512
#endif
#define HTTP_HOSTNAME_LEN			HTTP_URL_LEN

#define HTTP_STAT_SUCCESS				200
#define HTTP_CODE_BREAKPOINT			206
#define HTTP_CODE_PERM_REDIRECT			301
#define HTTP_CODE_TEMP_REDIRECT			302

#define HTTP_READ_BUFF_SIZE				1024


typedef struct {
	char *source;
	char filename[64];
	net_cache_t *nc;
} http_dload_t;

typedef struct {
	char base[HTTP_URL_LEN];
	os_file_t fp;
	char *content;
	char *pos;
	char *sub;
	int len;
	int *seg_offset;
	char seg_desc[64];
	uint8_t live_stream:1;
	int seg_begin;
	int seg_index;
	int seg_count;
	int seg_current;
	int duration;
	os_mutex_t lock;
} hls_t;

typedef struct {
	uint16_t port;
	int sock;
	int http_status;
	uint16_t https:1;
	uint16_t start:1;
	uint16_t chunked:1;
	uint16_t redirect:1;
	uint16_t download:1;
	uint16_t cache_on:1;
	uint16_t seeking:1;
	uint16_t deep_pause:1;
	uint16_t force_restart:1;
	uint16_t read_paused:1;
	int content_length;
	int breakpoint_pos;
	int load_pos;
	int seek_offset;
	long long pause_time;
	uint32_t host_addr;
	char host_name[HTTP_HOSTNAME_LEN];
	char uri[HTTP_URL_LEN];
	char url[HTTP_URL_LEN];
	char redirect_url[HTTP_URL_LEN];
	http_dload_t dload;
	net_cache_t *nc;
	hls_t *hls;
	os_task_t download_task;
	void *priv;
} http_loader_t;


#endif /* __HTTP_LOAD_H__ */
