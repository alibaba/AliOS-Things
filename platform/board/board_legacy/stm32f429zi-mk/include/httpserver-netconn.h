#ifndef __HTTPSERVER_NETCONN_H__
#define __HTTPSERVER_NETCONN_H__

#include "lwip/api.h"

/* HTTP Requests */
#define HTTP_GET_REQUEST            "GET /"
#define HTTP_POST_REQUEST           "POST /"

/*HTTP Responses*/
#define HTTP_HEADER_OK_PART1        "HTTP/1.1 200 OK\r\nCache-Control: no-cache\r\nContent-type: "
#define HTTP_HEADER_OK_PART2        "\r\nContent-length: "
#define HTTP_DYMAMIC_HEADER         "\r\nTransfer-Encoding: chunked\r\n"
#define HTTP_STATIC_HEADER          "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n"
#define HTTP_DYNAMIC_HEADER         "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nTransfer-Encoding: chunked\r\n"
#define HTTP_WRONG_HEADER           "HTTP/1.1 500 OK\r\nContent-type: text/html\r\nContent-length: 82\r\n\r\n"
//#define HTTP_WRONG_DATA             "\r\n57\r\n<HTML><BODY><H1>Error</H1><BR>Page doens't exist\r\n or out of memory.</BODY></HTML>\r\n0\r\n\r\n"
#define HTTP_WRONG_DATA             "<HTML><BODY><H1>Error</H1><BR>Page doens't exist\r\n or out of memory.</BODY></HTML>"
#define HTTP_TASK_STATUS_START      "<html><head><meta http-equiv=\"refresh\" content=\"2\"></head><BODY \"bgcolor=\"silver\">Running tasks:"\
                                    "<p><pre>Task          State  Priority  Stack    #<br>*****************************************<br>"
#define HTTP_TASK_STATUS_END        "\r\AliOS Things (c)2018 by Alibaba</pre></BODY></html>"

void http_server_netconn_init(void);
void DynWebPage(struct netconn *conn);

#endif /* __HTTPSERVER_NETCONN_H__ */
