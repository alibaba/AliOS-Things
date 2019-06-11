/*******************************************************************************
 *
 * Copyright (c) 2015 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Christian Renz - Please refer to git log
 *
 *******************************************************************************/


#include "liblwm2m.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <inttypes.h>

#include "commandline.h"
#include "connection.h"
#include "bootstrap_info.h"

#define CMD_STATUS_NEW  0
#define CMD_STATUS_SENT 1
#define CMD_STATUS_OK   2
#define CMD_STATUS_FAIL 3

typedef struct _endpoint_
{
    struct _endpoint_ * next;
    char *          name;
    void *          handle;
    bs_command_t *  cmdList;
    uint8_t         status;
} endpoint_t;

typedef struct
{
    int               sock;
    connection_t *    connList;
    lwm2m_context_t * lwm2mH;
    bs_info_t *       bsInfo;
    endpoint_t *      endpointList;
    int               addressFamily;
} internal_data_t;

/*
 * ensure sync with: er_coap_13.h COAP_MAX_PACKET_SIZE!
 * or internals.h LWM2M_MAX_PACKET_SIZE!
 */
#define MAX_PACKET_SIZE 198

static int g_quit = 0;

static void prv_quit(char * buffer,
                     void * user_data)
{
    g_quit = 1;
}

void handle_sigint(int signum)
{
    prv_quit(NULL, NULL);
}

void print_usage(char * filename,
                 char * port)
{
    fprintf(stdout, "Usage: bootstap_server [OPTION]\r\n");
    fprintf(stderr, "Launch a LWM2M Bootstrap Server.\r\n\n");
    fprintf(stdout, "Options:\r\n");
    fprintf(stdout, "  -f FILE\tSpecify BootStrap Information file. Default: ./%s\r\n", filename);
    fprintf(stdout, "  -l PORT\tSet the local UDP port of the Client. Default: %s\r\n", port);
    fprintf(stdout, "  -4\t\tUse IPv4 connection. Default: IPv6 connection\r\n");
    fprintf(stdout, "\r\n");
}

static void prv_print_uri(FILE * fd,
                          lwm2m_uri_t * uriP)
{
    fprintf(fd, "/");
    if (uriP != NULL)
    {
        fprintf(fd, "%hu", uriP->objectId);
        if (LWM2M_URI_IS_SET_INSTANCE(uriP))
        {
            fprintf(fd, "/%d", uriP->instanceId);
            if (LWM2M_URI_IS_SET_RESOURCE(uriP))
                fprintf(fd, "/%d", uriP->resourceId);
        }
    }
}
static void prv_endpoint_free(endpoint_t * endP)
{
    if (endP != NULL)
    {
        if (endP->name != NULL) free(endP->name);
        free(endP);
    }
}

static endpoint_t * prv_endpoint_find(internal_data_t * dataP,
                                      void * sessionH)
{
    endpoint_t * endP;

    endP = dataP->endpointList;

    while (endP != NULL
        && endP->handle != sessionH)
    {
        endP = endP->next;
    }

    return endP;
}

static endpoint_t * prv_endpoint_new(internal_data_t * dataP,
                                     void * sessionH)
{
    endpoint_t * endP;

    endP = prv_endpoint_find(dataP, sessionH);
    if (endP != NULL)
    {
        // delete previous state for the endpoint
        endpoint_t * parentP;

        parentP = dataP->endpointList;
        while (parentP != NULL
            && parentP->next != endP)
        {
            parentP = parentP->next;
        }
        if (parentP != NULL)
        {
            parentP->next = endP->next;
        }
        else
        {
            dataP->endpointList = endP->next;
        }
        prv_endpoint_free(endP);
    }

    endP = (endpoint_t *)malloc(sizeof(endpoint_t));
    return endP;
}

static void prv_endpoint_clean(internal_data_t * dataP)
{
    endpoint_t * endP;
    endpoint_t * parentP;

    while (dataP->endpointList != NULL
        && (dataP->endpointList->cmdList == NULL
         || dataP->endpointList->status == CMD_STATUS_FAIL))
    {
        endP = dataP->endpointList->next;
        prv_endpoint_free(dataP->endpointList);
        dataP->endpointList = endP;
    }

    parentP = dataP->endpointList;
    if (parentP != NULL)
    {
        endP = dataP->endpointList->next;
        while(endP != NULL)
        {
            endpoint_t * nextP;

            nextP = endP->next;
            if (endP->cmdList == NULL
            || endP->status == CMD_STATUS_FAIL)
            {
                prv_endpoint_free(endP);
                parentP->next = nextP;
            }
            else
            {
                parentP = endP;
            }
            endP = nextP;
        }
    }
}

