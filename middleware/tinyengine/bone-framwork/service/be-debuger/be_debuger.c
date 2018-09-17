//
//  be_debuger.c
//  app
//
//  Created by howie on 2018/4/19.
//  Copyright © 2018 alibaba. All rights reserved.
//

#include <string.h>
#include <unistd.h>
#include "be_debuger.h"
#ifdef BE_OS_AOS
#if defined(WITH_LWIP)
#include <lwip/sockets.h>
#include <lwip/pbuf.h>
#include <lwip/udp.h>
#include <lwip/tcp.h>
#else
#include <aos/network.h>
#endif
#endif
#include <hal/hal.h>
#include <be_osal.h>


#define BE_DEBUGER_UPLOAD_CMD "#be-debuger-upload#"
#define BE_DEBUGER_UPLOAD_CMD_REPLY "#be-debuger-upload-reply# %d \n"
#define MODULE_TAG ("be_debuger")
#define CLI_REPLY   printf

static be_debug_http_t http;
static struct list_head              clients = LIST_HEAD_INIT(clients);
static on_debuger_recv_begin         recv_begin_cb    = NULL;
static on_debuger_recv_update        recv_update_cb   = NULL;
static on_debuger_recv_complete      recv_complete_cb = NULL;


static void task_uart_recv(int content_length)
{
    char buf[BE_DEBUGER_UAER_RECV_BUFFER] = {0};
    int recv_bytes = 0;
    int nread = 0;

    CLI_REPLY(BE_DEBUGER_UPLOAD_CMD_REPLY, nread);

    if (recv_begin_cb != NULL) recv_begin_cb(content_length);

    while (1) {
        uart_dev_t uart_stdio;
        memset(&uart_stdio, 0, sizeof(uart_stdio));
        uart_stdio.port = 0;

        int status = hal_uart_recv_II(&uart_stdio, buf, sizeof(buf), &recv_bytes, 0xFFFFFFFF);
        if (status == 0) {
            int last_nread = nread;
            nread += recv_bytes;

            if (nread >= content_length) {
                if (recv_complete_cb != NULL)
                    recv_complete_cb(buf, content_length - last_nread);
                CLI_REPLY(BE_DEBUGER_UPLOAD_CMD_REPLY, content_length);
                break;
            } else {
                if (recv_update_cb != NULL) recv_update_cb(buf, recv_bytes);
                CLI_REPLY(BE_DEBUGER_UPLOAD_CMD_REPLY, nread);
            }
        }
    }
    be_osal_exit_task(0);
}

static void handle_cli_cmd(char* pwbuf, int blen, int argc, char** argv)
{
    if (argc == 2) {
        be_debug(MODULE_TAG, "aos_cli_stop ok, argc=%d\r\n", argc);
        // aos_cli_stop();
        be_osal_new_task("be-debuger-uart", task_uart_recv, (char*)argv[1], 8192, NULL);
    } else {
        be_debug(MODULE_TAG, "aos_cli_stop failure, argc=%d\r\n", argc);
    }
}

static struct cli_command be_uploadcmd = {
    .name = BE_DEBUGER_UPLOAD_CMD,
    .help = "be-debuger-upload 579123(size)",
    .function = handle_cli_cmd};

#if defined(WITH_LWIP)
static ip_addr_t ipgroup_rev;
static struct udp_pcb* udp_pcb;

static void on_udp_recv(void* arg, struct udp_pcb* upcb, struct pbuf* p, ip_addr_t* addr, u16_t port)
{
    char buffer[128] = {0};
    if (p->len > sizeof(buffer)) goto end;

    memcpy(buffer, p->payload, p->len);
    be_debug(MODULE_TAG, "%s\n", buffer);
    if (strstr(buffer, BE_DEBUGER_SSDP_ST) != NULL) {
        const char* reply = "HTTP/1.1 200 OK\r\n\r\n";
        size_t len = strlen(reply);
        struct pbuf* pp = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
        memcpy(pp->payload, reply, len);
        udp_sendto(udp_pcb, pp, addr, port);
        pbuf_free(pp);
    }
end:
    pbuf_free(p);
}
#else
#endif

