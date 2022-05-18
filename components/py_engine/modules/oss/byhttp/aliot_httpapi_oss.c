#include "aliot_httpapi_oss.h"

#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "py/mperrno.h"
#include "shared/timeutils/timeutils.h"

#define TAG  "OSS_HTTP"

#define CRLF "\r\n"

/* 获取星期 */
const char *getWeekdayByYearday(int32_t iY, int32_t iM, int32_t iD)
{
    int32_t iWeekDay = -1;
    if (1 == iM || 2 == iM) {
        iM += 12;
        iY--;
    }
    iWeekDay = (iD + 1 + 2 * iM + 3 * (iM + 1) / 5 + iY + iY / 4 - iY / 100 + iY / 400) % 7;

    switch (iWeekDay) {
    case 0:
        return "Sun";
    case 1:
        return "Mon";
    case 2:
        return "Tue";
    case 3:
        return "Wed";
    case 4:
        return "Thu";
    case 5:
        return "Fri";
    case 6:
        return "Sat";
    default:
        return NULL;
    }

    return NULL;
}

/* 获取月份 */
const char *getMothStr(int32_t moth)
{
    switch (moth) {
    case 1:
        return "Jan";
    case 2:
        return "Feb";
    case 3:
        return "Mar";
    case 4:
        return "Apr";
    case 5:
        return "May";
    case 6:
        return "Jun";
    case 7:
        return "Jul";
    case 8:
        return "Aug";
    case 9:
        return "Sep";
    case 10:
        return "Oct";
    case 11:
        return "Nov";
    case 12:
        return "Dec";
    default:
        return NULL;
    }
}

static int32_t httpclient_put_file(httpclient_t *client, const char *url, const char *localPath,
                                   httpclient_data_t *client_data)
{
    int32_t ret = 0;
    uint8_t buf[MAX_HTTP_OUTPUT_BUFFER] = { 0 };

    if (NULL == client || NULL == url || NULL == localPath || NULL == client_data) {
        LOGE(TAG, "params null");
        return -EINVAL;
    }

    FILE *fd = fopen(localPath, "r");
    if (fd == NULL) {
        LOGE(TAG, "failed to open file:%s", localPath);
        return -ENOENT;
    }

    ret = httpclient_conn(client, url);
    if (ret == HTTP_SUCCESS) {
        ret = httpclient_send(client, url, HTTP_PUT, client_data);
        if (ret == HTTP_SUCCESS) {
            do {
                int32_t read_size = fread(buf, 1, sizeof(buf), fd);
                if (read_size > 0) {
                    // pass do nothing
                } else if (read_size < 0) {
                    // if num < 0 read file eror
                    ret = read_size;
                    goto exit;
                } else if (read_size == 0) {
                    // if num == 0 read file eof
                    fclose(fd);
                    fd = NULL;
                    break;
                }
                ret = http_tcp_send_wrapper(client, buf, read_size);
                if (ret < 0) {
                    goto exit;
                }
            } while (1);

            ret = httpclient_recv(client, client_data);
        }
    }

exit:
    if (fd != NULL) {
        fclose(fd);
    }

    httpclient_clse(client);
    return ret;
}

static int32_t httpclient_put_content(httpclient_t *client, const char *url, const char *content_data,
                                      httpclient_data_t *client_data)
{
    int32_t ret = 0;
    int32_t num = 0;
    if (NULL == client || NULL == url || NULL == content_data || NULL == client_data) {
        return -EINVAL;
    }

    ret = httpclient_conn(client, url);
    if (ret == HTTP_SUCCESS) {
        ret = httpclient_send(client, url, HTTP_PUT, client_data);
        if (ret == HTTP_SUCCESS) {
            num = strlen(content_data) + 2;
            ret = http_tcp_send_wrapper(client, content_data, num);
            if (ret < 0) {
                goto exit;
            }

            ret = httpclient_recv(client, client_data);
        }
    }

exit:
    httpclient_clse(client);
    return ret;
}

static int32_t httpclient_get_file(httpclient_t *client, const char *url, const char *localPath,
                                   httpclient_data_t *client_data)
{
    int32_t num = 0;
    int32_t ret;
    FILE *fd = fopen(localPath, "w+");
    if (fd == 0) {
        return -EPERM;
    }

    ret = httpclient_conn(client, url);
    if (ret == HTTP_SUCCESS) {
        ret = httpclient_send(client, url, HTTP_GET, client_data);
        if (ret != HTTP_SUCCESS) {
            goto exit;
        }
        do {
            ret = httpclient_recv(client, client_data);
            if (ret < 0) {
                goto exit;
            }

            num = fwrite(client_data->response_buf, 1, client_data->content_block_len, fd);
            if (num <= 0) {
                ret = num;
                goto exit;
            }
        } while (client_data->is_more);
    }

exit:
    if (fd != NULL) {
        fclose(fd);
    }

    httpclient_clse(client);
    return ret;
}

