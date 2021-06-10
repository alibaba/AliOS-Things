/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "uvoice_os.h"
#include "uvoice_types.h"
#include "uvoice_mlist.h"

#include "uvoice_config.h"


#define SOURCE_LIST_INDEX_KEY		"mlist_index"


typedef struct {
	int count;
	int index;
	char source[128];
	os_mutex_t lock;
} mlist_t;

static mlist_t *g_src_list;

int mlist_source_show(void)
{
	mlist_t *src_list = g_src_list;
	char *listpath;
	os_file_t *list_fp;
	char file_name[128];
	int index = 0;
	char *ptr = NULL;
	int len;

	if (!src_list) {
		M_LOGE("src_list null !\n");
		return -1;
	}

	os_mutex_lock(src_list->lock, OS_WAIT_FOREVER);

	len = strlen(PLAYER_SOURCE_LIST_DIR) +
		strlen(PLAYER_SOURCE_LIST_NAME) + 4;
	listpath = snd_zalloc(len, AFM_EXTN);
	if (!listpath) {
		M_LOGE("alloc list path failed !\n");
		os_mutex_unlock(src_list->lock);
		return -1;
	}

	snprintf(listpath, len, "%s/%s",
		PLAYER_SOURCE_LIST_DIR, PLAYER_SOURCE_LIST_NAME);

	list_fp = os_fopen(listpath, "r");
	if (!list_fp) {
		M_LOGE("list not found, please scan\n");
		snd_free(listpath);
		os_mutex_unlock(src_list->lock);
		return -1;
	}

	while (!os_feof(list_fp)) {
		memset(file_name, 0, sizeof(file_name));
		os_fgets(file_name, sizeof(file_name), list_fp);

		if (file_name[strlen(file_name) - 1] == '\n')
			file_name[strlen(file_name) - 1] = '\0';

		ptr = strchr(file_name, '/');
		if (!ptr)
			continue;

		ptr += 1;
		index = atoi(ptr);

		ptr = strchr(ptr, '/');
		if (!ptr)
			continue;

		ptr += 1;
		M_LOGI("No.%d %s\n", index, ptr);
	}

	os_fclose(list_fp);
	snd_free(listpath);
	os_mutex_unlock(src_list->lock);
	return 0;
}

int mlist_source_scan(void)
{
	mlist_t *src_list = g_src_list;
	char *listpath;
	int listpath_len;
	os_file_t *list_fp;
	os_dir_t *dir;
	os_dirent_t *content = NULL;
	media_format_t format = MEDIA_FMT_UNKNOWN;
	int index = 0;
	bool curr_source_found = false;
	int ret;

	if (!src_list) {
		M_LOGE("src_list null !\n");
		return -1;
	}

	os_mutex_lock(src_list->lock, OS_WAIT_FOREVER);

	ret = access(PLAYER_SOURCE_LIST_DIR, F_OK);
	if (ret) {
		ret = os_mkdir(PLAYER_SOURCE_LIST_DIR);
		if (ret) {
			M_LOGE("create %s failed %d!\n",
				PLAYER_SOURCE_LIST_DIR, ret);
			os_mutex_unlock(src_list->lock);
			return -1;
		}
		M_LOGD("create dir: %s\n", PLAYER_SOURCE_LIST_DIR);
	}

	listpath_len = strlen(PLAYER_SOURCE_LIST_DIR) +
		strlen(PLAYER_SOURCE_LIST_NAME) + 2;
	listpath = snd_zalloc(listpath_len, AFM_EXTN);
	if (!listpath) {
		M_LOGE("alloc list path failed !\n");
		os_mutex_unlock(src_list->lock);
		return -1;
	}

	snprintf(listpath, listpath_len, "%s/%s",
		PLAYER_SOURCE_LIST_DIR, PLAYER_SOURCE_LIST_NAME);

	list_fp = os_fopen(listpath, "w+");
	if (!list_fp) {
		M_LOGE("open %s failed !\n", listpath);
		snd_free(listpath);
		os_mutex_unlock(src_list->lock);
		return -1;
	}

	dir = os_opendir(PLAYER_SOURCE_LIST_DIR);
	if (!dir) {
		M_LOGE("open %s failed !\n", PLAYER_SOURCE_LIST_DIR);
		os_fclose(list_fp);
		snd_free(listpath);
		os_mutex_unlock(src_list->lock);
		return -1;
	}

	while (1) {
		content = os_readdir(dir);
		if (!content)
			break;

#ifdef __os_alios_things__
		if (content->d_type & (AM_DIR | AM_HID | AM_ARC | AM_SYS))
			continue;
#else
		if (!(content->d_type & (DT_REG)))
			continue;
#endif

		format = MEDIA_FMT_UNKNOWN;
		format_parse_byname(content->d_name, &format);
		if (format == MEDIA_FMT_UNKNOWN)
			continue;

		if (strlen(content->d_name) + strlen("fs:") +
				strlen(PLAYER_SOURCE_LIST_DIR) > 126)
			continue;

		index++;
		if (os_fprintf(list_fp, "/%d/%s\n", index, content->d_name) < 0) {
			M_LOGE("write list failed !\n");
			index--;
			continue;
		}

		if (src_list->index > 0) {

			if (strlen(src_list->source) <=
				(strlen("fs:") + strlen(PLAYER_SOURCE_LIST_DIR) + 1)) {
				M_LOGE("source name invalid\n");
				continue;
			}

			if (!strncmp(content->d_name, (char *)src_list->source +
				strlen("fs:") + strlen(PLAYER_SOURCE_LIST_DIR) + 1,
				strlen(content->d_name))) {
				curr_source_found = true;
				if (src_list->index != index) {
					M_LOGI("update index %d\n", index);
					src_list->index = index;
					ret = os_kv_set(SOURCE_LIST_INDEX_KEY,
							&src_list->index,
							sizeof(src_list->index), 0);
					if (ret) {
						M_LOGW("kv set failed %d!\n", ret);
						ret = -1;
						goto __exit;
					}
				}
			}
		}

		//M_LOGD("No.%d %s\n", index, content->d_name);
	}

	if (src_list->count != index)
		src_list->count = index;
	M_LOGD("list count %d\n", src_list->count);

__exit:
	os_closedir(dir);
	os_fclose(list_fp);
	snd_free(listpath);

	if (!curr_source_found && src_list->index > 0) {
		M_LOGD("set default index\n");
		src_list->index = 0;
		os_mutex_unlock(src_list->lock);
		if (mlist_index_set(1)) {
			M_LOGE("set index failed !\n");
			ret = -1;
		}
	} else {
		os_mutex_unlock(src_list->lock);
	}

	return ret;
}

