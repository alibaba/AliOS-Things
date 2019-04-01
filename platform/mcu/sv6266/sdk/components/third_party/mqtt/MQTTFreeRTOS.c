/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *    Ian Craggs - convert to FreeRTOS
 *******************************************************************************/

#include "MQTTFreeRTOS.h"
#include "lwip/netdb.h"

static unsigned int mqtt_current_time_ms(void)
{
    unsigned int current_ms = 0;
    current_ms = os_tick2ms(xTaskGetTickCount());
    
    return current_ms;
}

void TimerCountdownMS(Timer* timer, unsigned int timeout_ms)
{
    timer->end_time = mqtt_current_time_ms() + timeout_ms;
}

void TimerCountdown(Timer* timer, unsigned int timeout) 
{
    timer->end_time = mqtt_current_time_ms() + (timeout * 1000);
}

int TimerLeftMS(Timer* timer) 
{
    unsigned int cur_time = 0;
    cur_time = mqtt_current_time_ms();
    if (timer->end_time < cur_time || timer->end_time == cur_time) {
        return 0;
    } else {
        return timer->end_time - cur_time;
    }
}

char TimerIsExpired(Timer* timer)
{
    unsigned int cur_time = 0;
    cur_time = mqtt_current_time_ms();
    //printf("cur_time: %d end_time: %d\n", cur_time, timer->end_time);
    if (timer->end_time < cur_time || timer->end_time == cur_time) {
        //printf("MQTT expired enter");
        return 1;
    } else {
        //printf("MQTT not expired");
        return 0;
    }
}

void TimerInit(Timer* timer)
{
    timer->end_time = 0;
}

int FreeRTOS_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
    int rc = 0, cnt = 0;
    int recvlen = 0;
    int ret = -1;
    fd_set fdset;
    struct timeval tv;
    
    FD_ZERO(&fdset);
    FD_SET(n->my_socket, &fdset);
    
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    ret = select(n->my_socket + 1, &fdset, NULL, NULL, &tv);
    //printf("mqtt read timer=%d ms\n", timeout_ms);
    if (ret < 0) {
        //printf("mqtt read(select) fail ret=%d\n", ret);
        return -1;
    } else if (ret == 0) {
        //printf("mqtt read(select) timeout\n");
        return -2;
    } else if (ret == 1) {
        do {
            //printf("mqtt read recv len = %d, recvlen = %d\n", len, recvlen);
            rc = recv(n->my_socket, buffer + recvlen, len - recvlen, 0);
            if (rc > 0) {
            recvlen += rc;
                //printf("mqtt read ret=%d, rc = %d, recvlen = %d\n", ret, rc, recvlen);
            } else {
                //printf("mqtt read fail: ret=%d, rc = %d, recvlen = %d\n", ret, rc, recvlen);
                return -3;
            }
        } while (recvlen < len);
    }
    
    return recvlen;
}

int FreeRTOS_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
    int rc = 0;
    int ret = -1;
    fd_set fdset;
    struct timeval tv;
    
    FD_ZERO(&fdset);
    FD_SET(n->my_socket, &fdset);
    
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    //printf("mqtt write timer=%d ms", timeout_ms);
    ret = select(n->my_socket + 1, NULL, &fdset, NULL, &tv);
    
    if (ret < 0) {
        printf("mqtt write fail");
        return -1;
    } else if (ret == 0) {
        printf("mqtt write timeout");
        return -2;
    } else if (ret == 1) {
        rc = write(n->my_socket, buffer, len);
    }
    return rc;
}

void FreeRTOS_disconnect(Network* n)
{
    close(n->my_socket);
    n->my_socket = -1;
}


void NetworkInit(Network* n)
{
    n->my_socket = -1;
    n->mqttread = FreeRTOS_read;
    n->mqttwrite = FreeRTOS_write;
    n->disconnect = FreeRTOS_disconnect;
}

int NetworkConnect(Network *n, char *addr,  char *port)
{
    int type = SOCK_STREAM;
    struct sockaddr_in address;
    int rc = -1;
    sa_family_t family = AF_INET;
    struct addrinfo *result = NULL;
    struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

    if ((rc = getaddrinfo(addr, NULL, &hints, &result)) == 0) {
        struct addrinfo *res = result;

        /* prefer ip4 addresses */
        while (res) {
            if (res->ai_family == AF_INET) {
                result = res;
                break;
            }
            res = res->ai_next;
        }

        if (result->ai_family == AF_INET) {
            address.sin_port = htons(atoi(port));
            address.sin_family = family = AF_INET;
            address.sin_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr;
        } else {
            rc = -1;
        }
        freeaddrinfo(result);
    }
    
    /* create client socket */
    if (rc == 0) {
        int opval = 1;
        n->my_socket = socket(family, type, 0);
        if (n->my_socket < 0) {
            printf("mqtt socket create fail");
            return -1;
        }
        /* connect remote servers*/
        rc = connect(n->my_socket, (struct sockaddr *)&address, sizeof(address));
        
        if (rc < 0) {
            close(n->my_socket);
            printf("mqtt socket connect fail:rc=%d,socket = %d", rc, n->my_socket);
            return -2;
        }
        
        setsockopt(n->my_socket ,IPPROTO_TCP, TCP_NODELAY, &opval, sizeof(opval));
    }
    
    return rc;
}

