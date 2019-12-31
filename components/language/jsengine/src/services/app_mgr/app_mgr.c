/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "jse_common.h"

#include "app_mgr.h"
#include "ota_socket.h"
#include "dbg_ws.h"

#include "ota/ota_agent.h"

#ifdef LINUXOSX
#define OTA_BUFFER_MAX_SIZE 8192
#else
#define OTA_BUFFER_MAX_SIZE 1536
#endif
#define HTTP_HEADER                      \
    "GET /%s HTTP/1.1\r\nAccept:*/*\r\n" \
    "User-Agent: Mozilla/5.0\r\n"        \
    "Cache-Control: no-cache\r\n"        \
    "Connection: close\r\n"              \
    "Host:%s:%d\r\n\r\n"

typedef struct {
    uint16_t file_count;
    uint16_t pack_version;
    uint32_t pack_size;
} JSEPACK_HEADER;

typedef struct {
    uint32_t header_size;
    uint32_t file_size;
    uint8_t md5[16];
    /* uint8_t  name[...];
       uint8 data[]; */
} JSEPACK_FILE;

static write_js_cb_t jspackcb = NULL;
static JSEPACK_HEADER header;
static JSEPACK_FILE fileheader;
static int32_t jspacksize = 0;

static int32_t jspackfile_offset;
static int32_t jspackfile_header_offset;
static int32_t jspackfile_count;
static int32_t jspack_done        = 0;
static int32_t jspack_found_error = 0;

#define JSEPACK_BLOCK_SIZE 2 * 1024

static uint8_t *jspackdst_buf = NULL;

void apppack_init(write_js_cb_t cb)
{
    jspackcb                 = cb;
    jspacksize               = 0;
    jspackfile_offset        = 0;
    jspackfile_header_offset = 0;
    jspack_done              = 0;
    jspackfile_count         = 0;
    jspack_found_error       = 0;

    jspackdst_buf = jse_malloc(JSEPACK_BLOCK_SIZE);

    jse_debug("sizeof(fileheader) = %d \n", sizeof(fileheader));

    /* remove all js file */
    jse_rmdir(JSE_FS_ROOT_DIR);
}

void apppack_final()
{
    jspackcb = NULL;
    jse_free(jspackdst_buf);
    jspackdst_buf = NULL;
    jspack_done   = 1;
}

static jse_md5_context g_ctx;
static uint8_t digest[16]      = {0};
static int32_t app_file_offset = 0;

static void jspackoutput(const char *filename, const uint8_t *md5,
                         int32_t file_size, int32_t type, int32_t offset,
                         uint8_t *buf, int32_t buf_len)
{
    int i;
    int outsize;

    if (offset == 0) {
        jse_md5_init(&g_ctx);
        jse_md5_starts(&g_ctx);
        app_file_offset = 0;
    }

    jse_md5_update(&g_ctx, buf, buf_len);

    if (buf_len > 0) {
        outsize = buf_len;
        if (jspackcb) {
            jspackcb(filename, file_size, type, app_file_offset, buf, outsize,
                     0);
        }

        app_file_offset += outsize;
        buf_len -= outsize;
        buf += outsize;
    }

    int32_t complete = 0;

    /* end of file, check MD5 */
    if (file_size == app_file_offset) {
        jse_md5_finish(&g_ctx, digest);
        jse_md5_free(&g_ctx);

        jse_warn("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x "
                 "0x%x 0x%x 0x%x 0x%x",
                 digest[0], digest[1], digest[2], digest[3], digest[4],
                 digest[5], digest[6], digest[7], digest[8], digest[9],
                 digest[10], digest[11], digest[12], digest[13], digest[14],
                 digest[15]);

        jse_warn("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x "
                 "0x%x 0x%x 0x%x 0x%x",
                 md5[0], md5[1], md5[2], md5[3], md5[4], md5[5], md5[6], md5[7],
                 md5[8], md5[9], md5[10], md5[11], md5[12], md5[13], md5[14],
                 md5[15]);

        jspackfile_count--;
        if (jspackfile_count == 0) {
            jspack_done = 1;
        }

        if (memcmp(digest, md5, 16) == 0) {
            complete = 1; /* check success */
        } else {
            complete           = -1;
            jspack_found_error = 1;
            jspack_done        = 1;
        }

        if (jspackcb) {
            jspackcb(filename, app_file_offset, type, app_file_offset, NULL, 0,
                     complete);
        }
    }
}

