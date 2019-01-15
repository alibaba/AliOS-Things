/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#if defined (AOS_COMP_SPIFFS) && defined (AOS_COMP_VFS)

#include "include/ulog_config.h"
#include <string.h>
#include "aos/vfs.h"
#include "aos/errno.h"
#include <fcntl.h>
#include "ulog/ulog.h"

#define TRACE_SESSION_FS LOG

#define ULOG_FILE_FORMAT     "/spiffs/ulog_%03d.log"

#define ULOG_FILE_CFG_IDX    0

#define ULOG_CFG_LINE_MAX_SIZE 64

 /**
  * Logging File Index is saved in ulog cfg files,with key ULOG_FILE_CFG_UP_IDX.
  * REMARK, this value is 1 BASED
  */
#define ULOG_FILE_CFG_UP_IDX "ulog_file_idx"

#define LOG_LINE_SEPARATOR '\n'
#define LOG_LINE_TAIL_STR  "\n"
static uint16_t operating_file_idx = 0;
/**
* record the log files operating. Recover(reset or get) in reload_log_argu()
* used and count in write_log_line()
*/
static uint32_t operating_file_offset = 0;

static int get_log_line(const int file_instanse, const off_t off, char* buf, const uint16_t buf_len);

/**
*
* Check if file already exist
*
* @param  file_idx file index to be checked
* @return true indicats it is existed, or else not.
*/
static bool log_file_exist(const uint16_t file_idx)
{
    bool rc = false;
    char ulog_file_name[32];
    snprintf(ulog_file_name, sizeof(ulog_file_name), ULOG_FILE_FORMAT, file_idx);

    const int fd = aos_open(ulog_file_name, (O_RDWR| O_CREAT | O_EXCL));
    if ( fd < 0 ) {
        if (fd==-EEXIST) {
            TRACE_SESSION_FS("file %s alreay exist\n", ulog_file_name);
            rc = true;
        }
    } else {
        aos_unlink(ulog_file_name);
    }
    return rc;
}

static int open_create_log_file(const uint16_t file_idx, const bool keep_open)
{
    int fd = -1;
    char ulog_file_name[32];
    snprintf(ulog_file_name, sizeof(ulog_file_name), ULOG_FILE_FORMAT, file_idx);
    TRACE_SESSION_FS("open create log %s\n", ulog_file_name);
    aos_unlink(ulog_file_name);
    fd = aos_open(ulog_file_name, (O_RDWR | O_CREAT | O_TRUNC));
    if (fd >= 0) {
        if (!keep_open) {
            aos_close(fd);
        }
    }else{
        TRACE_SESSION_FS("open create file %s fail fd %d %d\n",ulog_file_name,fd,errno);
    }
    return fd;
}

static int open_log_file(const uint16_t file_idx, int flag, const off_t off)
{
    int fd = -1;
    char ulog_file_name[32];
    memset(ulog_file_name, 0, sizeof(ulog_file_name));
    snprintf(ulog_file_name, sizeof(ulog_file_name), ULOG_FILE_FORMAT, file_idx);
    fd = aos_open(ulog_file_name, flag);
    if (fd >= 0) {
        if (aos_lseek(fd, off, SEEK_SET) == -1) {
            TRACE_SESSION_FS("seek fail %s\n", ulog_file_name);
            aos_close(fd);
            fd = -1;
        }
    } else {
        TRACE_SESSION_FS("open %s fail fd %d err %d\n", ulog_file_name, fd, errno);
    }
    return fd;
}

static int close_log_file(const int fd)
{
    return aos_close(fd);
}

static uint32_t get_ulog_config(const char* cfg_key)
{
    uint32_t rc = -1;
    const int fd = open_log_file(ULOG_FILE_CFG_IDX, O_RDONLY, 0);
    if (fd >= 0) {
        char one_cfg_item[ULOG_CFG_LINE_MAX_SIZE];
        int off = 0;
        int read_len = 0;
        /* log cofig file exist, read it to archive config item */
        while ((read_len = get_log_line(fd, off, one_cfg_item, sizeof(one_cfg_item))) > 1) {
            off += ( read_len + ((ULOG_CFG_LINE_MAX_SIZE == read_len)?0:1) );
            char * q = NULL;
            if ((q = strstr(one_cfg_item, cfg_key)) != NULL) {
                char* val_str = &q[strlen(cfg_key) + 1];
                rc = strtoul(val_str, NULL, 10);
                break;
            }
        }
        close_log_file(ULOG_FILE_CFG_IDX);
    }
    return rc;
}