static void http_close_client(be_debug_http_client_t *client)
{
    // sock close
    close(client->sock);
    
    // free headers
    be_debug_http_header_t *entry;
    be_debug_http_header_t *tmp2;
    struct list_head *headers = &client->req.headers;
    if (headers->next != NULL && headers->prev != NULL) {
        list_for_each_entry_safe(entry, tmp2, headers, list) {
            list_del_init(&entry->list);
            free(entry);
        }
    }
    
    headers = &client->resp.headers;
    if (headers->next != NULL && headers->prev != NULL) {
        list_for_each_entry_safe(entry, tmp2, headers, list) {
            list_del_init(&entry->list);
            free(entry);
        }
    }
    
    // free client
    list_del_init(&client->list);
    aos_cancel_poll_read_fd(client->sock, NULL, NULL);
    free(client);
}

static const char* http_headers_value(struct list_head* headers, const char* field)
{
    be_debug_http_header_t* header = NULL;
    list_for_each_entry(header, headers, list) {
        if (strcasecmp(header->field, field) == 0) {
            return header->value;
        }
    }
    return "";
}

static int on_http_req_url(http_parser* parser, const char* at, size_t length)
{
    be_debug_http_req_t* req = (be_debug_http_req_t*)parser->data;
    req->method = (char*)http_method_str(parser->method);
    strncpy(req->url, at, length > sizeof(req->url) ? sizeof(req->url) : length);
    return 0;
}

static int on_http_header_field(http_parser* parser, const char* at, size_t length)
{
    be_debug_http_header_t* header = (be_debug_http_header_t*)calloc(1, sizeof(be_debug_http_header_t));
    strncpy(header->field, at, length);
    list_add_tail(&header->list, &((be_debug_http_req_t*)parser->data)->headers);
    return 0;
}

static int on_http_header_value(http_parser* parser, const char* at, size_t length)
{
    be_debug_http_header_t* header = NULL;
    list_for_each_entry_reverse(header, &((be_debug_http_req_t*)parser->data)->headers, list) {
        size_t value_len = sizeof(header->value);
        strncpy(header->value, at, length >= value_len ? value_len - 1 : length);
        break;
    }
    
    return 0;
}

static http_parser_settings settings = {
    .on_url = on_http_req_url,
    .on_header_field = on_http_header_field,
    .on_header_value = on_http_header_value,
};

static void send_response(char *body, be_debug_http_req_t *req)
{
    be_debug_http_client_t* client = (be_debug_http_client_t*)container_of(req, be_debug_http_client_t, req);
    const char* format =
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %d\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "\r\n"
        "%s\r\n";
    unsigned char response[512] = {0};
    sprintf((char*)response, format, strlen(body), body);
    send(client->sock, response, strlen((char*)response), 0);
    http_close_client(client);
}

static void http_respone(be_debug_http_req_t* req)
{
    char* body = NULL;
    char* body_ok = "ok\n";
    char name[12] = {0};

    if (strstr(req->url, "/reboot") != NULL) {
        body = body_ok;
        // before reboot send response first
        send_response(body, req);
        be_osal_reboot();
    } else {
        body = body_ok;
    }
    send_response(body, req);
}

