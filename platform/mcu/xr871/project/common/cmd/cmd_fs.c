/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cmd_util.h"

#include "fs/fatfs/ff.h"

#define FS_TEST_LFN	1

#if FS_TEST_LFN
#define FS_TEST_LFN_STR	"longlongLongLongName"
#else
#define FS_TEST_LFN_STR	""
#endif

#define FS_TEST_DATA_BUF_SIZE	1024
#define FS_TEST_PATH_MAX_SIZE	256

#define CMD_FS_VOL_NAME	""

static FATFS cmd_fs;

int fs_test_create_file(const char *file_path, int file_size, uint8_t *buf, int buf_size)
{
	int len;
	uint32_t byte_w, left = file_size;
	FRESULT res;
	uint32_t start_tm;

	FIL *fp = cmd_malloc(sizeof(FIL));
	cmd_memset(fp, 0, sizeof(FIL));

	start_tm = OS_GetTicks();
	res = f_open(fp, file_path, FA_WRITE | FA_CREATE_ALWAYS);
	if (res != FR_OK) {
		CMD_ERR("create file %s failed, return %d\n", file_path, res);
		cmd_free(fp);
		return -1;
	}

	while (left > 0) {
		len = left > buf_size ? buf_size : left;
		res = f_write(fp, buf, len, &byte_w);
		if (res != FR_OK || len != byte_w) {
			CMD_ERR("write file failed, len %d, left %d, return %d, written %u\n",
			        len, left, res, byte_w);
			break;
		}
		left -= byte_w;
	}

	f_close(fp);
	cmd_free(fp);

	CMD_DBG("create file %s %s, size %d B (%d KB, %d MB), cost %u ms\n",
	        file_path, (left > 0) ? "failed" : "success",
	        file_size, file_size / 1024, file_size / 1024 / 1024,
	        OS_GetTicks() - start_tm);
	return (left > 0 ? -1 : 0);
}

int fs_test_create_dir(const char *dir_path, int file_num, int file_size_unit)
{
	int i;
	char *path;
	uint8_t *buf;
	FRESULT res;

	res = f_mkdir(dir_path);
	if (res != FR_OK) {
		CMD_ERR("create dir %s failed, return %d\n", dir_path, res);
		return -1;
	} else {
		CMD_DBG("create dir %s success\n", dir_path);
	}

	path = cmd_malloc(FS_TEST_PATH_MAX_SIZE);
	buf = cmd_malloc(FS_TEST_DATA_BUF_SIZE);
	for (i = 0; i < FS_TEST_DATA_BUF_SIZE; ++i) {
		buf[i] = '0' + (i % 10);
	}

	for (i = 0; i < file_num; ++i) {
		cmd_snprintf(path, FS_TEST_PATH_MAX_SIZE, "%s/file-%s-%02d.dat", dir_path, FS_TEST_LFN_STR, i);
		if (fs_test_create_file(path, i * file_size_unit, buf, FS_TEST_DATA_BUF_SIZE) < 0) {
			break;
		}
	}

	cmd_free(buf);
	cmd_free(path);
	return (i < file_num ? -1 : 0);
}

int fs_test_rm_dir(const char *dir_path, int do_rm_dir)
{
	int ret = -1;
	FRESULT res;
	DIR *dp = NULL;
	FILINFO *entry = NULL;
	char *path = NULL;

	path = cmd_malloc(FS_TEST_PATH_MAX_SIZE);

	dp = cmd_malloc(sizeof(DIR));
	cmd_memset(dp, 0, sizeof(DIR));

	entry = cmd_malloc(sizeof(FILINFO));
	cmd_memset(entry, 0, sizeof(FILINFO));

	res = f_opendir(dp, dir_path);
	if (res != FR_OK) {
		CMD_ERR("open dir %s failed, return %d\n", dir_path, res);
		goto out;
	}

	while (1) {
		res = f_readdir(dp, entry);
		if (res != FR_OK) {
			CMD_ERR("read dir %s failed, return %d\n", dir_path, res);
			break;
		}

		if (entry->fname[0] == 0) {
			CMD_DBG("delete dir %s files finish\n", dir_path);
			ret = 0;
			break;
		}

		cmd_snprintf(path, FS_TEST_PATH_MAX_SIZE, "%s/%s", dir_path, entry->fname);
		if (entry->fattrib & AM_DIR) {
			if (fs_test_rm_dir(path, 1) < 0) {
				break;
			}
		} else {
			res = f_unlink(path);
			CMD_DBG("delete file %s %s\n", path,
					(res != FR_OK) ? "failed" : "success");
			if (res != FR_OK) {
				break;
			}
		}
	}

	f_closedir(dp);
	if (ret == 0 && do_rm_dir) {
		res = f_unlink(dir_path);
		if (res != FR_OK) {
			CMD_ERR("delete dir %s failed, return %d\n", dir_path, res);
			ret = -1;
		} else {
			CMD_DBG("delete dir %s success\n", dir_path);
		}
	}

out:
	if (entry)
		cmd_free(entry);

	if (dp)
		cmd_free(dp);

	if (path)
		cmd_free(path);

	return ret;
}