#define JSEPACK_HEADER_SIZE 8  /* sizeof(JSEPACK_HEADER) */
#define JSEPACK_FILE_HEADER 24 /* sizeof(JSEPACK_FILE)  exclude filename */
static uint32_t g_file_header_size = 24 + 1;
static uint8_t *g_file_name        = NULL;

/* report process state */
void apppack_post_process_state()
{
    char msg[128];
    if (jspacksize >= JSEPACK_HEADER_SIZE) {
        sprintf(msg, "%d/%d", jspacksize, header.pack_size);
#ifdef JSE_IDE_DEBUG
        bone_websocket_send_frame("/ide/updateapp_process", 200, msg);
#endif
    }
}

/* analysis recursive */
int apppack_update(uint8_t *ptr, int size)
{
    int len = 0;
    uint8_t *pdst;

    if (jspack_found_error) return -1;

    if ((size < 1) || (jspack_done)) {
        return 0;
    }

    jse_debug("jspacksize = %d size=%d \n", jspacksize, size);
    jse_debug("jspackfile_header_offset = %d \n", jspackfile_header_offset);

    jse_debug("g_file_header_size = %d \n", g_file_header_size);

    /* parse file header */
    if (jspacksize == 0) {
        if (size > JSEPACK_HEADER_SIZE) {
            jspacksize = JSEPACK_HEADER_SIZE;
        } else {
            jspacksize = size;
        }

        memcpy(&header, ptr, jspacksize);
        if (jspacksize < JSEPACK_HEADER_SIZE) {
            return 0;
        }

        return apppack_update(ptr + jspacksize, size - jspacksize);
    } else if (jspacksize < JSEPACK_HEADER_SIZE) {
        len = JSEPACK_HEADER_SIZE - jspacksize;
        if (len > size) {
            len = size;
        }

        pdst = (uint8_t *)&header;
        memcpy(pdst + jspacksize, ptr, len);
        jspacksize += len;

        return apppack_update(ptr + len, size - len);
    } else if (jspacksize == JSEPACK_HEADER_SIZE) {
        /* start parse file header */

        jspackfile_count = header.file_count;
        jse_warn("file_count = %d ", header.file_count);
        jse_warn("pack_version = %d ", header.pack_version);
        jse_warn("pack_size = %d ", header.pack_size);

        /* reset jspackfile_header_offset */
        len = JSEPACK_FILE_HEADER; /* sizeof(JSEPACK_FILE) */
        if (len > size) {
            len = size;
        }

        memcpy(&fileheader, ptr, len);
        jspackfile_header_offset = len;
        jspacksize += len;
        jspackfile_offset = 0;

        return apppack_update(ptr + len, size - len);
    } else if (jspackfile_header_offset < JSEPACK_FILE_HEADER) {
        jse_warn("jspackfile_header_offset = %d size=%d \n",
                 jspackfile_header_offset, size);

        len = JSEPACK_FILE_HEADER -
              jspackfile_header_offset; /* sizeof(JSEPACK_FILE) */
        if (len > size) {
            len = size;
        }

        pdst = (uint8_t *)&fileheader;
        memcpy(pdst + jspackfile_header_offset, ptr, len);

        jspackfile_header_offset += len;
        jspacksize += len;
        jspackfile_offset = 0;

        return apppack_update(ptr + len, size - len);

    } else if (jspackfile_header_offset == JSEPACK_FILE_HEADER) {
        /* read g_file_header_size */

        jse_warn("file_size = %d ", fileheader.file_size);
        jse_warn("header_size = %d ", fileheader.header_size);

        g_file_header_size = fileheader.header_size;

        jse_warn("update g_file_header_size = %d ", g_file_header_size);

        jse_warn("filename len = %d ", g_file_header_size - JSEPACK_FILE_HEADER);

        if (g_file_name) jse_free(g_file_name);
        g_file_name =
            jse_calloc(1, g_file_header_size - JSEPACK_FILE_HEADER + 1);

        /* get file name */
        len = g_file_header_size - JSEPACK_FILE_HEADER;
        if (len > size) {
            /* part of file name */
            len = size;
        }

        memcpy(g_file_name, ptr, len);

        jspackfile_header_offset += len;
        jspacksize += len;

        return apppack_update(ptr + len, size - len);

    } else if (jspackfile_header_offset < g_file_header_size) {
        /* read rest length of file name */
        len = g_file_header_size - jspackfile_header_offset;
        if (len > size) {
            /* part of file name */
            len = size;
        }

        int offset = jspackfile_header_offset - JSEPACK_FILE_HEADER;

        jse_warn("get filename, offset = %d  len = %d \n", offset, len);

        memcpy(g_file_name + offset, ptr, len);

        jspackfile_header_offset += len;
        jspacksize += len;

        return apppack_update(ptr + len, size - len);

    } else if (jspackfile_header_offset == g_file_header_size) {
        if (jspackfile_offset == 0) {
            jse_warn("name = %s ", g_file_name);
            jse_warn("file_size = %d ", fileheader.file_size);

            len = fileheader.file_size;
        } else {
            len = fileheader.file_size - jspackfile_offset;
        }

        if (len > size) {
            len = size;
        }

        /* max length */
        if (len > JSEPACK_BLOCK_SIZE) {
            len = JSEPACK_BLOCK_SIZE;
        }

        /* parse file */
        jspackoutput(g_file_name, fileheader.md5, fileheader.file_size, 3,
                     jspackfile_offset, ptr, len);

        jspacksize += len;
        jspackfile_offset += len;

        jse_warn("jspackfile_offset = %d file_size = %d", jspackfile_offset,
                 fileheader.file_size);

        if (jspackfile_offset == fileheader.file_size) {
            /* next file */
            jspackfile_header_offset = 0;
            jspackfile_offset        = 0;
            /* reset g_file_header_size to default */
            g_file_header_size = 24 + 1;
            if (jspackfile_count > 0)
                jse_warn("parse next file\n");
            else
                jse_warn("app pack parse complete \n");
        }

        jse_warn("jspacksize = %d len = %d \n", jspacksize, len);

        return apppack_update(ptr + len, size - len);
    }

    jse_warn("jspackfile_header_offset = %d \n", jspackfile_header_offset);
    jse_warn("g_file_header_size = %d \n", g_file_header_size);
    jse_warn("apppack check file content fail\n");

    return -1;
}