static void on_http_recv(int sock, void* arg)
{
    be_debug_http_client_t* client = NULL;
    list_for_each_entry(client, &clients, list) {
        if (client->sock == sock) {
            unsigned char buf[BE_DEBUGER_HTTP_RECV_BUFFER] = {0};
            ssize_t recv_bytes = 0;

            do {
                recv_bytes = recv(client->sock, buf, sizeof(buf), 0);
            } while (recv_bytes < 0 && (errno == EINTR));

            if (recv_bytes == 0 ||
                (recv_bytes < 0 && errno != EINPROGRESS && errno != EAGAIN)) {
                http_close_client(client);
            } else if (recv_bytes > 0) {
                be_debug(MODULE_TAG, "be-debuger http recv: %d bytes\n", (int)recv_bytes);
                http_parser* parser = &client->req.parser;
                be_debug_http_req_t* req = &client->req;
                http_parser_execute(parser, &settings, (char*)buf, strlen((char*)buf));

                if (parser->method == HTTP_POST) {
                    if (strstr(buf, "POST /reboot") != NULL) {
                        http_respone(req);
                    }
                    const char* content_type = http_headers_value(&req->headers, "content-type");
                    if (content_type == NULL) return;

                    if (strstr(content_type, "multipart/form-data") != NULL) {
                        char* boundary = strstr(content_type, "boundary=");

                        if (boundary == NULL) return;

                        char boundary_start[64] = {0};
                        char boundary_end[64] = {0};
                        
                        strcat(boundary_start, "--");
                        strcat(boundary_start, boundary + strlen("boundary="));
                        strcat(boundary_start, "\r\n");
                        
                        strcat(boundary_end, "\r\n--");
                        strcat(boundary_end, boundary + strlen("boundary="));
                        strcat(boundary_end, "--\r\n");
                        
                        char* start = strstr((char*)buf, boundary_start);
                        
                        if (start != NULL) {
                            char* body_header_start = start + strlen(boundary_start);
                            char* body_header_end = strstr(start, "\r\n\r\n");
                            size_t body_content_length = atoi(http_headers_value(&req->headers, "content-length"));
                            if (body_header_start != NULL &&
                                body_header_end != NULL &&
                                body_content_length > 0) {
                                size_t body_header_length = (body_header_end + strlen("\r\n\r\n")) - start;
                                size_t content_length = body_content_length - body_header_length - strlen(boundary_end);
                                if (recv_begin_cb != NULL) {
                                    req->content_length = content_length;
                                    recv_begin_cb(content_length);
                                }
                            } else {
                                be_debug(MODULE_TAG, "boundary invalid");
                            }
                        } else {
                            size_t nread = req->content_nread;
                            req->content_nread += recv_bytes;
                            
                            if (req->content_nread > req->content_length) {
                                if (recv_complete_cb)
                                    recv_complete_cb(buf, req->content_length - nread);
                                http_respone(req);
                            } else if (req->content_nread == req->content_length) {
                                if (recv_complete_cb)
                                    recv_complete_cb(buf, recv_bytes);
                                http_respone(req);
                            } else {
                                if (recv_update_cb)
                                    recv_update_cb(buf, recv_bytes);
                            }
                        }
                    } else if (strstr(content_type,"application/x-www-form-urlencoded") != NULL) {
                        http_respone(req);
                    }
                } else if (parser->method == HTTP_GET) {
                    http_respone(req);
                } else {
                }
            }
            break;
        }
    }
}

static void on_http_accept(int fd, void* arg) 
{
#ifdef WITH_LWIP
    socklen_t len;
    struct sockaddr_in addr;
    int sock = accept(fd, (struct sockaddr*)&addr, &len);
    be_debug_http_client_t* client =
        (be_debug_http_client_t*)calloc(1, sizeof(be_debug_http_client_t));
    client->sock = sock;
    memset(&client->req.parser, 0, sizeof(http_parser));
    client->req.parser.data = &client->req;
    http_parser_init(&client->req.parser, HTTP_REQUEST);
    INIT_LIST_HEAD(&client->req.headers);
    list_add_tail(&client->list, &clients);
    aos_poll_read_fd(sock, on_http_recv, NULL);
#else
    /*TODO: How to implement accept on aos SAL ?*/
#endif
}

int be_debuger_init(on_debuger_recv_begin on_begin,
                    on_debuger_recv_update on_update,
                    on_debuger_recv_complete on_complete)
{
    recv_begin_cb = on_begin;
    recv_update_cb = on_update;
    recv_complete_cb = on_complete;
    return 0;
}

int be_debuger_ssdp_start(void)
{
#if defined(WITH_LWIP)
    IP4_ADDR(&ipgroup_rev, 239, 255, 255, 250);
    igmp_joingroup(IP_ADDR_ANY, &ipgroup_rev);
    
    udp_pcb = udp_new();
    udp_bind(udp_pcb, IP_ADDR_ANY, BE_DEBUGER_SSDP_PORT);
    udp_recv(udp_pcb, (udp_recv_fn)on_udp_recv, NULL);
#else
#endif
    return 0;
}


#ifdef WITH_LWIP
int be_debuger_http_start(void)
{
    int succ = -1;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return succ;

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(int)) !=
        0) {
        succ = -2;
        goto end;
    }
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(BE_DEBUGER_HTTP_PORT);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    
    if (bind(sock,(struct sockaddr *)&addr, sizeof(addr)) < 0) {
        succ = -3;
        goto end;
    }
    
    if (listen(sock, 128) != 0) {
        succ = -4;
        goto end;
    }
    
    if (aos_poll_read_fd(sock, on_http_accept, NULL) != 0) {
        succ = -5;
        goto end;
    }
    
    http.sock = sock;
    return 0;
end:
    close(sock);
    return succ;
}

#else
//for qingwen fix it 
int be_debuger_http_start(void)
{
    return 0;
}

#endif

int be_debuger_uart_start(void) 
{
    aos_cli_register_command(&be_uploadcmd);

    return 0;
}
