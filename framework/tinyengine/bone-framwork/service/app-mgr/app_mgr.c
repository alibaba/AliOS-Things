/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "app_mgr.h"
#include <be_osal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <utils/be_common.h>
#include "ota_download.h"
#include "ota_log.h"
#include "ota_socket.h"
#include "ota_update_manifest.h"
#include "ota_util.h"

#define OTA_BUFFER_MAX_SIZE 1536
#define HTTP_HEADER \
    "GET /%s HTTP/1.1\r\nAccept:*/*\r\n\
    User-Agent: Mozilla/5.0\r\n\
    Cache-Control: no-cache\r\n\
    Connection: close\r\n\
    Host:%s:%d\r\n\r\n"

static void *aes_ctx = NULL;

// app pack, JS文件内容使用aes ecb 128bit加密，秘钥需要与打包程序一致
static const uint8_t aes_key[] = {0x49, 0x6f, 0x54, 0x2d, 0x41, 0x45,
                                  0x53, 0x2d, 0x4b, 0x45, 0x59, 0x30,
                                  0x30, 0x30, 0x30, 0x31};

typedef struct {
    uint16_t file_count;
    uint16_t pack_version;
    uint32_t pack_size;
} JSEPACK_HEADER;

typedef struct {
    uint8_t name[16 + 16];
    uint32_t file_type : 8;
    uint32_t file_size : 24;
    uint8_t md5[16];
    // uint8 data[];
} JSEPACK_FILE;

static write_js_cb_t jspackcb = NULL;
static JSEPACK_HEADER header;
static JSEPACK_FILE fileheader;
static int32_t jspacksize;

static int32_t jspackfile_offset;
static int32_t jspackfile_header_offset;
static int32_t jspackfile_count;
static int32_t jspack_done = 0;

#define JSEPACK_BLOCK_SIZE 2 * 1024

static uint8_t *jspackdst_buf = NULL;

void apppack_init(write_js_cb_t cb) 
{
    jspackcb = cb;
    jspacksize = 0;
    jspackfile_offset = 0;
    jspackfile_header_offset = 0;
    jspack_done = 0;
    jspackfile_count = 0;

    int size = 0;
    ali_aes_get_ctx_size(AES_ECB, &size);

    aes_ctx = malloc(size);
    jspackdst_buf = malloc(JSEPACK_BLOCK_SIZE);
}

void apppack_final() 
{
    jspackcb = NULL;
    free(aes_ctx);
    aes_ctx = NULL;
    free(jspackdst_buf);
    jspackdst_buf = NULL;
    jspack_done = 1;
}

static MD5_CTX g_ctx;
static uint8_t digest[16] = {0};

// aes数据操作需要16Byte对齐，剩余数据暂存
static uint8_t aes_tmp[16];
static int32_t aes_tmp_len = 0;
static int32_t app_file_offset = 0;

static void jspackoutput(const char *filename, const uint8_t *md5,
                         int32_t file_size, int32_t type, int32_t offset,
                         uint8_t *buf, int32_t buf_len) 
{
    int i;
    int outsize;

    if (offset == 0) {
        MD5_Init(&g_ctx);
        ali_aes_reset(aes_ctx);
        ali_aes_init(AES_ECB, false, aes_key, NULL, 16, NULL, aes_ctx);
        aes_tmp_len = 0;
        app_file_offset = 0;
    }

    MD5_Update(&g_ctx, buf, buf_len);

    if (aes_tmp_len > 0) {
        i = 16 - aes_tmp_len;
        if (i > buf_len) i = buf_len;

        memcpy(aes_tmp + aes_tmp_len, buf, i);

        aes_tmp_len += i;
        buf_len -= i;
        buf += i;

        if (aes_tmp_len == 16) {
            aes_tmp_len = 0;
            memset(jspackdst_buf, 0, JSEPACK_BLOCK_SIZE);
            i = ali_aes_process(aes_tmp, jspackdst_buf, 16, aes_ctx);
            if (jspackcb)
                jspackcb(filename, file_size, type, app_file_offset,
                         jspackdst_buf, 16, 0);
            app_file_offset += 16;
        }
    }

    if (buf_len > 0) {
        i = buf_len / 16;
        i = i * 16;

        if (i > 0) {
            memset(jspackdst_buf, 0, JSEPACK_BLOCK_SIZE);
            ali_aes_process(buf, jspackdst_buf, i, aes_ctx);
            outsize = i;

            if (jspackcb) {
                jspackcb(filename, file_size, type, app_file_offset,
                         jspackdst_buf, outsize, 0);
            }

            app_file_offset += i;
            buf_len -= i;
            buf += i;
        }
    }

    // 剩余数据
    if (buf_len > 0) {
        memcpy(aes_tmp, buf, buf_len);
        aes_tmp_len = buf_len;
    }

    int32_t complete = 0;

    // 文件结束，校验MD5
    if (file_size == app_file_offset) {
        MD5_Final(digest, &g_ctx);

        be_warn("APP_MGR",
                "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x "
                "0x%x 0x%x 0x%x 0x%x",
                digest[0], digest[1], digest[2], digest[3], digest[4],
                digest[5], digest[6], digest[7], digest[8], digest[9],
                digest[10], digest[11], digest[12], digest[13], digest[14],
                digest[15]);

        be_warn("APP_MGR",
                "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x "
                "0x%x 0x%x 0x%x 0x%x",
                md5[0], md5[1], md5[2], md5[3], md5[4], md5[5], md5[6], md5[7],
                md5[8], md5[9], md5[10], md5[11], md5[12], md5[13], md5[14],
                md5[15]);

        jspackfile_count--;
        if (jspackfile_count == 0) {
            jspack_done = 1;
        }

        if (memcmp(digest, md5, 16) == 0) {
            complete = 1;
        } else {
            complete = -1;
        }

        if (jspackcb) {
            jspackcb(filename, app_file_offset, type, app_file_offset, NULL, 0,
                     complete);
        }
    }
}