#ifdef JSE_IDE_DEBUG
/**
 * @brief http_gethost_info
 *
 * @Param: src  url
 * @Param: web  WEB
 * @Param: file  download filename
 * @Param: port  default 80
 */
static void http_gethost_info(char *src, char **web, char **file, int *port)
{
    char *pa;
    char *pb;
    int isHttps = 0;

    if (!src || strlen(src) == 0) {
        jse_warn("http_gethost_info parms error!\n");
        return;
    }

    jse_warn("src = %s %d ", src, strlen(src));

    *port = 0;
    if (!(*src)) {
        return;
    }

    pa = src;
    if (!strncmp(pa, "https://", strlen("https://"))) {
        pa      = src + strlen("https://");
        isHttps = 1;
    }

    if (!isHttps) {
        if (!strncmp(pa, "http://", strlen("http://"))) {
            pa = src + strlen("http://");
        }
    }

    *web = pa;
    pb   = strchr(pa, '/');
    if (pb) {
        *pb = 0;
        pb += 1;
        if (*pb) {
            *file                   = pb;
            *((*file) + strlen(pb)) = 0;
        }
    } else {
        (*web)[strlen(pa)] = 0;
    }

    pa = strchr(*web, ':');
    if (pa) {
        *pa   = 0;
        *port = atoi(pa + 1);
    } else {
        /* TODO: support https:443
        if (isHttps) {
            *port = 80;
        } else {
            *port = 80;
        } */
        *port = 80;
    }
}