static void prv_send_command(internal_data_t * dataP,
                             endpoint_t * endP)
{
    int res;

    if (endP->cmdList == NULL) return;

    switch (endP->cmdList->operation)
    {
    case BS_DELETE:
        fprintf(stdout, "Sending DELETE ");
        prv_print_uri(stdout, endP->cmdList->uri);
        fprintf(stdout, " to \"%s\"", endP->name);
        res = lwm2m_bootstrap_delete(dataP->lwm2mH, endP->handle, endP->cmdList->uri);
        break;

    case BS_WRITE_SECURITY:
    {
        lwm2m_uri_t uri;
        bs_server_tlv_t * serverP;

        serverP = (bs_server_tlv_t *)LWM2M_LIST_FIND(dataP->bsInfo->serverList, endP->cmdList->serverId);
        if (serverP == NULL
         || serverP->securityData == NULL)
        {
            endP->status = CMD_STATUS_FAIL;
            return;
        }

        uri.flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID;
        uri.objectId = LWM2M_SECURITY_OBJECT_ID;
        uri.instanceId = endP->cmdList->serverId;

        fprintf(stdout, "Sending WRITE ");
        prv_print_uri(stdout, &uri);
        fprintf(stdout, " to \"%s\"", endP->name);

        res = lwm2m_bootstrap_write(dataP->lwm2mH, endP->handle, &uri, LWM2M_CONTENT_TLV, serverP->securityData, serverP->securityLen);
    }
        break;

    case BS_WRITE_SERVER:
    {
        lwm2m_uri_t uri;
        bs_server_tlv_t * serverP;

        serverP = (bs_server_tlv_t *)LWM2M_LIST_FIND(dataP->bsInfo->serverList, endP->cmdList->serverId);
        if (serverP == NULL
         || serverP->serverData == NULL)
        {
            endP->status = CMD_STATUS_FAIL;
            return;
        }

        uri.flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID;
        uri.objectId = LWM2M_SERVER_OBJECT_ID;
        uri.instanceId = endP->cmdList->serverId;

        fprintf(stdout, "Sending WRITE ");
        prv_print_uri(stdout, &uri);
        fprintf(stdout, " to \"%s\"", endP->name);

        res = lwm2m_bootstrap_write(dataP->lwm2mH, endP->handle, &uri, LWM2M_CONTENT_TLV, serverP->serverData, serverP->serverLen);
    }
        break;

    case BS_FINISH:
        fprintf(stdout, "Sending BOOTSTRAP FINISH ");
        fprintf(stdout, " to \"%s\"", endP->name);

        res = lwm2m_bootstrap_finish(dataP->lwm2mH, endP->handle);
        break;

    default:
        return;
    }

    if (res == COAP_NO_ERROR)
    {
        fprintf(stdout, " OK.\r\n");

        endP->status = CMD_STATUS_SENT;
    }
    else
    {
        fprintf(stdout, " failed!\r\n");

        endP->status = CMD_STATUS_FAIL;
    }
}