#define JSEPACK_HEADER_SIZE 8   // sizeof(JSEPACK_HEADER)
#define JSEPACK_FILE_HEADER 52  // sizeof(JSEPACK_FILE)

int apppack_update(uint8_t *ptr, int size) 
{
    int len = 0;
    uint8_t *pdst;

    if ((size < 1) || (jspack_done)) {
        return 0;
    }

    if (jspacksize == 0) {
        if (size > JSEPACK_HEADER_SIZE) {
            jspacksize = JSEPACK_HEADER_SIZE;
        } else {
            jspacksize = size;
        }

        memcpy(&header, ptr, jspacksize);
        if (jspacksize < JSEPACK_HEADER_SIZE) {
            return;
        }

        return apppack_update(ptr + jspacksize, size - jspacksize);
    } else if (jspacksize < JSEPACK_HEADER_SIZE) {
        len = JSEPACK_HEADER_SIZE - jspacksize;
        if (len > size) {
            len = size;
        }

        pdst = &header;
        memcpy(pdst + jspacksize, ptr, len);
        jspacksize += len;

        return apppack_update(ptr + len, size - len);
    } else if (jspacksize == JSEPACK_HEADER_SIZE) {
        // 开始分析fileheader

        jspackfile_count = header.file_count;
        be_warn("APP_MGR", "file_count = %d ", header.file_count);
        be_warn("APP_MGR", "pack_version = %d ", header.pack_version);
        be_warn("APP_MGR", "pack_size = %d ", header.pack_size);

        len = JSEPACK_FILE_HEADER;  // sizeof(JSEPACK_FILE)
        if (len > size) {
            len = size;
        }

        memcpy(&fileheader, ptr, len);
        jspackfile_header_offset = len;
        jspacksize += len;

        return apppack_update(ptr + len, size - len);
    } else if (jspackfile_header_offset < JSEPACK_FILE_HEADER) {
        len = JSEPACK_FILE_HEADER -
              jspackfile_header_offset;  // sizeof(JSEPACK_FILE)
        if (len > size) {
            len = size;
        }

        pdst = &fileheader;
        memcpy(pdst + jspackfile_header_offset, ptr, len);

        jspackfile_header_offset += len;
        jspacksize += len;
        jspackfile_offset = 0;

        return apppack_update(ptr + len, size - len);
    } else if (jspackfile_header_offset == JSEPACK_FILE_HEADER) {
        if (jspackfile_offset == 0) {
            be_warn("APP_MGR", "name = %s ", fileheader.name);
            be_warn("APP_MGR", "file_size = %d ", fileheader.file_size);
            be_warn("APP_MGR", "file_type = %d ", fileheader.file_type);

            len = fileheader.file_size;
        } else {
            len = fileheader.file_size - jspackfile_offset;
        }

        if (len > size) {
            len = size;
        }

        // 限长
        if (len > JSEPACK_BLOCK_SIZE) {
            len = JSEPACK_BLOCK_SIZE;
        }

        // 分析文件
        jspackoutput(fileheader.name, fileheader.md5, fileheader.file_size,
                     fileheader.file_type, jspackfile_offset, ptr, len);

        jspacksize += len;
        jspackfile_offset += len;

        if (jspackfile_offset == fileheader.file_size) {
            // 下一个文件
            jspackfile_header_offset = 0;
            jspackfile_offset = 0;
        }

        return apppack_update(ptr + len, size - len);
    }

    be_warn("APP_MGR", "apppack check file content fail\n");

    return -1;
}

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

    be_warn("APP_MGR", "src = %s %d ", src, strlen(src));

    if (!src || strlen(src) == 0) {
        be_warn("APP_MGR", "http_gethost_info parms error!\n");
        return;
    }

    *port = 0;
    if (!(*src)) {
        return;
    }

    pa = src;
    if (!strncmp(pa, "https://", strlen("https://"))) {
        pa = src + strlen("https://");
        isHttps = 1;
    }

    if (!isHttps) {
        if (!strncmp(pa, "http://", strlen("http://"))) {
            pa = src + strlen("http://");
        }
    }

    *web = pa;
    pb = strchr(pa, '/');
    if (pb) {
        *pb = 0;
        pb += 1;
        if (*pb) {
            *file = pb;
            *((*file) + strlen(pb)) = 0;
        }
    } else {
        (*web)[strlen(pa)] = 0;
    }

    pa = strchr(*web, ':');
    if (pa) {
        *pa = 0;
        *port = atoi(pa + 1);
    } else {
        if (isHttps) {
            *port = 80;  // 443
        } else {
            *port = 80;
        }
    }
}

