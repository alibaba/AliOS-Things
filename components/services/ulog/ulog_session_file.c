/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "ulog_session_file.h"
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "ulog_api.h"
#include "ulog/ulog.h"
#include "ulog_ring_fifo.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "aos/errno.h"
#include "cJSON.h"
#include "aos/kernel.h"
#include "uagent/uagent.h"
typedef struct {
    /* unit is s, if idle keeps log_file_sync_idle_timeout, the logging file will be synced */
    uint16_t           cfg_log_idle_timeout;
    uint16_t           log_idle_standing;
    aos_timer_t        timer_id_log_file_idle;
    uint32_t           off_set;
} log_file_sync_param_t;

static log_file_sync_param_t log_file_sync_param = {0, 0, {}, 0 };

 /**
  * record the log files operating. Recover(reset or get) in reload_log_argu()
  * used and count in write_log_line()
  */
static uint32_t operating_file_offset = 0;

/**
* indicates if log on fs feature initialized
*
*/
static uint8_t session_fs_init = 0;

static int operating_fd = -1;

static uint8_t log_file_failed = 0;
#if ULOG_RESERVED_FS
static char *ulog_fs_tmp[ULOG_RESERVED_FS_SIZE] = { NULL };

static uint16_t tmp_queue_in = 0;
static uint16_t tmp_queue_out = 0;

static void stop_operating(const bool roll_new);

static int push_fs_tmp(const char* data, const unsigned short len)
{
    int rc = -1;
    if (NULL != data && len != 0 && ((tmp_queue_in + 1) % ULOG_RESERVED_FS_SIZE) != tmp_queue_out) {
        if (NULL == ulog_fs_tmp[tmp_queue_in]) {
            ulog_fs_tmp[tmp_queue_in] = (char*)aos_malloc(len + 1);
            if (NULL != ulog_fs_tmp[tmp_queue_in]) {
                memcpy(ulog_fs_tmp[tmp_queue_in], data, len);
                ulog_fs_tmp[tmp_queue_in][len] = '\0';
                tmp_queue_in = (tmp_queue_in + 1) % ULOG_RESERVED_FS_SIZE;
                rc = 0;
            }
        }
    }
    return rc;
}

static int pop_fs_tmp(char* data, const unsigned short len)
{
    int rc = -EINVAL;
    if (NULL != data && len != 0) {
        if (tmp_queue_in == tmp_queue_out) {
            rc = -1;
        } else {
            if (NULL != ulog_fs_tmp[tmp_queue_out]) {
                strncpy(data, ulog_fs_tmp[tmp_queue_out], len - 1);
                aos_free(ulog_fs_tmp[tmp_queue_out]);
                ulog_fs_tmp[tmp_queue_out] = NULL;
                tmp_queue_out = (tmp_queue_out + 1) % ULOG_RESERVED_FS_SIZE;
                rc = 0;

            } else {
                rc = -EIO;
            }
        }
    }
    return rc;
}
#endif

#if ULOG_UPLOAD_LOG_FILE
#include "network/network.h"
#include "network/http.h"
#include "linkkit/http_api.h"
static httpc_connection_t *settings = NULL;
static httpc_handle_t httpc_handle = 0;
static char *up_uri = NULL;
static bool http_client_initd = false;

static int get_server_uri(const char *url, char **server, char **uri)
{
    int rc = -1;
    if (NULL != url) {
        uint8_t pos = -1;
        if (0 == strncmp(url, URL_PREFIX_HTTP, strlen(URL_PREFIX_HTTP))) {
            pos = strlen(URL_PREFIX_HTTP);
        } else if (0 == strncmp(url, URL_PREFIX_HTTPS, strlen(URL_PREFIX_HTTPS))) {
            pos = strlen(URL_PREFIX_HTTPS);
        }
        if (pos > 0) {
            char *p = NULL;
            if (NULL != (p = strchr(&url[pos], '/'))) {
                p++;
                *server = (char*)aos_malloc(p - url + 1);
                if (NULL != *server) {
                    memcpy(*server, url, p - url);
                    (*server)[p - url] = '\0';
                    const short n = strlen(p);
                    *uri = (char*)aos_malloc(n + 1);
                    if (NULL != *uri) {
                        memcpy(*uri, p, n);
                        (*uri)[n] = '\0';
                        rc = 0;
                    } else {
                        aos_free(*server);
                        *server = NULL;
                    }
                }
            }
        }
    }
    return rc;
}