int mlist_source_get(int index, char *path, int len)
{
	mlist_t *src_list = g_src_list;
	char file_name[128];
	int listpath_len;
	char *listpath;
	os_file_t *list_fp;
	char *ptr = NULL;
	int file_index = 0;
	int ret = -1;

	if (!src_list) {
		M_LOGE("src_list null !\n");
		return -1;
	}

	if (!path) {
		M_LOGE("path null !\n");
		return -1;
	}

	if (len < 0) {
		M_LOGE("len %d invalid!\n", len);
		return -1;
	}

	os_mutex_lock(src_list->lock, OS_WAIT_FOREVER);

	if (index <= 0 || index > src_list->count) {
		M_LOGE("index %d count %d\n",
			index, src_list->count);
		os_mutex_unlock(src_list->lock);
		return -1;
	}

	listpath_len = strlen(PLAYER_SOURCE_LIST_DIR) +
		strlen(PLAYER_SOURCE_LIST_NAME) + 4;
	listpath = snd_zalloc(listpath_len, AFM_EXTN);
	if (!listpath) {
		M_LOGE("alloc list path failed !\n");
		os_mutex_unlock(src_list->lock);
		return -1;
	}

	snprintf(listpath, listpath_len, "%s/%s",
		PLAYER_SOURCE_LIST_DIR, PLAYER_SOURCE_LIST_NAME);

	list_fp = os_fopen(listpath, "r");
	if (!list_fp) {
		M_LOGE("open %s failed !\n", listpath);
		snd_free(listpath);
		os_mutex_unlock(src_list->lock);
		return -1;
	}

	while (!os_feof(list_fp)) {
		memset(file_name, 0, sizeof(file_name));
		os_fgets(file_name, sizeof(file_name), list_fp);

		if (file_name[strlen(file_name) - 1] == '\n')
			file_name[strlen(file_name) - 1] = '\0';

		ptr = strchr(file_name, '/');
		if (!ptr)
			continue;
		ptr += 1;

		file_index = atoi(ptr);
		if (file_index <= 0)
			continue;

		//M_LOGD("index %d, name: %s\n", file_index, file_name);

		if (file_index == index) {

			ptr = strchr(ptr, '/');
			if (!ptr)
				continue;
			ptr += 1;

			if (strchr(ptr, '/'))
				continue;

			if (strlen(ptr) + strlen("fs:") +
					strlen(PLAYER_SOURCE_LIST_DIR) + 2 > len) {
				M_LOGE("name %s length %d too long !\n",
					ptr, strlen(ptr));
				ret = -1;
				break;
			}

			snprintf(path, len, "fs:%s/%s",
					PLAYER_SOURCE_LIST_DIR, ptr);
			//M_LOGD("found /%d/%s\n", index, path);

			ret = 0;
			break;
		}
	}

	os_fclose(list_fp);
	snd_free(listpath);

	os_mutex_unlock(src_list->lock);
	return ret;
}