int apppack_download(char *url, download_js_cb_t func) 
{
    int ret = 0;
    int sockfd = 0;
    int port = 0;
    int nbytes = 0;
    int send = 0;
    int totalsend = 0;
    uint32_t breakpoint = 0;
    int size = 0;
    int header_found = 0;
    char *pos = 0;
    int file_size = 0;
    char *host_file = NULL;
    char *host_addr = NULL;

    be_warn("APP_MGR", "url = %s ", url);

    if (!url || strlen(url) == 0 || func == NULL) {
        be_warn("APP_MGR", "ota_download parms error!\n");
        return OTA_DOWNLOAD_URL_FAIL;
    }

    char *http_buffer = malloc(OTA_BUFFER_MAX_SIZE);

    be_warn("APP_MGR", "http_buffer = %p", http_buffer);
    http_gethost_info(url, &host_addr, &host_file, &port);

    if (host_file == NULL || host_addr == NULL) {
        ret = OTA_DOWNLOAD_URL_FAIL;
        free(http_buffer);
        return ret;
    }

    be_warn("APP_MGR", "ota_socket_connect  %d %s ", port, host_addr);

    sockfd = ota_socket_connect(port, host_addr);
    if (sockfd < 0) {
        be_warn("APP_MGR", "ota_socket_connect error\n ");
        ret = OTA_DOWNLOAD_SOCKET_FAIL;
        free(http_buffer);
        return ret;
    }

    breakpoint = 0;
    sprintf(http_buffer, HTTP_HEADER, host_file, host_addr, port);

    send = 0;
    totalsend = 0;
    nbytes = strlen(http_buffer);

    be_warn("APP_MGR", "send %s", http_buffer);

    while (totalsend < nbytes) {
        send = ota_socket_send(sockfd, http_buffer + totalsend,
                               nbytes - totalsend);
        if (send == -1) {
            be_warn("APP_MGR", "send error!%s\n ", strerror(errno));
            ret = OTA_DOWNLOAD_SEND_FAIL;
            goto DOWNLOAD_END;
        }
        totalsend += send;
        be_warn("APP_MGR", "%d bytes send OK!\n ", totalsend);
    }

    memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
    while ((nbytes = ota_socket_recv(sockfd, http_buffer,
                                     OTA_BUFFER_MAX_SIZE - 1)) != 0) {
        if (nbytes < 0) {
            be_warn("APP_MGR", "ota_socket_recv nbytes < 0");
            if (errno != EINTR) {
                break;
            }
            if (ota_socket_check_conn(sockfd) < 0) {
                be_warn("APP_MGR", "download system error %s", strerror(errno));
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
                // header pos
                // memcpy(headbuf, http_buffer, OTA_BUFFER_MAX_SIZE);
            } else {
                pos += 4;
                int len = pos - http_buffer;
                header_found = 1;
                size = nbytes - len;
                func((uint8_t *)pos, size);
            }

            continue;
        }

        size += nbytes;
        func((uint8_t *)http_buffer, nbytes);
        memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);

        if (size == file_size) {
            nbytes = 0;
            break;
        }

        memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
    }

    if (nbytes < 0) {
        be_warn("APP_MGR", "download read error %s", strerror(errno));
        ret = OTA_DOWNLOAD_FAILED;
    } else if (nbytes == 0) {
        ret = OTA_DOWNLOAD_FINISH;
    } else {
        ret = OTA_DOWNLOAD_CANCEL;
    }