int apppack_download(char *url, download_js_cb_t func)
{
    int ret             = 0;
    int sockfd          = 0;
    int port            = 0;
    int nbytes          = 0;
    int send            = 0;
    int totalsend       = 0;
    uint32_t breakpoint = 0;
    int size            = 0;
    int header_found    = 0;
    char *pos           = 0;
    int file_size       = 0;
    char *host_file     = NULL;
    char *host_addr     = NULL;

    jse_warn("url = %s ", url);

    if (!url || strlen(url) == 0 || func == NULL) {
        jse_warn("ota_download parms error!\n");
        bone_websocket_send_frame("/device/updateapp_reply", 201,
                                  "Bad Request");
        return OTA_DOWNLOAD_INIT_FAIL;
    }

    char *http_buffer = jse_malloc(OTA_BUFFER_MAX_SIZE);

    jse_warn("http_buffer = %p", http_buffer);
    http_gethost_info(url, &host_addr, &host_file, &port);

    if (host_file == NULL || host_addr == NULL) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
        jse_free(http_buffer);
        bone_websocket_send_frame("/device/updateapp_reply", 201,
                                  "Bad Request");
        return ret;
    }

    jse_warn("ota_socket_connect  %d %s ", port, host_addr);

    sockfd = ota_socket_connect(port, host_addr);
    if (sockfd < 0) {
        jse_warn("ota_socket_connect error\n ");
        ret = OTA_DOWNLOAD_CON_FAIL;
        jse_free(http_buffer);
        bone_websocket_send_frame("/device/updateapp_reply", 205,
                                  "connect failed");
        return ret;
    }

    breakpoint = 0;
    sprintf(http_buffer, HTTP_HEADER, host_file, host_addr, port);

    send      = 0;
    totalsend = 0;
    nbytes    = strlen(http_buffer);

    jse_warn("send %s", http_buffer);

    while (totalsend < nbytes) {
        send = ota_socket_send(sockfd, http_buffer + totalsend,
                               nbytes - totalsend);
        if (send == -1) {
            jse_warn("send error!%s\n ", strerror(errno));
            ret = OTA_DOWNLOAD_REQ_FAIL;
            bone_websocket_send_frame("/device/updateapp_reply", 205,
                                      "download failed");
            goto DOWNLOAD_END;
        }
        totalsend += send;
        jse_warn("%d bytes send OK!\n ", totalsend);
    }

#ifdef LINUXOSX /* clean the root direcoty when user update app */
    be_osal_rmdir(JSE_FS_ROOT_DIR);
#else
    jse_unlink(JSE_FS_ROOT_DIR "/index.js");
#endif

    memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
    while ((nbytes = ota_socket_recv(sockfd, http_buffer,
                                     OTA_BUFFER_MAX_SIZE - 1)) != 0) {
        if (nbytes < 0) {
            jse_warn("ota_socket_recv nbytes < 0");
            if (errno != EINTR) {
                break;
            }
            if (ota_socket_check_conn(sockfd) < 0) {
                jse_warn("download system error %s", strerror(errno));
                break;
            } else {
                continue;
            }
        }

        if (!header_found) {
            if (!file_size) {
                char *ptr = strstr(http_buffer, "Content-Length:");
                if (ptr) {
                    sscanf(ptr, "%*[^ ]%d", &file_size);
                }
            }

            pos = strstr(http_buffer, "\r\n\r\n");
            if (!pos) {
                /* header pos */
                /* memcpy(headbuf, http_buffer, OTA_BUFFER_MAX_SIZE); */
            } else {
                pos += 4;
                int len      = pos - http_buffer;
                header_found = 1;
                size         = nbytes - len;
                func((uint8_t *)pos, size);

                if (size == file_size) {
                    nbytes = 0;
                    break;
                }
                memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
            }

            continue;
        }

        size += nbytes;
        func((uint8_t *)http_buffer, nbytes);

        if (size == file_size) {
            nbytes = 0;
            break;
        }

        memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
    }

    if (nbytes < 0) {
        jse_warn("download read error %s", strerror(errno));
        ret = OTA_DOWNLOAD_RECV_FAIL;
    } else if (nbytes == 0) {
        ret = OTA_SUCCESS;
    } else {
        /* can never reach here */
        /* ret = OTA_INIT_FAIL; */
    }