int32_t oss_http_put_object(const char *key, const char *secret, const char *endPoint, const char *BucketName,
                            const char *ObjectName, const char *LocalPath, char *resbuf, uint32_t reslen)

{
    httpclient_t client = { 0 };
    httpclient_data_t client_data = { 0 };
    char url[256] = { 0 };
    char puthead[256] = { 0 };
    char signmsg[128] = { 0 };
    int32_t ret = 0;
    char signbin[64] = { 0 };
    char sign[64] = { 0 };
    uint32_t outputLength = 0;
    long LocalfileSize = 0;
    struct stat tmpstat = { 0 };

    /* 获取时间 */
    struct timeval tv = { 0 };
    timeutils_struct_time_t datetime = { 0 };
    gettimeofday(&tv, NULL);
    timeutils_seconds_since_epoch_to_struct_time(tv.tv_sec, &datetime);

    const char *gmtweek = getWeekdayByYearday(datetime.tm_year, datetime.tm_mon, datetime.tm_mday);
    const char *gmtmon = getMothStr(datetime.tm_mon);

    /* 签名认证组包 */
    sprintf(signmsg, "PUT\n\ntext/plain\n%s, %02ld %s %04ld %02ld:%02ld:%02ld GMT\n/%s/%s", gmtweek,
            (long)datetime.tm_mday, gmtmon, (long)datetime.tm_year, (long)datetime.tm_hour, (long)datetime.tm_min,
            (long)datetime.tm_sec, BucketName, ObjectName);
    LOGD(TAG, "signmsg:%s\r\n", signmsg);
    /* 签名认证 */

    aliot_hmac_sha1(signmsg, strlen(signmsg), signbin, secret, strlen(secret));

    /* 签名认证之后的base64数据格式转换 */
    aliot_base64encode((const uint8_t *)signbin, strlen(signbin), sizeof(sign), (uint8_t *)sign,
                       &outputLength);  // mod by wy 210428(sign类型不匹配)
    LOGD(TAG, "sign:%s\r\n", sign);

    /* 组合url */
    sprintf(url, "http://%s.%s/%s", BucketName, endPoint, ObjectName);
    LOGD(TAG, "url:%s\r\n", url);

    ret = stat(LocalPath, &tmpstat);
    if (ret < 0) {
        return -1;
    }
    LocalfileSize = tmpstat.st_size + 2;

    /* 请求头组包 */
    sprintf(puthead,
            "Authorization: OSS %s:%s" CRLF "Date: %s, %02ld %s %04ld %02ld:%02ld:%02ld GMT" CRLF
            "Content-Type: text/plain" CRLF "Content-Length: %ld" CRLF CRLF,
            key, sign, gmtweek, (long)datetime.tm_mday, gmtmon, (long)datetime.tm_year, (long)datetime.tm_hour,
            (long)datetime.tm_min, (long)datetime.tm_sec, LocalfileSize);  // mod by wy 210428(%2d-->%2ld)
    LOGD(TAG, "puthead:%s\r\n", puthead);

    client.header = puthead;
    client_data.response_buf = resbuf;
    client_data.response_buf_len = reslen;
    client_data.header_buf = resbuf;
    client_data.header_buf_len = reslen;

    /* 获取文件并PUT请求连接HTTP服务器 */
    ret = (int32_t)httpclient_put_file(&client, url, LocalPath, &client_data);
    return ret;
}