static void report_up_process(const http_upload_fail_t status, const char process, const ulog_idx_type idx)
{
    cJSON *resp = cJSON_CreateObject();
    if (NULL != resp) {
        char *text = NULL;
        cJSON_AddItemToObject(resp, "idx", cJSON_CreateNumber(idx));
        if (status >= http_upload_start) {
            cJSON_AddItemToObject(resp, "process", cJSON_CreateNumber(process));
        } else {
            cJSON_AddItemToObject(resp, "error", cJSON_CreateNumber(status));
        }
        text = cJSON_PrintUnformatted(resp);
        cJSON_Delete(resp);
        uagent_send(UAGENT_MOD_ULOG, ULOG_LOG_LIST, strlen(text), text, send_policy_object);
        cJSON_free(text);
    }
}

int http_start(const char *url, const unsigned short idx)
{
    int rc = -1;
    if (!http_client_initd) {
        http_client_initd = true;
        http_client_initialize();
    }

    if (NULL == settings) {
        LOGI(ULOG_TAG_SELF, "http start %s idx %d", url, idx);
        settings = (httpc_connection_t*)aos_malloc(sizeof(httpc_connection_t));
        if (NULL != settings) {
            const int fd = socket(AF_INET, SOCK_STREAM, 0);
            if (fd < 0) {
                LOGE(ULOG_TAG_SELF, "alloc socket fd fail");
            } else {
                memset(settings, 0, sizeof(httpc_connection_t));
                settings->socket = fd;

                settings->req_buf = (uint8_t *)aos_malloc(REQ_BUF_SIZE);
                if (NULL == settings->req_buf) {
                    LOGE(ULOG_TAG_SELF, "allock req buffer fail");
                } else {
                    if (0 == get_server_uri(url, &settings->server_name, &up_uri)) {
                        LOGI(ULOG_TAG_SELF, "get server %s uri %s", settings->server_name, up_uri);

                        settings->req_buf_size = REQ_BUF_SIZE;
                        httpc_handle = httpc_init(settings);

                        if (httpc_handle == 0) {
                            LOGE(ULOG_TAG_SELF, "http session init fail");
                        } else {
                            char buf[16];
                            snprintf(buf, 16, "up idx=%d", idx);
                            rc = ulog_man(buf);
                        }

                        if(0 != rc){
                            aos_free(settings->server_name);
                            settings->server_name = NULL;
                            aos_free(up_uri);
                            up_uri = NULL;
                        }
                    } else {
                        LOGE(ULOG_TAG_SELF, "get server uri fail");
                    }
                    if(0 !=rc ){
                        aos_free(settings->req_buf);
                        settings->req_buf = NULL;
                    }
                }
                if(0 !=rc ){
                    close(fd);
                }
            }
            if (rc != 0) {
                aos_free(settings);
                settings = NULL;
            }
        } else {
            LOGE(ULOG_TAG_SELF, "allock http connect instanse fail");
        }
    } else {
        LOGE(ULOG_TAG_SELF, "Last upload not finish");
    }
    return rc;
}

static uint8_t *rsp_buf = NULL;