DOWNLOAD_END:
    ota_socket_close(sockfd);
    jse_free(http_buffer);
    return ret;
}

static int32_t update_done = 1;
static int app_fd          = -1;

int write_app_pack(const char *filename, int32_t file_size, int32_t type,
                   int32_t offset, uint8_t *buf, int32_t buf_len,
                   int32_t complete)
{
    /* char path[64]; */
    int ret;

    jse_debug("file_size=%d, offset = %d buf_len = %d complete = %d \n",
              file_size, offset, buf_len, complete);

    if (offset == 0) {
        app_fd = app_mgr_open_file(filename);
        jse_warn("app_mgr_open_file %s return  = %d ", filename, app_fd);
    }

    if (app_fd > 0) {
        if (buf_len > 0) {
            ret = jse_write(app_fd, buf, buf_len);
        }

        if ((offset + buf_len) == file_size) {
            ret    = jse_sync(app_fd);
            ret    = jse_close(app_fd);
            app_fd = -1;
            jse_warn("jse_close return %d", ret);
        }
    }

    if (complete != 0) {
        /* check failed */
        if (app_fd > 0) {
            ret    = jse_sync(app_fd);
            ret    = jse_close(app_fd);
            app_fd = -1;
        }
        jse_warn("file verify %s ", (complete == 1 ? "success" : "failed"));
        return -1;
    }

    return 0;
}

int download_apppack(uint8_t *buf, int32_t buf_len)
{
    jse_warn("download buf len = %d ", buf_len);
    apppack_update(buf, buf_len);
    jse_warn("apppack_post_process_state");
    apppack_post_process_state();
    return 0;
}

static void download_work(void *arg)
{
    int ret;

    jse_warn("download_work task name=%s", jse_osal_get_taskname());
    jse_warn("url=%s ", (char *)arg);

    ret = apppack_download((char *)arg, download_apppack);

    apppack_final();
    update_done = 1;

    jse_free(arg);

    if (ret == OTA_SUCCESS) {
        app_mgr_set_boneflag(1);
        jse_warn("Upgrade app success");
        bone_websocket_send_frame("/device/updateapp_reply", 200, "success");
        jse_osal_delay(200);
    }

    jse_warn("reboot ...");

    jse_system_reboot();
    jse_osal_delete_task(NULL);
}

int apppack_upgrade(char *url)
{
    jse_warn("apppack_upgrade url=%s \n", (char *)url);
    if (update_done) {
        update_done = 0;
        /* app_mgr_set_boneflag(0); */

        apppack_init(write_app_pack);

        jse_warn("create upgrade task ...");

        if (jse_osal_create_task("appupdate", download_work, url, 1024 * 4,
                                UPDATE_TSK_PRIORITY, NULL) != 0) {
            update_done = 1;
            apppack_final();
            jse_warn("jse_osal_task_new fail");
            bone_websocket_send_frame("/device/updateapp_reply", 203,
                                      "out of memory");
            return -1;
        }

    } else {
        jse_free(url);
        jse_warn("apppack upgrading...");
        bone_websocket_send_frame("/device/updateapp_reply", 204,
                                  "Busy,please try again later");
    }

    return 0;
}

#endif

#ifdef LINUXOSX
static int upgrading_mutex   = 0;
static int upgrade_file_size = 0;

