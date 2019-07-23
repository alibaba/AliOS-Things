/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "ulog_session_file.h"
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "ulog_api.h"
#include "ulog/ulog.h"

#include "aos/errno.h"
#include "cJSON.h"
#include "aos/kernel.h"
#include "uagent/uagent.h"

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

#if ULOG_RESERVED_FS
static char *ulog_fs_tmp[ULOG_RESERVED_FS_SIZE] = { NULL };

static uint16_t tmp_queue_in = 0;
static uint16_t tmp_queue_out = 0;

static void stop_operating();

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
#include "http_api.h"
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
            char *upload_stream = (char*)aos_malloc(LOCAL_FILE_SIZE);
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
    if (file_idx <= LOCAL_FILE_CNT) {
        char ulog_file_name[128];
        snprintf(ulog_file_name, sizeof(ulog_file_name), ULOG_FILE_FORMAT, file_idx);

        const int fd = aos_open(ulog_file_name, (O_RDWR | O_CREAT | O_EXCL)
#ifdef CSP_LINUXHOST
            , 0644
#endif
        );
        SESSION_FS_DEBUG("check if file %s exist %d\n", ulog_file_name, fd);

        if (fd < 0) {
#ifdef CSP_LINUXHOST
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

static int open_create_log_file(const ulog_idx_type file_idx, const bool keep_open)
{
    int fd = -1;
    char ulog_file_name[128];
    snprintf(ulog_file_name, sizeof(ulog_file_name), ULOG_FILE_FORMAT, file_idx);
    SESSION_FS_DEBUG("open create log %s\n", ulog_file_name);
    aos_unlink(ulog_file_name);
    fd = aos_open(ulog_file_name, (O_RDWR | O_CREAT | O_TRUNC)
#ifdef CSP_LINUXHOST
        , 0644
#endif
    );
    if (fd >= 0) {
        if (!keep_open) {
            aos_close(fd);
        }
    } else {
        SESSION_FS_INFO("open create file %s fail fd %d, errno %d\n", ulog_file_name, fd, errno);
    }
    return fd;
}

/* O_APPEND */
int open_log_file(const ulog_idx_type file_idx, int flag, const off_t off)
{
    int fd = -1;
    char ulog_file_name[128];
    memset(ulog_file_name, 0, sizeof(ulog_file_name));
    snprintf(ulog_file_name, sizeof(ulog_file_name), ULOG_FILE_FORMAT, file_idx);
    fd = aos_open(ulog_file_name, flag);
    if (fd >= 0) {
        const int seek_off = aos_lseek(fd, off, SEEK_SET);
        if (seek_off != off) {
            SESSION_FS_INFO("seek fail %s %d\n", ulog_file_name, seek_off);
            aos_close(fd);
            fd = -1;
        }
    } else {
        SESSION_FS_INFO("open %s fail fd %d\n", ulog_file_name, fd);
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
            SESSION_FS_INFO("write fail rc %d\n", rtn);
        }

        if (!keep_open) {
            aos_close(file_instanse);
        }
    }

    return rtn;
}

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
                        SESSION_FS_INFO("sync to cfg file fail %d\n", fd);
                    }
                    aos_close(fd);
                } else {
                    SESSION_FS_INFO("refresh ulog cfg fail fd %d\n", fd);
                }

            } else {
                SESSION_FS_INFO("sync to cfg file fail %d\n", fd);
            }
        } else {

            SESSION_FS_INFO("Fatal Error Operate ulog mm cfg\n");
        }
    } else {

        SESSION_FS_INFO("Fatal Error update ulog mm cfg\n");
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
    if (log_file_exist(ULOG_FILE_CFG_IDX)) {
        /* ulog cfg exist, try to read it */
        load_cfg_2_mm();
        tmp_idx = get_working_from_cfg_mm();

        SESSION_FS_INFO("[%s]log file idx %d\n", ULOG_TAG_SELF, tmp_idx);
        if (tmp_idx <= LOCAL_FILE_CNT) {
            /* read it for test */
            if (log_file_exist(tmp_idx)) {
                /* log file does exist, open it and calculate content */
                int fd = open_log_file(tmp_idx, O_RDONLY, 0);
                if (fd >= 0) {
                    int read_len = 0;
                    char log_buf[ULOG_SIZE];
                    /* log file exist, read it to archive operating_file_offset */
                    while ((read_len = get_log_line(fd, log_buf, ULOG_SIZE)) > 1) {
                        operating_file_offset += read_len;
                    }

                    aos_close(fd);
                    rc = 0;
                } else {
                    SESSION_FS_INFO("log file exist buf can not open it\n");
                    rc = -2;
                }
            } else {
                /* no such log file exist, then create it */
                rc = update_new_log_file(tmp_idx);
            }
        }
    }

    if (LOCAL_FILE_CNT < tmp_idx) {
        rc = update_new_log_file(ULOG_FILE_IDX_START);
    }

    return rc;
}