DOWNLOAD_END:
    ota_socket_close(sockfd);
    free(http_buffer);
    return ret;
}

static int32_t update_done = 1;
static int app_fd = -1;

int write_app_pack(const char *filename, int32_t file_size, int32_t type,
                        int32_t offset, uint8_t *buf, int32_t buf_len,
                        int32_t complete) 
{
    char path[64];
    int ret;

    if (complete == -1) {
        // 校验出错
        if (app_fd > 0) {
            ret = be_osal_sync(app_fd);
            ret = be_osal_close(app_fd);
            app_fd = -1;
            be_warn("APP_UPDATE", "file verify failed ");
        }
        return -1;
    }

    if (offset == 0) {
        if (type == 0) {
            snprintf(path, sizeof(path), "%s/%s", BE_FS_ROOT_DIR, filename);
        } else {
            snprintf(path, sizeof(path), "%s/drivers/%s", BE_FS_ROOT_DIR,
                     filename);
        }

        app_fd = be_osal_open(path, O_RDWR | O_CREAT | O_TRUNC);
        be_warn("APP_UPDATE", "be_osal_open %s return  = %d ", path, app_fd);
    }

    if (app_fd > 0) {
        if (buf_len > 0) {
            ret = be_osal_write(app_fd, buf, buf_len);
        }

        if ((offset + buf_len) == file_size) {
            ret = be_osal_sync(app_fd);
            ret = be_osal_close(app_fd);
            app_fd = -1;
            be_warn("APP_UPDATE", "be_osal_close return %d", ret);
        }
    }

    return 0;
}

int download_apppack(uint8_t *buf, int32_t buf_len) 
{
    be_warn("APP_UPDATE", "download buf len = %d ", buf_len);
    apppack_update(buf, buf_len);

    return 0;
}

static void download_work(void *arg) 
{
    int ret;

    be_warn("APP_UPDATE", "download_work aos_task_name=%s \r\n",
            be_osal_task_name());
    be_warn("APP_UPDATE", "url=%s \r\n", (char *)arg);
    ret = apppack_download((char *)arg, download_apppack);
    apppack_final();
    update_done = 1;

    if (ret == OTA_DOWNLOAD_FINISH) {
        app_mgr_set_boneflag(1);
    }

    be_osal_exit_task(0);
}

int apppack_upgrade(char *url) 
{
    if (update_done) {
        update_done = 0;
        app_mgr_set_boneflag(0);
        apppack_init(write_app_pack);
        if (aos_task_new("appupdate", download_work, url, 1024 * 4) != 0) {
            update_done = 1;
            apppack_final();
            be_warn("APP_UPDATE", "aos_task_new fail");
            return -1;
        }
    } else {
        be_warn("APP_UPDATE", "apppack upgrading...");
    }

    return 0;
}

void app_mgr_set_boneflag(int flag) 
{ 
    be_osal_kv_set(BoneFlag, &flag, 4, 1); 
}

int app_mgr_get_boneflag() 
{
    int flag = 0;
    int len = 4;
    be_osal_kv_get(BoneFlag, &flag, &len);
    return flag;
}

/*
{
    "productKey": "a1l42fkZvj8",
    "DeviceSecret": "zxMnkmAKVn7fW9m3HUYIPR5SqzpbVQOr",
    "DeviceName": "ESP32-0001"
}
max length 192
*/

void app_mgr_set_devicespec(char *jsonstr) 
{
    be_osal_kv_set(DeviceSpec, jsonstr, strlen(jsonstr), 1);
}

int app_mgr_get_devicespec(char *jsonstr, int jsonstrlen) 
{
    be_osal_kv_get(DeviceSpec, jsonstr, &jsonstrlen);
    return jsonstrlen;
}