/**
*
* Copies one line from file_instanse into buf, On sucessful the destination buf
* buf is changed into NULL terminated C String. If log content size is longer
* than buf_len-1, remain will be not saved and NULL terminatated is implicitly
* appended at the end of destination.
*
* @param  file_instanse file description from aos_open() before
* @param  off offset which is alse the count has readed for this file before
* @param  buf local buffer use for saved, ZERO format is not MUST
* @param  buf_len buffer size
* @return actual log text length readed and saved in argumenent buf;
*         '\n','\0' is not count. Expected value: 1~buf_len-1
*         0 indicates EOF of the file, buf_len indicates the passing value is
*         limited to read the whole line.
*/
static int get_log_line(const int file_instanse, const off_t off, char* buf, const uint16_t buf_len)
{
    int rc = -1;
    if (file_instanse >= 0 && buf != NULL && buf_len > 0) {
        int seek_rt = -1;
        if (-1 != (seek_rt = aos_lseek(file_instanse, off, SEEK_SET))) {
            int cnt = 0;
            while ((cnt < buf_len) && (0 < aos_read(file_instanse, &buf[cnt], 1))) {
                if (buf[cnt++] == LOG_LINE_SEPARATOR) {
                    break;
                }
            }

            if (cnt == 0) {
                /* Nothing read, this is an empty file */
                rc = 0;
            }
            else if (cnt != buf_len) {
                if (buf[cnt - 1] == LOG_LINE_SEPARATOR) {
                    rc = cnt - 1;
                }
                else {
                    rc = cnt;
                }
                /* replacement/end with null terminated */
                buf[rc] = 0;
            }
            else {/* cnt == buf_len */
                if (buf[cnt - 1] == LOG_LINE_SEPARATOR) {
                    /* just fit */
                    rc = cnt - 1;
                }
                else {
                    /* buffer is not sufficient to save whole line,
                       return buf_len to indicates customer */
                    rc = cnt;
                }

                /* replacement with null terminated */
                buf[cnt - 1] = 0;
            }
        }
        else {
            TRACE_SESSION_FS("seek fail rlt %d err %d\n", seek_rt, errno);
        }
    }
    return rc;
}


/**
*
* Write one line into specify log file, which instance is file_instanse. Append a LOG_LINE_SEPARATOR
* after log context to separate logs
*
* @param  file_instanse file description from aos_open() before
* @param  buf local buffer use for write
* @param  keep_open keep opening after write finished
* @param operating indicates the rolling-back mechanism used.
* @return actual writen text length(includes length of LOG_LINE_SEPARATOR).
*         -EINVAL indicates parameter illegal, other value indicates call aos_write failure
*/
static int write_log_line(const int file_instanse, const char* buf, const bool keep_open)
{
    int rtn = -EINVAL;
    if (file_instanse >= 0 && buf != NULL) {
        int rc = -1;
        if ((rtn = aos_write(file_instanse, buf, strlen(buf))) > 0) {
            if( 1 == (rc=aos_write(file_instanse, LOG_LINE_TAIL_STR, 1)) ){
                rtn ++;
            }else{
                rtn = rc;
            }
        } else {
            TRACE_SESSION_FS("write fail rc %d\n", rtn);
        }

        if (!keep_open) {
            close_log_file(file_instanse);
        }
    }

    return rtn;
}

/**
*
* Refresh ulog cfg item and saved in cfg file, also create the new ulog file
*
* @param  init indicates if this call is initialization operation.
*              destroy and creat new cfg, with operating_idx reset to 1 if set;
*              open cfg file and write the current operating_idx in.
*
* @return 0 indicates config files and log files both operating ok;
*         -1 indicates config file operating fail
*         -2 indicates log file operating fail
*/
static int refresh_ulog_config(const bool init)
{
    int rc = -1;
    int fd = -1;
    operating_file_offset = 0;
    if (init) {
        fd = open_create_log_file(ULOG_FILE_CFG_IDX, true);
        operating_file_idx = 1;
    } else {
        if (operating_file_idx > LOCAL_FILE_CNT) {
            /* rolling back */
            operating_file_idx = 1;
        }
        fd = open_log_file(ULOG_FILE_CFG_IDX, O_WRONLY, 0);
    }

    if (fd >= 0) {
        char tmp_cfg_buf[ULOG_CFG_LINE_MAX_SIZE];
        snprintf(tmp_cfg_buf, sizeof(tmp_cfg_buf), "%s:%d", ULOG_FILE_CFG_UP_IDX, operating_file_idx);
        write_log_line(fd, tmp_cfg_buf, true);
        close_log_file(fd);
        /* create log file */
        if (open_create_log_file(operating_file_idx, false) >= 0) {
            rc = 0;
        } else {
            rc = -2;
        }
    } else {
        TRACE_SESSION_FS("refresh ulog cfg fail fd %d err %d\n", fd, errno);
    }
    return rc;
}