void on_fs_upload(const uint32_t idx, const uint32_t start)
{
    if (get_working_from_cfg_mm() == idx) {
        on_fs_record_pause(1, 0);
    }
    int fd = open_log_file(idx, O_RDONLY, 0);
    http_upload_fail_t http_operate = http_upload_common_fail;
    if (fd >= 0) {
        char hdr[HTTP_UP_HDR_SIZE] = { 0 };
        if (httpc_construct_header(hdr, HTTP_UP_HDR_SIZE, "Accept",
            "text/xml,text/javascript,text/html,application/json") > 0) {
            char *upload_stream = (char*)aos_malloc(LOCAL_FILE_SIZE+ULOG_SIZE);
            if (NULL != upload_stream) {
                int n = -1;
                if (0 < (n = aos_read(fd, upload_stream, LOCAL_FILE_SIZE+ULOG_SIZE))) {

                    char retry = HTTP_REQ_RETRY;
                    while (HTTP_SUCCESS != httpc_send_request(httpc_handle, HTTP_PUT, up_uri, hdr, "", upload_stream, n)) {
                        if (--retry <= 0) {
                            break;
                        } else {
                            LOGW(ULOG_TAG_SELF, "fs rqst %d fail", n);
                            aos_msleep(1000);
                        }
                    }
                    if (retry > 0) {
                        http_rsp_info_t rsp_info;
                        char resp_retry = HTTP_RSP_RETRY;
                        rsp_buf = (uint8_t*)aos_malloc(RSP_BUF_SIZE);
                        if (NULL != rsp_buf) {
                            while (0 > httpc_recv_response(httpc_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info, 2000)) {
                                if (--resp_retry <= 0) {
                                    break;
                                } else {
                                    aos_msleep(1000);
                                }
                            }
                            if (resp_retry > 0) {
                                http_operate = http_upload_finish;
                            }
                            aos_free(rsp_buf);
                            rsp_buf = NULL;
                        } else {
                            http_operate = http_upload_memory_fail;
                        }
                    }
                } else {
                    http_operate = http_upload_text_empty;
                    LOGW(ULOG_TAG_SELF, "nothing read");
                }
                aos_free(upload_stream);
            } else {
                http_operate = http_upload_memory_fail;
                LOGE(ULOG_TAG_SELF, "allocate file fail");
            }
        } else {
            LOGE(ULOG_TAG_SELF, "http construct header fail");
        }
        aos_close(fd);
    } else {
        http_operate = http_upload_file_operate_fail;
    }
    report_up_process(http_operate, http_operate==http_upload_finish?100:0,idx);

    httpc_deinit(httpc_handle);

    aos_free(settings->req_buf);
    settings->req_buf = NULL;
    aos_free(settings->server_name);
    settings->server_name = NULL;
    aos_free(up_uri);
    up_uri = NULL;

    aos_free(settings);
    settings = NULL;

    /* try re-start record, no impact even the record no abort before */
    on_fs_record_pause(0, 1);
}

#endif

#if LOG_FILE_SYNC_CYCLELY
static void timer_schedule(void* timer, void* para)
{
    char tmpbuf[ULOG_FILE_NOFITY_LEN];
    memset(tmpbuf, 0, ULOG_FILE_NOFITY_LEN);
    strncpy(tmpbuf, ULOG_FILE_NOTIFY, ULOG_FILE_NOFITY_LEN);

    uring_fifo_push_s(tmpbuf, ULOG_FILE_NOFITY_LEN);
}
#endif
static int32_t format_log_file(const uint16_t file_idx, char *file_name, uint16_t buffer_len)
{
    int32_t rc = -1;
    if(NULL != file_name && file_idx <= LOCAL_FILE_CNT && buffer_len>1) {
        memset(file_name, 0, buffer_len);
        if(ULOG_FILE_CFG_IDX==file_idx) {
            strncpy(file_name, ULOG_CFG_FILE, buffer_len-1);
        } else if(get_working_from_cfg_mm()==file_idx) {
            strncpy(file_name, OPERTING_LOG_FILE, buffer_len-1);
        }else {
            snprintf(file_name, buffer_len, ULOG_FILE_FORMAT, file_idx);
        }
        rc = 0;
    }
    return rc;
}
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
    char ulog_file_name[ULOG_FILE_PATH_SIZE];
    if (0 == format_log_file(file_idx, ulog_file_name, ULOG_FILE_PATH_SIZE)) {
        const int fd = aos_open(ulog_file_name, (O_RDWR | O_CREAT | O_EXCL)
#ifdef CSP_LINUXHOST
            , 0644
#endif
        );
        SESSION_FS_INFO("check if file %s exist %d\n", ulog_file_name, fd);

        if (fd < 0) {
#if defined(CSP_LINUXHOST)|| defined(CONFIG_VFS_USE_ERRNO)
            if (EEXIST == errno) {
                SESSION_FS_DEBUG("file %s alreay exist\n", ulog_file_name);
                rc = true;
            }
#else /* !CSP_LINUXHOST */
            if (fd == -EEXIST) {
                SESSION_FS_DEBUG("file %s alreay exist\n", ulog_file_name);
                rc = true;
            }
#endif /* CSP_LINUXHOST */
        } else {
            aos_unlink(ulog_file_name);
        }
    }
    return rc;
}

