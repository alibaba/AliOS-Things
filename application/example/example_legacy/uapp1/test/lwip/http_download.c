
#include "time.h"
#include "lwip/sockets.h"
#include "lwip/debug.h"
#include <stdio.h>
#include "lwip/netdb.h"
#include <string.h>
#include "lwip/api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "lwip/dns.h"
#include "string.h"


#define FILE_URL "http://download.skycn.com/hao123-soft-online-bcs/soft/2017_02_22_BaiduPlayer5Setup_5.exe"
#define MTU_SIZE 3000
#define MAX_LEN 1024*1024*1024

#define HEAD_INFO       "http://"

#define HEAD_INFO_S     "https://"

#define SIZE_TYPE 64
#define SIZE_FILE 64

struct resp_header
{
    int  status_code;
    char content_type[SIZE_TYPE];
    long content_length;
    char file_name[SIZE_FILE];
};

#define BUFSIZE 300
static char data_buf[BUFSIZE];
static char read_buf[MTU_SIZE] = {0};

void parse_url(const char *url, char *domain, int *port, char *file_name)
{

    int j = 0;
    int start = 0;
    int flag = 0;
    int i;

    char *patterns[] = {HEAD_INFO, HEAD_INFO_S, NULL};

    *port = 80;

    for ( i = 0; patterns[i]; i++) {
        if (strncmp(url, patterns[i], strlen(patterns[i])) == 0) {
            start = strlen(patterns[i]);
        }
    }

    for ( i = start; url[i] != '/' && url[i] != '\0'; i++, j++) {
        domain[j] = url[i];
    }

    domain[j] = '\0';


    char *pos = strstr(domain, ":");
    if (pos) {
        sscanf(pos, ":%d", port);
    }

    for ( i = 0; i < (int)strlen(domain); i++) {
        if (domain[i] == ':') {
            domain[i] = '\0';
            break;
        }
    }

    j = 0;
    for ( i = start; url[i] != '\0'; i++) {
          if (url[i] == '/' && flag == 0) {
              j    = 0;
              flag = 1;
              continue;
          }

          if(flag == 1) {
              file_name[j++] = url[i];
          }
    }

    file_name[j] = '\0';
}

struct resp_header get_resp_header(const char *response)
{

    struct resp_header resp;

    char *pos = strstr(response, "HTTP/");
    if (pos) {
        sscanf(pos, "%*s %d", &resp.status_code);
    }

    pos = strstr(response, "Content-Type:");
    if (pos) {
        sscanf(pos, "%*s %s", resp.content_type);
    }

    pos = strstr(response, "Content-Length:");
    if (pos) {
        sscanf(pos, "%*s %ld", &resp.content_length);
    }

    return resp;
}

u32_t get_time_in_secs()
{
    return (u32_t)(aos_now_ms()/1000); // get time in ms, change it in second.
}


int get_file_with_url( char *path)
{
    int      cfd;
    struct   sockaddr_in cadd;
    struct   hostent *pURL;
    struct   resp_header resp = {0};
    char     host[36] = {0};
    int      port = 0;
    int      len = 0;
    int      total_rcv_len = 0;
    int      curr_rcv_len =0;
    uint32_t cur_sec, last_update_sec;
    int      ret;

    if (-1 == (cfd = socket(AF_INET, SOCK_STREAM, 0))) {
        printf("%16.16s %4d socket error\r\n", __func__, __LINE__);
        return -1;
    }

    parse_url(path, host, &port, resp.file_name);
    printf("host: %s, port: %d\r\n", host, port);
    printf("file: %s\r\n", resp.file_name);

    pURL = lwip_gethostbyname(host);
    memset(&cadd, 0,sizeof(struct sockaddr_in));
    cadd.sin_family      = AF_INET;
    cadd.sin_addr.s_addr = *((unsigned long*) pURL->h_addr_list[0]);
    cadd.sin_port        = htons(port);

    memset(data_buf, 0, BUFSIZE);
    strcat(data_buf, "GET /");
    strcat(data_buf, resp.file_name);
    strcat(data_buf, " HTTP/1.1\r\n");
    strcat(data_buf, "HOST: ");
    strcat(data_buf, host);
    strcat(data_buf, "\r\n");
    strcat(data_buf, "Cache-Control: no-cache\r\n\r\n");

    if (-1 == (ret = connect(cfd, (struct sockaddr*) &cadd,
        (socklen_t) sizeof(cadd)))) {
        close(cfd);
        printf("\tconnect failed \r\n");

        return -2;
    }

    if (-1 == (ret = send(cfd, data_buf, strlen(data_buf), 0))) {
        close(cfd);
        return -3;
    }

    cur_sec = get_time_in_secs();
    last_update_sec = cur_sec;

    curr_rcv_len = 0;
    memset(data_buf,0,sizeof(data_buf));
    int recving = 0;

    while (1) {
        memset(read_buf, 0, sizeof(read_buf));
        len = recv(cfd, read_buf, MTU_SIZE,0);

        total_rcv_len += len;
        curr_rcv_len  += len;

        if (len > 0) {
            cur_sec = get_time_in_secs();
            if (cur_sec - last_update_sec > 1) {
                printf("\treceived bytes: %-9d\r\n", total_rcv_len);
                last_update_sec = cur_sec;
            }
        } else {
            recving = 0;
            printf("\t\nReach the end of file!\r\n");
            ret = 0;
            break;
        }

        if(get_time_in_secs() - cur_sec > 5) {
            printf("\t Download time out for about 20 seconds.\r\n");
            printf("\t Current time: %d\r\n", get_time_in_secs());
            printf("\t Latest time: %d\r\n", cur_sec);
            ret = -4;
            break;;
        }
    }

    close(cfd);

    return ret;
}

int tcp_download_file(void)
{
    int ret;

    return get_file_with_url(FILE_URL);
}