/**
*
* Reload log history arguments from ulog cfg file(ulog_0000.log) in fs,
* includes operating_file_idx & operating_file_offset. New ulog_0000.log
* will be created if none ulog_0000.log found or text is illegal.
* This is vital for ulog pop via fs, ulog pop via fs will be forbidden if
* this step fail.
*
* return 0 if this step pass, else indicates this step fail
*
*/
static int reload_log_argu()
{
    int rc = -1;
    if (log_file_exist(ULOG_FILE_CFG_IDX)) {
        /* ulog cfg exist, try to read it */
        const uint32_t tmp_idx = get_ulog_config(ULOG_FILE_CFG_UP_IDX);
        operating_file_offset = 0;
        TRACE_SESSION_FS("log file idx %d\n", tmp_idx);
        if (tmp_idx > 0 && tmp_idx <= LOCAL_FILE_CNT) {
            operating_file_idx = tmp_idx;
            if (log_file_exist(operating_file_idx)) {
                /* log file does exist, open it and calculate content */
                int fd = open_log_file(operating_file_idx, O_RDONLY, 0);
                if (fd >= 0) {
                    int read_len = 0;
                    char log_buf[SYSLOG_SIZE];
                    /* log file exist, read it to archive operating_file_offset */
                    while ((read_len = get_log_line(fd, operating_file_offset, log_buf, SYSLOG_SIZE)) > 1) {
                        operating_file_offset += (read_len + ((read_len == SYSLOG_SIZE) ? 0 : 1));
                    }

                    close_log_file(fd);
                    if(operating_file_offset>=LOCAL_FILE_SIZE){
                        operating_file_idx++;
                        /* operating file is close to full, create new one */
                        rc = refresh_ulog_config(false);
                    }else{
                        rc = 0;
                    }
                } else {
                    TRACE_SESSION_FS("log file exist buf can not open it\n");
                    rc = -2;
                }
            } else {
                /* no such log file exist, then create it */
                rc = refresh_ulog_config(false);
            }

        } else {
            /* destroy the ulog cfg */
            rc = refresh_ulog_config(true);
        }
    } else {
        rc = refresh_ulog_config(true);
    }
    return rc;
}

/**
* @brief not thread-safe, but only be used in one task(ulog), so not necessary considering mutex
* @param data
* @param len
*
* @return -1 indicates not send out sucessfully
*
*/
int32_t pop_out_on_fs(const char* data, const uint16_t len)
{
    int32_t ret = -1;
    return ret;
}

#endif

extern char *syslog_format_time(char *buffer, const int len);

void test_log_process()
{
    int fd = open_log_file(operating_file_idx, O_WRONLY, operating_file_offset);
    if (fd >= 0) {
        TRACE_SESSION_FS("PREPARE WRITE LOG %d, idx %d off %d\n", fd, operating_file_idx, operating_file_offset);

        char log_buf[64];
        int rc = -1;
        while (operating_file_offset < LOCAL_FILE_SIZE) {
            memset(log_buf, 0, sizeof(log_buf));
            syslog_format_time(log_buf, sizeof(log_buf));
            strncat(log_buf, "helloworld", sizeof(log_buf));
            rc=write_log_line(fd, log_buf, true);
            if(rc>0){
                operating_file_offset += rc;
				TRACE_SESSION_FS("log this line %d total %d\n", rc,operating_file_offset);
            }else{
                TRACE_SESSION_FS("write fail %d\n",rc);
                break;
            }

            aos_msleep(500);
        }
        close_log_file(fd);
        operating_file_idx++;
        refresh_ulog_config(false);

        TRACE_SESSION_FS("LOG PROCESS FIN %d\n",operating_file_offset);
    } else {
        TRACE_SESSION_FS("FAIL WRITE\n");
    }
}


int test_log_fs()
{
    vfs_spiffs_register();
    aos_dir_t *dp;
    dp = (aos_dir_t *)aos_opendir("/spiffs");

    if (dp != NULL) {
        aos_dirent_t *out_dirent;
        while(1) {
            out_dirent = (aos_dirent_t *)aos_readdir(dp);
            if (out_dirent == NULL)
                break;

            TRACE_SESSION_FS("file name is %s\n", out_dirent->d_name);
        }
    }
    aos_closedir(dp);

    if (0 == reload_log_argu()) {
        test_log_process();

        test_log_process();
    }else{
        TRACE_SESSION_FS("reload log argu fail\n");
    }
    return 0;
}