int open_create_log_file(const ulog_idx_type file_idx, const bool keep_open)
{
    int fd = -1;
    char ulog_file_name[ULOG_FILE_PATH_SIZE];

    memset(ulog_file_name, 0, sizeof(ulog_file_name));
    if(get_working_from_cfg_mm()==file_idx) {
        /* remove the operating idx name file */
        snprintf(ulog_file_name, sizeof(ulog_file_name), ULOG_FILE_FORMAT, file_idx);
        aos_unlink(ulog_file_name);
    }
    if (0 == format_log_file(file_idx, ulog_file_name, ULOG_FILE_PATH_SIZE)) {
        aos_unlink(ulog_file_name);
        fd = aos_open(ulog_file_name, (O_RDWR | O_CREAT | O_TRUNC)
#ifdef CSP_LINUXHOST
                , 0644
#endif
                );
        if (fd >= 0) {
            SESSION_FS_DEBUG("open create log %s\n", ulog_file_name);
            if (!keep_open) {
                SESSION_FS_DEBUG("close file: %s\n", ulog_file_name);
                aos_close(fd);
            }
        } else {
            SESSION_FS_INFO("open create file %s fail fd %d, errno %d", ulog_file_name, fd, errno);
        }
    }
    return fd;
}

static void roll_new_file(void)
{
    char ulog_file_name[ULOG_FILE_PATH_SIZE];
    int8_t retry = ULOG_FILE_FAIL_COUNT;

    memset(ulog_file_name, 0, sizeof(ulog_file_name));
    snprintf(ulog_file_name, sizeof(ulog_file_name), ULOG_FILE_FORMAT, get_working_from_cfg_mm());

    while(0!=aos_rename(OPERTING_LOG_FILE, ulog_file_name)) {
        if (--retry <= 0) {
            SESSION_FS_INFO("file %s error on rename, retry %d", ulog_file_name, retry);
            break;
        }
    }
}

int open_log_file(const ulog_idx_type file_idx, int flag, const off_t off)
{
    int fd = -1;
    char ulog_file_name[ULOG_FILE_PATH_SIZE];
    if (0 == format_log_file(file_idx, ulog_file_name, ULOG_FILE_PATH_SIZE)) {
    fd = aos_open(ulog_file_name, flag);
    if ((fd >= 0) && (off != -1)) {
        const int seek_off = aos_lseek(fd, off, SEEK_SET);
        if (seek_off != off) {
                SESSION_FS_INFO("seek fail %s %d", ulog_file_name, seek_off);
            aos_close(fd);
            fd = -1;
        }
    } else {
            SESSION_FS_INFO("open %s fail fd %d", ulog_file_name, fd);
        }
    }
    return fd;
}