int fs_test_cp_file(const char *file_dst, const char *file_src, uint8_t *buf, int len)
{
	int ret = -1;
	uint32_t byte_r, byte_w;
	FRESULT res;
	uint32_t start_tm;

	start_tm = OS_GetTicks();

	FIL *fp_r = cmd_malloc(sizeof(FIL));
	cmd_memset(fp_r, 0, sizeof(FIL));

	FIL *fp_w = cmd_malloc(sizeof(FIL));
	cmd_memset(fp_w, 0, sizeof(FIL));

	res = f_open(fp_r, file_src, FA_READ | FA_OPEN_EXISTING	);
	if (res != FR_OK) {
		CMD_ERR("open read file %s failed, return %d\n", file_src, res);
		goto out;
	}

	res = f_open(fp_w, file_dst, FA_WRITE | FA_CREATE_ALWAYS);
	if (res != FR_OK) {
		CMD_ERR("open write file %s failed, return %d\n", file_dst, res);
		f_close(fp_r);
		goto out;
	}

	while (1) {
		res = f_read(fp_r, buf, len, &byte_r);
		if (res != FR_OK) {
			CMD_ERR("read file %s failed\n", file_src);
			break;
		}

		if (byte_r == 0) {
			ret = 0;
			break;
		}

		res = f_write(fp_w, buf, byte_r, &byte_w);
		if (res != FR_OK || byte_r != byte_w) {
			CMD_ERR("write file %s failed, len %d, return %d, written %u\n",
					file_dst, byte_r, res, byte_w);
			break;
		}

		if (byte_r < len) {
			ret = 0;
			break;
		}
	}

	CMD_DBG("copy %s to %s %s, cost %u ms\n", file_src, file_dst,
	        (ret == 0) ? "success" : "failed", OS_GetTicks() - start_tm);
	f_close(fp_w);
	f_close(fp_r);

out:
	if (fp_w)
		cmd_free(fp_w);

	if (fp_r)
		cmd_free(fp_r);

	return ret;
}

int fs_test_cp_dir( const char *dir_dst, const char *dir_src)
{
	int ret = -1;
	FRESULT res;
	DIR *dp = NULL;
	FILINFO *entry = NULL;
	char *path_src = NULL;
	char *path_dst = NULL;
	uint8_t *buf = NULL;

	res = f_mkdir(dir_dst);
	if (res != FR_OK) {
		CMD_ERR("create dir %s failed, return %d\n", dir_dst, res);
		return -1;
	} else {
		CMD_DBG("create dir %s success\n", dir_dst);
	}

	dp = cmd_malloc(sizeof(DIR));
	cmd_memset(dp, 0, sizeof(DIR));

	entry = cmd_malloc(sizeof(FILINFO));
	cmd_memset(entry, 0, sizeof(FILINFO));

	res = f_opendir(dp, dir_src);
	if (res != FR_OK) {
		CMD_ERR("open dir %s failed, return %d\n", dir_src, res);
		goto out;
	}

	path_src = cmd_malloc(FS_TEST_PATH_MAX_SIZE);
	path_dst = cmd_malloc(FS_TEST_PATH_MAX_SIZE);
	buf = cmd_malloc(FS_TEST_DATA_BUF_SIZE);

	while (1) {
		res = f_readdir(dp, entry);
		if (res != FR_OK) {
			CMD_ERR("read dir %s failed, return %d\n", dir_src, res);
			goto out;
		}

		if (entry->fname[0] == 0) {
			break;
		}

		cmd_snprintf(path_src, FS_TEST_PATH_MAX_SIZE, "%s/%s", dir_src, entry->fname);
		cmd_snprintf(path_dst, FS_TEST_PATH_MAX_SIZE, "%s/%s", dir_dst, entry->fname);
		if (entry->fattrib & AM_DIR) {
			if (fs_test_cp_dir(path_dst, path_src) < 0) {
				goto out;
			}
		}

		if (fs_test_cp_file(path_dst, path_src, buf, FS_TEST_DATA_BUF_SIZE) < 0) {
			goto out;
		}
	}

	f_closedir(dp);
	ret = 0;

out:
	if (buf)
		cmd_free(buf);

	if (path_dst)
		cmd_free(path_dst);

	if (path_src)
		cmd_free(path_src);

	if (entry)
		cmd_free(entry);

	if (dp)
		cmd_free(dp);

	CMD_DBG("copy dir from %s to %s %s\n",
	        dir_src, dir_dst, ret == 0 ? "success" : "failed");
	return ret;
}