int mlist_source_del(int index)
{
	mlist_t *src_list = g_src_list;
	char file_path[128];
	int ret = 0;

	if (!src_list) {
		M_LOGE("src_list null !\n");
		return -1;
	}

	memset(file_path, 0, sizeof(file_path));

	if (mlist_source_get(index, file_path, sizeof(file_path))) {
		M_LOGE("get source name failed !\n");
		return -1;
	}

	if (strncmp(file_path, "fs:", strlen("fs:"))) {
		M_LOGE("source path invalid !\n");
		return -1;
	}

	if (unlink((char *)file_path + strlen("fs:"))) {
		M_LOGE("delete %s failed !\n", file_path);
		return -1;
	}

	os_mutex_lock(src_list->lock, OS_WAIT_FOREVER);

	if (src_list->index == index && index != 1) {
		M_LOGD("set default index\n");
		src_list->index = 1;
		ret = os_kv_set(SOURCE_LIST_INDEX_KEY,
			&src_list->index,
			sizeof(src_list->index), 0);
		if (ret) {
			M_LOGE("kv set failed %d!\n", ret);
			os_mutex_unlock(src_list->lock);
			return -1;
		}

		memset(src_list->source, 0, sizeof(src_list->source));
		os_mutex_unlock(src_list->lock);
		if (mlist_source_get(src_list->index,
				src_list->source,
				sizeof(src_list->source))) {
			M_LOGE("get source name failed !\n");
			return -1;
		}
	} else {
		os_mutex_unlock(src_list->lock);
	}

	if (mlist_source_scan()) {
		M_LOGE("scan source failed !\n");
		return -1;
	}

	M_LOGI("%s delete\n", file_path);
	return 0;
}

int mlist_index_get(int *index)
{
	mlist_t *src_list = g_src_list;
	int kv_len;
	int ret = 0;

	if (!src_list) {
		M_LOGE("src_list null !\n");
		return -1;
	}

	os_mutex_lock(src_list->lock, OS_WAIT_FOREVER);

	kv_len = sizeof(src_list->index);

	if (src_list->index == 0) {
		ret = os_kv_get(SOURCE_LIST_INDEX_KEY,
			&src_list->index, &kv_len);
		os_mutex_unlock(src_list->lock);
		if (ret == -ENOENT) {
			M_LOGD("set default index\n");
			if (mlist_index_set(1)) {
				M_LOGE("set source index failed !\n");
				return -1;
			}
		} else if (ret) {
			M_LOGE("kv get failed %d!\n", ret);
			return -1;
		}
	} else {
		os_mutex_unlock(src_list->lock);
	}

	if (strlen(src_list->source) <= 0) {
		if (mlist_source_get(src_list->index,
				src_list->source, sizeof(src_list->source))) {
			M_LOGE("get source name failed !\n");
			return -1;
		}
	}

	*index = src_list->index;
	M_LOGD("No.%d %s\n", *index,
		(char *)src_list->source + strlen("fs:") +
		strlen(PLAYER_SOURCE_LIST_DIR) + 1);

	return 0;
}

int mlist_index_set(int index)
{
	mlist_t *src_list = g_src_list;
	int kv_len;
	int ret = 0;

	if (!src_list) {
		M_LOGE("src_list null !\n");
		return -1;
	}

	os_mutex_lock(src_list->lock, OS_WAIT_FOREVER);

	kv_len = sizeof(src_list->index);

	if (src_list->index != index) {
		src_list->index = index;
		ret = os_kv_set(SOURCE_LIST_INDEX_KEY,
				&src_list->index,
				sizeof(src_list->index), 0);
		if (ret) {
			M_LOGE("kv set failed %d!\n", ret);
			os_mutex_unlock(src_list->lock);
			return -1;
		}

		M_LOGI("update source index %d\n", src_list->index);

		os_mutex_unlock(src_list->lock);

		memset(src_list->source, 0, sizeof(src_list->source));
		if (mlist_source_get(index, src_list->source,
				sizeof(src_list->source))) {
			M_LOGE("get source name failed !\n");
			return -1;
		}
	} else {
		os_mutex_unlock(src_list->lock);
	}

	return 0;
}

int mlist_init(void)
{
	mlist_t *src_list;
	int index = 0;

	src_list = snd_zalloc(sizeof(mlist_t), AFM_EXTN);
	if (!src_list) {
		M_LOGE("alloc src list failed !\n");
		return -1;
	}

	src_list->lock = os_mutex_new();

	g_src_list = src_list;

	if (mlist_source_scan()) {
		M_LOGE("scan source failed !\n");
		return -1;
	}

	if (mlist_index_get(&index)) {
		M_LOGE("get source index failed !\n");
		return -1;
	}

	M_LOGD("mlist init\n");
	return 0;
}

int mlist_deinit(void)
{
	mlist_t *src_list = g_src_list;
	if (!src_list) {
		M_LOGE("src_list null !\n");
		return -1;
	}

	os_mutex_free(src_list->lock);
	snd_free(src_list);
	g_src_list = NULL;

	M_LOGD("mlist free\n");
	return 0;
}