/**
*
* Copies one line from file_instanse into buf, On sucessful the destination buf
* buf is changed into NULL terminated C String. If log content size is longer
* than buf_len-1, remain will be not saved and NULL terminatated is implicitly
* appended at the end of destination.
*
* @param  file_instanse file description from aos_open() before
* @param  buf local buffer use for saved, ZERO format is not MUST
* @param  buf_len buffer size
* @return actual log text length readed in argumenent buf;
*         '\n',' is counted. Expected value: 1~buf_len-1
*         0 indicates EOF of the file, buf_len indicates the possible passing value is
*         limited to read the whole line.
*/
int get_log_line(const int fd, char* buf, const uint16_t buf_len)
{
    int rc = -1;
    if (fd >= 0 && buf != NULL && buf_len > 0) {
        memset(buf, 0, buf_len);
        int cnt = 0;
        while ((cnt < buf_len) && (0 < aos_read(fd, &buf[cnt], 1))) {
            if (buf[cnt++] == LOG_LINE_SEPARATOR) {
                break;
            }
        }

        if (cnt == 0) {
            /* Nothing read, this is an empty file */
            rc = 0;
        } else if (cnt != buf_len) {
            if (buf[cnt - 1] == LOG_LINE_SEPARATOR) {
                /* replacement/end with null terminated */
                buf[cnt - 1] = 0;
            } else {
                buf[cnt] = 0;
            }
            rc = cnt;

        } else {/* cnt == buf_len */
            /* two possible result */
            /* buffer len is just fit */
            /* buffer is not sufficient to save whole line,
            last characher will be missed and replace of null-terminated */
            rc = cnt;

            /* replacement with null terminated */
            buf[cnt - 1] = 0;
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
int write_log_line(const int file_instanse, const char* buf, const bool keep_open)
{
    int rtn = -EINVAL;
    if (file_instanse >= 0 && buf != NULL) {
        int rc = -1;
        if ((rtn = aos_write(file_instanse, buf, strlen(buf))) > 0) {
            if (1 == (rc = aos_write(file_instanse, LOG_LINE_END_STR, 1))) {
                rtn++;
            } else {
                rtn = rc;
            }
        } else {
            SESSION_FS_INFO("write fail rc %d", rtn);
        }

        if (!keep_open) {
            aos_close(file_instanse);
        }
    }

    return rtn;
}
#if ULOG_ENCODE_FS
/*Write one encode line to fs */
int write_log_encode_line(const int file_instanse, const char* buf, const uint16_t len, const bool keep_open)
{
    int rtn = -EINVAL;
    if (file_instanse >= 0 && buf != NULL) {
        if ((rtn = aos_write(file_instanse, buf, len)) <= 0) {
            SESSION_FS_INFO("write fail rc %d\n", rtn);
        }

        if (!keep_open) {
            aos_close(file_instanse);
        }
    }

    return rtn;
}
#endif

/**
*
* Refresh ulog cfg item and saved in cfg file, also create the new ulog file
*
*
* @return  0 indicates create new log file sucessfully, but not means update config pass;
*          -1 create new log file fail
*/
static int update_new_log_file(const ulog_idx_type idx)
{
    int rc = -1;
    int fd = -1;

    if (0 == update_mm_cfg(ulog_cfg_type_working, idx, ulog_cfg_para_none, NULL)) {
        /* read it for test */
        if (get_working_from_cfg_mm() == idx) {

            /* create log file */
            if (open_create_log_file(idx, false) >= 0) {
                rc = 0;
                char time_start[24];

                ulog_format_time(time_start, sizeof(time_start));

                update_mm_cfg(ulog_cfg_type_list, idx, ulog_cfg_para_start, time_start);
                fd = open_create_log_file(ULOG_FILE_CFG_IDX, true);

                if (fd >= 0) { /* need update cfg file */
                    if (0 != cfg_mm_2_file(fd)) {
                        /* sync to cfg file fail, have no impact unless the board boot-up before it sync write */
                        SESSION_FS_INFO("sync to cfg file fail %d", fd);
                    }
                    aos_close(fd);
                } else {
                    SESSION_FS_INFO("refresh ulog cfg fail fd %d", fd);
                }

            } else {
                SESSION_FS_INFO("sync to cfg file fail %d", fd);
            }
        } else {

            SESSION_FS_INFO("Fatal Error Operate ulog mm cfg");
        }
    } else {

        SESSION_FS_INFO("Fatal Error update ulog mm cfg");
    }

    return rc;
}

/**
*
* Reload log history arguments from ulog cfg file(ulog_000.log) in fs,
* includes operating_file_idx & operating_file_offset. New ulog_000.log
* will be created if none ulog_000.log found or text is illegal.
* This is vital for ulog pop via fs, ulog pop via fs will be forbidden if
* this step fail.
*
* return 0 if this step pass, else indicates this step fail
*
*/
static int reload_log_argu()
{
    int rc = -1;
    ulog_idx_type tmp_idx = ULOG_FILE_IDX_INVALID;
    operating_file_offset = 0;
    log_file_sync_param.off_set = 0;
    if (log_file_exist(ULOG_FILE_CFG_IDX)) {
        /* ulog cfg exist, try to read it */
        load_cfg_2_mm();
        tmp_idx = get_working_from_cfg_mm();

        SESSION_FS_INFO("log file idx %d", tmp_idx);
        if (tmp_idx <= LOCAL_FILE_CNT) {
            /* read it for test */
            if (log_file_exist(tmp_idx)) {
                SESSION_FS_INFO("log file exist");
                /* log file does exist, open it and calculate content */
                char *_fname = (char *)malloc(ULOG_FILE_PATH_SIZE);
                if (!_fname) {
                    SESSION_FS_INFO("malloc error");
                    return -1;
                }

                if (0 == format_log_file(tmp_idx, _fname, ULOG_FILE_PATH_SIZE)) {
                    struct stat buf;
                    if (stat(_fname, &buf) == -1) {
                        SESSION_FS_INFO("stat error");
                        free(_fname);
                        return -2;
                    }
                    free(_fname);
                    operating_file_offset = buf.st_size;
                    SESSION_FS_INFO("idx %d operating_file_offset is %d",
                                    tmp_idx, operating_file_offset);
                    rc = 0;
                } else {
                    free(_fname);
                    SESSION_FS_INFO("format_log_file error");
                    return -3;
                }
            } else {
                /* no such log file exist, then create it */
                SESSION_FS_INFO("log file not exist");
                rc = update_new_log_file(tmp_idx);
            }
        }
    } else {
        SESSION_FS_INFO("config file not exist");
    }

    if (LOCAL_FILE_CNT < tmp_idx) {
        rc = update_new_log_file(ULOG_FILE_IDX_START);
    }

    return rc;
}

void on_show_ulog_file()
{
    aos_dir_t *dp;
    SESSION_FS_INFO("log files in %s", FS_PATH);
    dp = (aos_dir_t *)aos_opendir(FS_PATH);

    if (dp != NULL) {
        aos_dirent_t *out_dirent;
        while (1) {
            out_dirent = (aos_dirent_t *)aos_readdir(dp);
            if (out_dirent != NULL) {
                SESSION_FS_INFO("file name is %s", out_dirent->d_name);
            } else {
                break;
            }
        }
    }
    aos_closedir(dp);
}

static void stop_operating(const bool roll_new)
{
    if (operating_fd >= 0) {
        char time_stop[24];

        ulog_format_time(time_stop, sizeof(time_stop));

        update_mm_cfg(ulog_cfg_type_list, get_working_from_cfg_mm(), ulog_cfg_para_end, time_stop);
        aos_fsync(operating_fd);
        aos_close(operating_fd);
        if(roll_new) {
            roll_new_file();
        }
        operating_fd = -1;
    }
}

static void write_fail_retry()
{
    if (++log_file_failed >= ULOG_FILE_FAIL_COUNT) {
        operating_fd = -1;
        int8_t retry = ULOG_FILE_FAIL_COUNT;
        char log_file_name[ULOG_FILE_PATH_SIZE];

        snprintf(log_file_name, ULOG_FILE_PATH_SIZE, ULOG_FILE_FORMAT, get_working_from_cfg_mm());
        while(0!=aos_unlink(log_file_name)) {
            if (--retry <= 0) {
                SESSION_FS_INFO("file %s error on remove, retry %d", log_file_name, retry);
                break;
            }
        }

        if(retry > 0){
            SESSION_FS_INFO("remove file %s, then create new one %d", log_file_name, get_working_from_cfg_mm());
            if (0 == update_new_log_file(get_working_from_cfg_mm())) {
                operating_fd = open_log_file(get_working_from_cfg_mm(), O_WRONLY | O_TRUNC, -1);
#if ULOG_RESERVED_FS
                if (operating_fd >= 0) {
                    char buf[ULOG_SIZE];
                    memset(buf, 0, ULOG_SIZE);
                    while (0 == pop_fs_tmp(buf, ULOG_SIZE)) {
                        pop_out_on_fs(buf, strlen(buf));
                       memset(buf, 0, ULOG_SIZE);
                    }
                }
#endif /* ULOG_RESERVED_FS */
            }
        }
    }
}
#if LOG_FILE_SYNC_CYCLELY
static void ulog_file_sync(void)
{
    if (operating_fd >= 0) {
        aos_fsync(operating_fd);
        SESSION_FS_DEBUG("File %d sync offset %d standing %d\n", operating_fd, log_file_sync_param.off_set, log_file_sync_param.log_idle_standing);
    }
    log_file_sync_param.off_set = 0;
    log_file_sync_param.log_idle_standing = 0;
}
#endif /* LOG_FILE_SYNC_CYCLELY */

void ulog_file_sync_schedule(void)
{
#if LOG_FILE_SYNC_CYCLELY
    if(++log_file_sync_param.log_idle_standing >= log_file_sync_param.cfg_log_idle_timeout) {
        log_file_sync_param.log_idle_standing = 0;
        SESSION_FS_DEBUG("File %d try sync off %d", operating_fd,log_file_sync_param.off_set);
        if(log_file_sync_param.off_set >0) {
            ulog_file_sync();
        }
    }
#endif
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
    int32_t rc = -1, write_rlt;

    if (len == 0) {
        return 0;
    }

    if (operating_fd >= 0) {
#if !ULOG_ENCODE_FS
        write_rlt = write_log_line(operating_fd, data, true);
#else
        write_rlt = write_log_encode_line(operating_fd, data, len, true);
#endif
        if (write_rlt > 0) {
            log_file_failed = 0;
            rc = 0;
            operating_file_offset += write_rlt;
            log_file_sync_param.log_idle_standing = 0;
            if (operating_file_offset >= LOCAL_FILE_SIZE) {
                stop_operating(true);

                /* roll back if working index reaches end */
                ulog_idx_type tmp_working = get_working_from_cfg_mm() + 1;
                if (tmp_working > LOCAL_FILE_CNT) {
                    tmp_working = ULOG_FILE_IDX_START;
                }
                operating_file_offset = 0;
                log_file_sync_param.off_set = 0;
                if (0 == update_new_log_file(tmp_working)) {
                    operating_fd = open_log_file(get_working_from_cfg_mm(), O_WRONLY | O_TRUNC, -1);
                }
            } else {
#if LOG_FILE_SYNC_CYCLELY
                log_file_sync_param.off_set += write_rlt;
                if(log_file_sync_param.off_set >= FILE_SYNC_SIZE) {
                    ulog_file_sync();
                }
#endif /* LOG_FILE_SYNC_CYCLELY */
            }
        } else {
            SESSION_FS_INFO("write fail %d retry %d", write_rlt, log_file_failed);
#if ULOG_RESERVED_FS
            /* save them temporary */
            rc = push_fs_tmp(data, len);
            if (0 != rc) {
                SESSION_FS_INFO("*(%d)", rc);
            }
#endif /* ULOG_RESERVED_FS */
            /* check fail count */
            write_fail_retry();
        }
    } else {
#if ULOG_RESERVED_FS
        rc = push_fs_tmp(data, len);
        if (0 != rc) {
            SESSION_FS_INFO("*(%d)", rc);
        }
#else
        SESSION_FS_DEBUG("$");
#endif
    }
    return rc;
}

void on_fs_record_pause(const uint32_t on, const uint32_t off)
{
    if (on^off) {
        LOGI(ULOG_TAG_SELF, "ulog fs ctrl %d fd %d", on, operating_fd);
        if (1 == on) {
            stop_operating(false);
        } else {/* resume the file record */
            if (operating_fd < 0) {
                if (0 == reload_log_argu()) {
                    operating_fd = open_log_file(get_working_from_cfg_mm(), O_WRONLY | O_APPEND, -1);
                    LOGI(ULOG_TAG_SELF, "reload ulog idx %d off %d rlt %d",
                        get_working_from_cfg_mm(), operating_file_offset, operating_fd);
#if ULOG_RESERVED_FS
                    if (operating_fd >= 0) {
                        char buf[ULOG_SIZE];
                        memset(buf, 0, ULOG_SIZE);
                        while (0 == pop_fs_tmp(buf, ULOG_SIZE)) {
                            pop_out_on_fs(buf, strlen(buf));
                            memset(buf, 0, ULOG_SIZE);
                        }
                    }
#endif
                } else {
                    LOGE(ULOG_TAG_SELF, "restart ulog fs fail");
                }
            } else {
                LOGW(ULOG_TAG_SELF, "resume file record fail %d", operating_fd);
            }
        }
    }
}

void on_update_level2cfg_file(const uint32_t param1, const uint32_t param2)
{
    int fd = open_create_log_file(ULOG_FILE_CFG_IDX, true);
    if (fd >= 0) { /* need update cfg file */
        if (0 != cfg_mm_2_file(fd)) {
            /* sync to cfg file fail, have no impact unless the board boot-up before it sync write */
            ULOG_TRACE_ERR("sync to cfg file fail %d", fd);
        } else {
            SESSION_FS_INFO("Update LogLevel into File scucessfully");
        }
        aos_fsync(fd);
        aos_close(fd);
    } else {
        ULOG_TRACE_ERR("refresh ulog cfg fail fd %d", fd);
    }
}
void fs_control_cli(const char cmd, const char* param)
{
    if (param != NULL) {
        switch (cmd)
        {
        case 't': {
            int control_cmd = strtoul(param, NULL, 10);
            if (control_cmd) {
                ulog_man("fspause on=1");
            } else {
                ulog_man("fspause off=1");
            }
        }
                  break;

        default:
            break;
        }
    }
}

/**
* @brief access dir or create if not exist on fs
*
* @return 0 indicates mkdir(or dir exist)sucessfully, or -1 fail
*
*/
static int32_t log_dir_touch(const char *path)
{
    if (aos_access(path, F_OK)) {
        if (aos_mkdir(path)) {
            SESSION_FS_INFO("dir %s create err!!\n", path);
            return -1;
        }
    }
    return 0;
}

/**
* @brief create(or dir exist) log dir on fs
*
* @return 0 indicates both FS_PATH and FS_BACKUP_PATH exist, or -1 fail
*
*/
static int32_t log_dir_create(void)
{
#if (AOS_CONFIG_USER_SPACE > 0)
    if ((log_dir_touch(FS_BASE_PATH)) || (log_dir_touch(FS_BASE_BACKUP_PATH))) {
        /* fs base path or base backup path create fail */
        return -1;
    }
#endif
    if ((log_dir_touch(FS_PATH) == 0) && (log_dir_touch(FS_BACKUP_PATH) == 0))
        return 0;
    else
        return -1;
}

/**
* @brief ulog on fs init
*
* @return 0 indicates initialized sucessfully, or else fail
*
*/
int32_t ulog_fs_init()
{
    int32_t rc = -1;
    if (0 == session_fs_init) {
        rc = log_dir_create();
        if (rc == 0) {
            SESSION_FS_INFO("dir %s and %s exist\n", FS_PATH, FS_BACKUP_PATH);
            session_fs_init = 1;
            cfg_init_mutex();
            rc = reload_log_argu();
            if (rc == 0) {
                operating_fd = open_log_file(get_working_from_cfg_mm(), O_WRONLY | O_APPEND, -1);
                SESSION_FS_INFO("reload ulog idx %d off %u new fd %d\n", get_working_from_cfg_mm(), operating_file_offset, operating_fd);
#if LOG_FILE_SYNC_CYCLELY
                if(operating_fd>=0) {
                    if(0 != FILE_SYNC_TIMEOUT) {
                        log_file_sync_param.log_idle_standing    = 0;
                        log_file_sync_param.off_set              = 0;
                        log_file_sync_param.cfg_log_idle_timeout = (FILE_SYNC_TIMEOUT*1000+500)/FILE_SYNC_SCHEDULE;
                        if (0 != aos_timer_new(&log_file_sync_param.timer_id_log_file_idle,
                                    timer_schedule, NULL, FILE_SYNC_SCHEDULE, 1)) {
                            SESSION_FS_INFO("start timer failed\n");
                        }
                    }
                }
#endif
            }
        }
    }
    return rc;
}