static int prv_bootstrap_callback(void * sessionH,
                                  uint8_t status,
                                  lwm2m_uri_t * uriP,
                                  char * name,
                                  void * userData)
{
    internal_data_t * dataP = (internal_data_t *)userData;
    uint8_t result;
    endpoint_t * endP;

    switch (status)
    {
    case COAP_NO_ERROR:
    {
        bs_endpoint_info_t * endInfoP;

        // Display
        fprintf(stdout, "\r\nBootstrap request from \"%s\"\r\n", name);

        // find Bootstrap Info for this endpoint
        endInfoP = dataP->bsInfo->endpointList;
        while (endInfoP != NULL
            && endInfoP->name != NULL
            && strcmp(name, endInfoP->name) != 0)
        {
            endInfoP = endInfoP->next;
        }
        if (endInfoP == NULL)
        {
            // find default Bootstrap Info
            endInfoP = dataP->bsInfo->endpointList;
            while (endInfoP != NULL
                && endInfoP->name != NULL)
            {
                endInfoP = endInfoP->next;
            }
        }
        // Nothing found, discard the request
        if (endInfoP == NULL)return COAP_IGNORE;

        endP = prv_endpoint_new(dataP, sessionH);
        if (endP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;

        endP->cmdList = endInfoP->commandList;
        endP->handle = sessionH;
        endP->name = strdup(name);
        endP->status = CMD_STATUS_NEW;
        endP->next = dataP->endpointList;
        dataP->endpointList = endP;

        return COAP_204_CHANGED;
    }

    default:
        // Display
        fprintf(stdout, "\r\n Received status ");
        print_status(stdout, status);
        fprintf(stdout, " for URI ");
        prv_print_uri(stdout, uriP);

        endP = prv_endpoint_find(dataP, sessionH);
        if (endP == NULL)
        {
            fprintf(stdout, " from unknown endpoint.\r\n");
            return COAP_NO_ERROR;
        }
        fprintf(stdout, " from endpoint %s.\r\n", endP->name);

        // should not happen
        if (endP->cmdList == NULL) return COAP_NO_ERROR;
        if (endP->status != CMD_STATUS_SENT) return COAP_NO_ERROR;

        switch (endP->cmdList->operation)
        {
        case BS_DELETE:
            if (status == COAP_202_DELETED)
            {
                endP->status = CMD_STATUS_OK;
            }
            else
            {
                endP->status = CMD_STATUS_FAIL;
            }
            break;

        case BS_WRITE_SECURITY:
        case BS_WRITE_SERVER:
            if (status == COAP_204_CHANGED)
            {
                endP->status = CMD_STATUS_OK;
            }
            else
            {
                endP->status = CMD_STATUS_FAIL;
            }
            break;

        default:
            endP->status = CMD_STATUS_FAIL;
            break;
        }
        break;
    }

    return COAP_NO_ERROR;
}

static void prv_bootstrap_client(char * buffer,
                                 void * user_data)
{
    internal_data_t * dataP = (internal_data_t *)user_data;
    char * uri;
    char * name;
    char* end = NULL;
    char * host;
    char * port;
    connection_t * newConnP = NULL;

    uri = buffer;
    end = get_end_of_arg(buffer);
    if (end[0] != 0)
    {
        *end = 0;
        buffer = end + 1;
        name = get_next_arg(buffer, &end);
    }
    if (!check_end_of_args(end)) goto syntax_error;

    // parse uri in the form "coaps://[host]:[port]"
    if (0==strncmp(uri, "coaps://", strlen("coaps://"))) {
        host = uri+strlen("coaps://");
    }
    else if (0==strncmp(uri, "coap://",  strlen("coap://"))) {
        host = uri+strlen("coap://");
    }
    else {
        goto syntax_error;
    }
    port = strrchr(host, ':');
    if (port == NULL) goto syntax_error;
    // remove brackets
    if (host[0] == '[')
    {
        host++;
        if (*(port - 1) == ']')
        {
            *(port - 1) = 0;
        }
        else goto syntax_error;
    }
    // split strings
    *port = 0;
    port++;

    fprintf(stderr, "Trying to connect to LWM2M CLient at %s:%s\r\n", host, port);
    newConnP = connection_create(dataP->connList, dataP->sock, host, port, dataP->addressFamily);
    if (newConnP == NULL) {
        fprintf(stderr, "Connection creation failed.\r\n");
        return;
    }
    dataP->connList = newConnP;

    // simulate a client bootstrap request.
    if (COAP_204_CHANGED == prv_bootstrap_callback(newConnP, COAP_NO_ERROR, NULL, name, user_data))
    {
        fprintf(stdout, "OK");
    }
    else
    {
        fprintf(stdout, "Error");
    }
    return;

syntax_error:
    fprintf(stdout, "Syntax error !");
}


int main(int argc, char *argv[])
{
    fd_set readfds;
    struct timeval tv;
    int result;
    char * port = "5685";
    internal_data_t data;
    char * filename = "bootstrap_server.ini";
    int opt;
    FILE * fd;
    command_desc_t commands[] =
    {
        {"boot", "Bootstrap a client (Server Initiated).", " boot URI [NAME]\r\n"
                                    "   URI: uri of the client to bootstrap\r\n"
                                    "   NAME: endpoint name of the client as in the .ini file (optionnal)\r\n"
                                    "Example: boot coap://[::1]:56830 testlwm2mclient", prv_bootstrap_client, &data},
        {"q", "Quit the server.", NULL, prv_quit, NULL},

        COMMAND_END_LIST
    };

    memset(&data, 0, sizeof(internal_data_t));

    data.addressFamily = AF_INET6;

    opt = 1;
    while (opt < argc)
    {
        if (argv[opt] == NULL
         || argv[opt][0] != '-'
         || argv[opt][2] != 0)
        {
            print_usage(filename, port);
            return 0;
        }
        switch (argv[opt][1])
        {
        case 'f':
            opt++;
            if (opt >= argc)
            {
                print_usage(filename, port);
                return 0;
            }
            filename = argv[opt];
            break;
        case 'l':
            opt++;
            if (opt >= argc)
            {
                print_usage(filename, port);
                return 0;
            }
            port = argv[opt];
            break;
        case '4':
            data.addressFamily = AF_INET;
            break;
        default:
            print_usage(filename, port);
            return 0;
        }
        opt += 1;
    }

    data.sock = create_socket(port, data.addressFamily);
    if (data.sock < 0)
    {
        fprintf(stderr, "Error opening socket: %d\r\n", errno);
        return -1;
    }

    data.lwm2mH = lwm2m_init(NULL);
    if (NULL == data.lwm2mH)
    {
        fprintf(stderr, "lwm2m_init() failed\r\n");
        return -1;
    }

    signal(SIGINT, handle_sigint);

    fd = fopen(filename, "r");
    if (fd == NULL)
    {
        fprintf(stderr, "Opening file %s failed.\r\n", filename);
        return -1;
    }

    data.bsInfo = bs_get_info(fd);
    fclose(fd);
    if (data.bsInfo == NULL)
    {
        fprintf(stderr, "Reading Bootstrap Info from file %s failed.\r\n", filename);
        return -1;
    }

    lwm2m_set_bootstrap_callback(data.lwm2mH, prv_bootstrap_callback, (void *)&data);

    fprintf(stdout, "LWM2M Bootstrap Server now listening on port %s.\r\n\n", port);
    fprintf(stdout, "> "); fflush(stdout);

    while (0 == g_quit)
    {
        endpoint_t * endP;

        FD_ZERO(&readfds);
        FD_SET(data.sock, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        tv.tv_sec = 60;
        tv.tv_usec = 0;

        result = lwm2m_step(data.lwm2mH, &(tv.tv_sec));
        if (result != 0)
        {
            fprintf(stderr, "lwm2m_step() failed: 0x%X\r\n", result);
            return -1;
        }

        result = select(FD_SETSIZE, &readfds, 0, 0, &tv);

        if ( result < 0 )
        {
            if (errno != EINTR)
            {
              fprintf(stderr, "Error in select(): %d\r\n", errno);
            }
        }
        else if (result >= 0)
        {
            uint8_t buffer[MAX_PACKET_SIZE];
            int numBytes;

            // Packet received
            if (FD_ISSET(data.sock, &readfds))
            {
                struct sockaddr_storage addr;
                socklen_t addrLen;

                addrLen = sizeof(addr);
                numBytes = recvfrom(data.sock, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);

                if (numBytes == -1)
                {
                    fprintf(stderr, "Error in recvfrom(): %d\r\n", errno);
                }
                else
                {
                    char s[INET6_ADDRSTRLEN];
                    in_port_t port;
                    connection_t * connP;

					s[0] = 0;
                    if (AF_INET == addr.ss_family)
                    {
                        struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
                        inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET6_ADDRSTRLEN);
                        port = saddr->sin_port;
                    }
                    else if (AF_INET6 == addr.ss_family)
                    {
                        struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&addr;
                        inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
                        port = saddr->sin6_port;
                    }

                    fprintf(stderr, "%d bytes received from [%s]:%hu\r\n", numBytes, s, ntohs(port));

                    output_buffer(stderr, buffer, numBytes, 0);

                    connP = connection_find(data.connList, &addr, addrLen);
                    if (connP == NULL)
                    {
                        connP = connection_new_incoming(data.connList, data.sock, (struct sockaddr *)&addr, addrLen);
                        if (connP != NULL)
                        {
                            data.connList = connP;
                        }
                    }
                    if (connP != NULL)
                    {
                        lwm2m_handle_packet(data.lwm2mH, buffer, numBytes, connP);
                    }
                }
            }
            // command line input
            else if (FD_ISSET(STDIN_FILENO, &readfds))
            {
                numBytes = read(STDIN_FILENO, buffer, MAX_PACKET_SIZE - 1);

                if (numBytes > 1)
                {
                    buffer[numBytes] = 0;
                    handle_command(commands, (char*)buffer);
                }
                if (g_quit == 0)
                {
                    fprintf(stdout, "\r\n> ");
                    fflush(stdout);
                }
                else
                {
                    fprintf(stdout, "\r\n");
                }
            }
            // Do operations on endpoints
            prv_endpoint_clean(&data);

            endP = data.endpointList;
            while (endP != NULL)
            {
                switch(endP->status)
                {
                case CMD_STATUS_OK:
                    endP->cmdList = endP->cmdList->next;
                    endP->status = CMD_STATUS_NEW;
                    // fall through
                case CMD_STATUS_NEW:
                    prv_send_command(&data, endP);
                    break;
                default:
                    break;
                }

                endP = endP->next;
            }
        }
    }

    lwm2m_deinit(data.lwm2mH);
    bs_free_info(data.bsInfo);
    while (data.endpointList != NULL)
    {
        endpoint_t * endP;

        endP = data.endpointList;
        data.endpointList = data.endpointList->next;

        prv_endpoint_free(endP);
    }
    close(data.sock);
    connection_free(data.connList);

    return 0;
}