void on_show_ulog_file()
{
    aos_dir_t *dp;
    SESSION_FS_INFO("log files in %s\n", FS_PATH);
    dp = (aos_dir_t *)aos_opendir(FS_PATH);

    if (dp != NULL) {
        aos_dirent_t *out_dirent;
        while (1) {
            out_dirent = (aos_dirent_t *)aos_readdir(dp);
            if (out_dirent != NULL) {
                SESSION_FS_INFO("file name is %s\n", out_dirent->d_name);
            } else {
                break;
            }
        }
    }
    aos_closedir(dp);
}

static void stop_operating()
{
    if (operating_fd >= 0) {
        char time_stop[24];

        ulog_format_time(time_stop, sizeof(time_stop));

        update_mm_cfg(ulog_cfg_type_list, get_working_from_cfg_mm(), ulog_cfg_para_end, time_stop);
        aos_sync(operating_fd);
        aos_close(operating_fd);
        operating_fd = -1;
    }
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
    int32_t rc = -1;
    if (operating_fd >= 0) {
        const int write_rlt = write_log_line(operating_fd, data, true);
        if (write_rlt > 0) {
            rc = 0;
            operating_file_offset += write_rlt;
            if (operating_file_offset >= LOCAL_FILE_SIZE) {
                stop_operating();

                /* roll back if working index reaches end */
                ulog_idx_type tmp_working = get_working_from_cfg_mm() + 1;
                if (tmp_working > LOCAL_FILE_CNT) {
                    tmp_working = ULOG_FILE_IDX_START;
                }
                operating_file_offset = 0;
                if (0 == update_new_log_file(tmp_working)) {
                    operating_fd = open_log_file(get_working_from_cfg_mm(), O_WRONLY, 0);
                }
            }

        } else {
            SESSION_FS_INFO("write fail %d\n", write_rlt);
        }
    } else {
#if ULOG_RESERVED_FS
        rc = push_fs_tmp(data, len);
        if (0 != rc) {
            SESSION_FS_INFO("*(%d)", rc);
        }
#else
        SESSION_FS_INFO("$");
#endif
    }
    return rc;
}

void on_fs_record_pause(const uint32_t on, const uint32_t off)
{
    if (on^off) {
        LOGI(ULOG_TAG_SELF, "ulog fs ctrl %d fd %d", on, operating_fd);
        if (1 == on) {
            stop_operating();
        } else {/* resume the file record */
            if (operating_fd < 0) {
                if (0 == reload_log_argu()) {
                    operating_fd = open_log_file(get_working_from_cfg_mm(), O_WRONLY, operating_file_offset);
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
* @brief ulog on fs init
*
* @return 0 indicates initialized sucessfully, or else fail
*
*/
int32_t ulog_fs_init()
{
    int32_t rc = -1;
    if (0 == session_fs_init) {
        session_fs_init = 1;
        cfg_init_mutex();
        rc = reload_log_argu();
        if (rc == 0) {
            SESSION_FS_INFO("reload ulog idx %d off %d\n", get_working_from_cfg_mm(), operating_file_offset);
            operating_fd = open_log_file(get_working_from_cfg_mm(), O_WRONLY, operating_file_offset);
        }
    }
    return rc;
}

