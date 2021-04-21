//
// Copyright (c) 2020 Alibaba All rights reserved.
//

//#include <stdio.h>
//#include <dirent.h>
//#include <string.h>
//#include <errno.h>
//#include <unistd.h>
//#include <sys/stat.h>
//#include <stdbool.h>
//
//#include <sys/fcntl.h>
//
//#ifdef WITH_LWIP
//#include "httpclient.h"
//#else
//#define WITH_LWIP
//#include "httpclient.h"
//#undef WITH_LWIP
//#endif
//
//#include <string>
//#include "utils/log.h"
#include "jquick_network.h"

//static const char* DEFAULT_REQ_HEADER = "Accept: */*\r\n";  ///"Content-Type: application/json\r\n";
//static const std::string DEFAULT_REQ_METHOD = "GET";
//static const std::string DEFAULT_REQ_HEADER_KEY = "Content-Type: ";
//
//#ifndef HTTP_FILE_PATH_LEN
//#define HTTP_FILE_PATH_LEN 300
//#endif

int jquick_network_is_connect()
{
    // Todo: pls add the implement on the anyka
    //
    return 1;
}

long jquick_network_download_url_to_file(const char* url, const char* filePath)
{
//    std::string err;
//    std::string method = DEFAULT_REQ_METHOD;
//    std::string content_type;
//    std::string req_header = DEFAULT_REQ_HEADER_KEY;
//    const char* req_header_cstr = DEFAULT_REQ_HEADER;
//    int total_len = 0, recv_len = 0, recv_total_len = 0;
//    int file_fd = -1;
//    int fileSize = -1;
//    int ret = 0;
//    httpclient_source_t http_source;
//
//    LOGD("[debug_downloader] url: %s, output: %s\n",
//         url, filePath);
//
//    memset(&http_source, 0, sizeof(http_source));
//
//    if (!url) {
//        LOGE("[debug_downloader]Empty url!");
//        goto exit;
//    }
//
//    if (!content_type.empty()) {
//        req_header.append(content_type);
//        req_header.append("\r\n");
//        req_header_cstr = req_header.c_str();
//    }
//
//    httpclient_prepare(&http_source, 1024, 8 * 1024);
//    httpclient_set_custom_header(&http_source.client, (char*)req_header_cstr);
//
//    ret = httpclient_connect(&http_source.client, (char*)url);
//    if (HTTPCLIENT_OK != ret) {
//        LOGE("[debug_downloader]http connect failed ret %d", ret);
//        goto exit;
//    }
//
//    ret = httpclient_send_request(&http_source.client, (char*)url,
//                                  HTTPCLIENT_GET, &http_source.client_data);
//    if (HTTPCLIENT_OK != ret) {
//        LOGE("[debug_downloader]http send request failed");
//        goto exit;
//    }
//
//    while (0 == total_len || recv_total_len < total_len) {
//        ret = httpclient_recv_response(&http_source.client, &http_source.client_data);
//        if (HTTPCLIENT_OK == ret || HTTPCLIENT_RETRIEVE_MORE_DATA == ret ||
//            HTTPCLIENT_CLOSED == ret) {
//            if (0 == total_len) {
//                total_len = http_source.client_data.response_content_len;
//
//                if (total_len <= 0) {
//                    LOGE("[debug_downloader]Invalid length %d", total_len);
//                    break;
//                }
//
//                LOGD("[debug_downloader]http total content len %d", total_len);
//
//                file_fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC);
//                if (file_fd < 0) {
//                    LOGE("[debug_downloader]open http tmp file %s failed %d", filePath, file_fd);
//                    ret = -1;
//                    goto exit;
//                }
//            }
//
//            recv_len = http_source.client_data.content_block_len;
//            if (recv_len > 0) {
//                /* write to file */
//                ret = write(file_fd, http_source.client_data.response_buf, recv_len);
//                if (ret < 0) {
//                    LOGE("[debug_downloader]write file %s fail, size = %d, ret: %d\n",
//                         filePath, recv_len, ret);
//
//                    goto exit;
//                }
//
//                // LOGD("[debug_downloader]http recv len %d\n", recv_len);
//
//                recv_total_len += recv_len;
//            }
//
//            if (HTTPCLIENT_CLOSED == ret) {
//                break;
//            }
//        } else {
//            LOGE("[debug_downloader]http recv failed %d ", ret);
//            goto exit;
//        }
//    }
//
//    if (total_len > 0 && recv_total_len == total_len) {
//        fileSize = total_len;
//    }
//
//    exit:
//    close(file_fd);
//    httpclient_close(&http_source.client);
//    httpclient_unprepare(&http_source);
//
//    LOGD("[debug_downloader] download %s complent, file size %lld\n",
//         url, fileSize);
//
//    return fileSize;
    return -1;
}