int upgrade_simulator_reply(uint8_t *buf, int32_t buf_len)
{
    char msg[64]            = {0};
    static int last_buf_len = 0;
    static int total_recv   = 0;

    total_recv += buf_len;
    sprintf(msg, "%d/%d", total_recv, upgrade_file_size);

    if (((total_recv - last_buf_len) > OTA_BUFFER_MAX_SIZE * 2) ||
        total_recv >= upgrade_file_size) {
        jse_debug("upgrade_simulator_reply lastbuf=%d %s\n\r", last_buf_len,
                  msg);
#ifdef JSE_IDE_DEBUG
        bone_websocket_send_frame("/ide/updateimg_process", 200, msg);
#endif
        if (total_recv == upgrade_file_size) {
            last_buf_len = 0;
            total_recv   = 0;
        } else {
            last_buf_len = total_recv;
        }
    }
}
static void upgrade_simulator_work(upgrade_image_param_t *arg)
{
    int ret;

    jse_warn("url=%s ,size=%d", (char *)arg->url, arg->file_size);
    ret = apppack_download((char *)arg->url, upgrade_simulator_reply);
    upgrading_mutex = 0;
    jse_free(arg);
    jse_free(arg->url);
    if (ret == OTA_DOWNLOAD_FINISH) {
        jse_warn("Upgrade app success");
#ifdef JSE_IDE_DEBUG
        bone_websocket_send_frame("/device/updateimg_reply", 200, "success");
#endif
        jse_osal_delay(200);
    }

    jse_warn("reboot ...");

    jse_system_reboot();
    jse_osal_exit_task(0);
}

int simulator_upgrade(upgrade_image_param_t *p_info)
{
    jse_warn("simulator_upgrade url=%s %d\n", p_info->url, p_info->file_size);
    if (upgrading_mutex == 0) {
        upgrading_mutex   = 1;
        upgrade_file_size = p_info->file_size;
        jse_warn("simulator_upgrade ...");

        if (jse_osal_new_task("simulator_upgrade", upgrade_simulator_work,
                             p_info, 1024 * 4, NULL) != 0) {
            jse_warn("jse_osal_task_new fail");
#ifdef JSE_IDE_DEBUG
            bone_websocket_send_frame("/device/updateimg_reply", 203,
                                      "out of memory");
#endif
            upgrading_mutex = 0;
            return -1;
        }

    } else {
        jse_free(p_info);
        jse_warn("simulator_upgrading...");
#ifdef JSE_IDE_DEBUG
        bone_websocket_send_frame("/device/updateimg_reply", 204,
                                  "Busy, please try again later");
#endif
    }

    return 0;
}
#endif

void app_mgr_set_boneflag(int enable)
{
    jse_system_kv_set(BoneFlag, &enable, 4, 1);
}

int app_mgr_get_boneflag()
{
    int flag = 0;
    int len  = 4;
    jse_system_kv_get(BoneFlag, &flag, &len);
    return flag;
}

/*
max length 192
*/

void app_mgr_set_devicespec(char *jsonstr)
{
    jse_system_kv_set(DeviceSpec, jsonstr, strlen(jsonstr), 1);
}

int app_mgr_get_devicespec(char *jsonstr, int jsonstrlen)
{
    jse_system_kv_get(DeviceSpec, jsonstr, &jsonstrlen);
    return jsonstrlen;
}

int app_mgr_open_file(const char *targetname)
{
    int fd;
    char path[256] = {0};

    if (targetname == NULL) {
        return -1;
    }
    snprintf(path, sizeof(path), "%s/", JSE_FS_ROOT_DIR);
    if (targetname[0] == '.') {
        if (targetname[1] == '/') {
            strncat(path, targetname + 2, sizeof(path) - 1);
        } else {
            /* .aaa  not support hide file */
            return -1;
        }
    } else {
        strncat(path, targetname, sizeof(path) - 1);
    }

    int i   = strlen(JSE_FS_ROOT_DIR); /* 8 */
    int len = strlen(path);
    for (; i < len; i++) {
        if (path[i] == '/') {
            path[i] = 0;
            jse_mkdir(path);
            path[i] = '/';
        }
    }

    fd = jse_open(path, O_RDWR | O_CREAT | O_TRUNC);

    return fd;
}