int32_t oss_http_get_object(const char *key, const char *secret, const char *endPoint, const char *BucketName,
                            const char *ObjectName, const char *LocalPath, char *range, char *resbuf, uint32_t reslen)
{
    httpclient_t client = { 0 };
    httpclient_data_t client_data = { 0 };
    char url[256] = { 0 };
    char gethead[258] = { 0 };
    char signmsg[128] = { 0 };
    int32_t ret = 0;
    char signbin[128] = { 0 };
    char sign[64] = { 0 };
    uint32_t outputLength = 0;

    /* 获取当前时间 */
    struct timeval tv = { 0 };
    timeutils_struct_time_t datetime = { 0 };
    gettimeofday(&tv, NULL);
    timeutils_seconds_since_epoch_to_struct_time(tv.tv_sec, &datetime);

    const char *gmtweek = getWeekdayByYearday(datetime.tm_year, datetime.tm_mon, datetime.tm_mday);
    const char *gmtmon = getMothStr(datetime.tm_mon);

    /* 签名认证组包 */
    sprintf(signmsg, "GET\n\n\n%s, %02ld %s %04ld %02ld:%02ld:%02ld GMT\n/%s/%s", gmtweek, (long)datetime.tm_mday,
            gmtmon, (long)datetime.tm_year, (long)datetime.tm_hour, (long)datetime.tm_min, (long)datetime.tm_sec,
            BucketName, ObjectName);
    LOGD(TAG, "signmsg:%s\r\n", signmsg);

    /* 签名认证 */
    aliot_hmac_sha1(signmsg, strlen(signmsg), signbin, secret, strlen(secret));

    /* 签名认证之后的base64数据格式转换 */
    aliot_base64encode((const uint8_t *)signbin, strlen(signbin), sizeof(sign), (uint8_t *)sign,
                       &outputLength);  // mod by wy 210428 (类型不匹配)
    LOGD(TAG, "sign:%s\r\n", sign);

    /* 组合url */
    sprintf(url, "http://%s.%s/%s", BucketName, endPoint, ObjectName);
    LOGD(TAG, "url:%s\r\n", url);

    /* 请求头组包 */
    if (range == NULL) {
        sprintf(gethead, "Authorization: OSS %s:%s" CRLF "Date: %s, %02ld %s %04ld %02ld:%02ld:%02ld GMT" CRLF CRLF,
                key, sign, gmtweek, (long)datetime.tm_mday, gmtmon, (long)datetime.tm_year, (long)datetime.tm_hour,
                (long)datetime.tm_min, (long)datetime.tm_sec);
    } else {
        sprintf(gethead,
                "Authorization: OSS %s:%s" CRLF "Date: %s, %02ld %s %04ld %02ld:%02ld:%02ld GMT" CRLF
                "Range: bytes=%s" CRLF CRLF,
                key, sign, gmtweek, (long)datetime.tm_mday, gmtmon, (long)datetime.tm_year, (long)datetime.tm_hour,
                (long)datetime.tm_min, (long)datetime.tm_sec, range);  // mod by wy 210428(%2d-->%2ld)
    }
    LOGD(TAG, "gethead:%s\r\n", gethead);

    client.header = gethead;
    client_data.response_buf = resbuf;
    client_data.response_buf_len = reslen;
    client_data.header_buf = resbuf;
    client_data.header_buf_len = reslen;

    /* 获取文件并GET请求连接HTTP服务器 */
    ret = httpclient_get_file(&client, url, LocalPath, &client_data);

    return ret;
}

int32_t oss_http_put_content(const char *key, const char *secret, const char *endPoint, const char *BucketName,
                             const char *ObjectName, const char *ContentData, char *resbuf, uint32_t reslen)

{
    httpclient_t client = { 0 };
    httpclient_data_t client_data = { 0 };
    char url[256] = { 0 };
    char puthead[256] = { 0 };
    char signmsg[128] = { 0 };
    int32_t ret = 0;
    char signbin[64] = { 0 };
    char sign[64] = { 0 };
    uint32_t outputLength = 0;
    long LocalfileSize = 0;

    /* 获取时间 */
    struct timeval tv = { 0 };
    timeutils_struct_time_t datetime = { 0 };
    gettimeofday(&tv, NULL);
    timeutils_seconds_since_epoch_to_struct_time(tv.tv_sec, &datetime);

    const char *gmtweek = getWeekdayByYearday(datetime.tm_year, datetime.tm_mon, datetime.tm_mday);
    const char *gmtmon = getMothStr(datetime.tm_mon);

    /* 签名认证组包 */
    sprintf(signmsg, "PUT\n\ntext/plain\n%s, %02ld %s %04ld %02ld:%02ld:%02ld GMT\n/%s/%s", gmtweek,
            (long)datetime.tm_mday, gmtmon, (long)datetime.tm_year, (long)datetime.tm_hour, (long)datetime.tm_min,
            (long)datetime.tm_sec, BucketName, ObjectName);
    LOGD(TAG, "signmsg:%s\r\n", signmsg);

    /* 签名认证 */
    aliot_hmac_sha1(signmsg, strlen(signmsg), signbin, secret, strlen(secret));

    /* 签名认证之后的base64数据格式转换 */
    aliot_base64encode((const uint8_t *)signbin, strlen(signbin), sizeof(sign), (uint8_t *)sign,
                       &outputLength);  // mod by wy 210428(sign类型不匹配)
    LOGD(TAG, "sign:%s\r\n", sign);

    /* 组合url */
    sprintf(url, "http://%s.%s/%s", BucketName, endPoint, ObjectName);
    LOGD(TAG, "url:%s\r\n", url);

    LocalfileSize = strlen(ContentData) + 2;

    /* 请求头组包 */
    sprintf(puthead,
            "Authorization: OSS %s:%s" CRLF "Date: %s, %02ld %s %04ld %02ld:%02ld:%02ld GMT" CRLF
            "Content-Type: text/plain" CRLF "Content-Length: %ld" CRLF CRLF,
            key, sign, gmtweek, (long)datetime.tm_mday, gmtmon, (long)datetime.tm_year, (long)datetime.tm_hour,
            (long)datetime.tm_min, (long)datetime.tm_sec, LocalfileSize);  // mod by wy 210428(%2d-->%2ld)
    LOGD(TAG, "puthead:%s\r\n", puthead);

    client.header = puthead;
    client_data.response_buf = resbuf;
    client_data.response_buf_len = reslen;
    client_data.header_buf = resbuf;
    client_data.header_buf_len = reslen;

    /* 获取文件并PUT请求连接HTTP服务器 */
    ret = (int32_t)httpclient_put_content(&client, url, ContentData, &client_data);
    return ret;
}