struct fs_test_param {
	uint8_t task_idx;
	uint8_t file_num;
	int file_size_unit;
};

static void fs_test_task(void *arg)
{
	char dir_path[2][64];
	struct fs_test_param *param = arg;

	CMD_DBG("fatfs test task %d start ...\n", param->task_idx);

	CMD_DBG("*** create dir and file test ***\n");
	cmd_sprintf(dir_path[0], "%s/dir%d_0-%s", CMD_FS_VOL_NAME, param->task_idx, FS_TEST_LFN_STR);
	fs_test_create_dir(dir_path[0], param->file_num, param->file_size_unit);

	CMD_DBG("*** copy dir test ***\n");
	cmd_sprintf(dir_path[1], "%s/dir%d_1-%s", CMD_FS_VOL_NAME, param->task_idx, FS_TEST_LFN_STR);
	fs_test_cp_dir(dir_path[1], dir_path[0]);

	CMD_DBG("*** copy dir test ***\n");
	cmd_sprintf(dir_path[1], "%s/dir%d_2-%s", CMD_FS_VOL_NAME, param->task_idx, FS_TEST_LFN_STR);
	fs_test_cp_dir(dir_path[1], dir_path[0]);

	CMD_DBG("*** remove dir test ***\n");
	cmd_sprintf(dir_path[0], "%s/dir%d_0-%s", CMD_FS_VOL_NAME, param->task_idx, FS_TEST_LFN_STR);
	fs_test_rm_dir(dir_path[0], 1);

	CMD_DBG("fatfs test task %d finish\n", param->task_idx);
	cmd_free(arg);
	OS_ThreadDelete(NULL);
}

static enum cmd_status cmd_fs_mount_exec(char *cmd)
{
	FRESULT res;

	res = f_mount(&cmd_fs, "", 1);
	if (res != FR_OK) {
		CMD_ERR("mount fatfs failed, return %d\n", res);
		return CMD_STATUS_FAIL;
	}

	CMD_DBG("mount fatfs success\n");
	return CMD_STATUS_OK;
}

static enum cmd_status cmd_fs_unmount_exec(char *cmd)
{
	FRESULT res;

	res = f_mount(NULL, "", 1);
	if (res != FR_OK) {
		CMD_ERR("unmount fatfs failed, return %d\n", res);
		return CMD_STATUS_FAIL;
	}

	cmd_memset(&cmd_fs, 0, sizeof(cmd_fs));
	CMD_DBG("unmount fatfs success\n");
	return CMD_STATUS_OK;
}

// fs test t=1, n=2, u=2097152	// 2MB
static enum cmd_status cmd_fs_test_exec(char *cmd)
{
	OS_Thread_t thread;
	int i, thread_cnt, file_num, file_size_unit, cnt;
	struct fs_test_param *param;

	cnt = cmd_sscanf(cmd, "t=%d, n=%d, u=%d", &thread_cnt, &file_num, &file_size_unit);
	if (cnt != 3 || thread_cnt < 1) {
		CMD_ERR("invalid argument %s\n", cmd);
		return CMD_STATUS_FAIL;
	}

	for (i = 0; i < thread_cnt; ++i) {
		param = cmd_malloc(sizeof(struct fs_test_param));
		param->task_idx = i;
		param->file_num = file_num;
		param->file_size_unit = file_size_unit;

		OS_ThreadSetInvalid(&thread);
		if (OS_ThreadCreate(&thread,
			                "",
			                fs_test_task,
			                (void *)param,
			                OS_THREAD_PRIO_APP,
			                2 * 1024) != OS_OK) {
			CMD_ERR("create fs test task %d failed\n", i);
			return CMD_STATUS_FAIL;
		}
		OS_Sleep(1);
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_fs_emptydir_exec(char *cmd)
{
	if (fs_test_rm_dir(cmd, 0) < 0)
		return CMD_STATUS_FAIL;
	else
		return CMD_STATUS_OK;
}

static struct cmd_data g_fs_cmds[] = {
	{ "mount",		cmd_fs_mount_exec },
	{ "unmount",	cmd_fs_unmount_exec },
	{ "test",		cmd_fs_test_exec },
	{ "rmdir",		cmd_fs_emptydir_exec },
};

enum cmd_status cmd_fs_exec(char *cmd)
{
	return cmd_exec(cmd, g_fs_cmds, cmd_nitems(g_fs_cmds));
}
