/******************************************************************************
 *
 *  Copyright 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include "hci/btsnoop.h"
#include "hci/hci_layer.h"
#include "osi/mutex.h"

#if CONFIG_BT_SNOOP_ENABLED
#define LOG_TAG "bt_snoop"

#define DEFAULT_BTSNOOP_PATH "/data/btsnoop_hci.log"
#define PACKET_PER_FILE     8192
#define LOCK_TIMEOUT        1024

typedef enum {
    kCommandPacket = 1,
    kAclPacket = 2,
    kScoPacket = 3,
    kEventPacket = 4
} packet_type_t;

typedef struct {
    uint32_t length_original;
    uint32_t length_captured;
    uint32_t flags;
    uint32_t dropped_packets;
    uint64_t timestamp;
    uint8_t type;
} __attribute__((__packed__)) btsnoop_header_t;

static btsnoop_net_callback g_btsnoop_net_callback;

// Epoch in microseconds since 01/01/0000.
static const uint64_t BTSNOOP_EPOCH_DELTA = 0x00dcddb30f2f8000ULL;

static FILE *logfile_fd = NULL;
static osi_mutex_t btsnoop_mutex;

static int32_t packets_per_file;
static int32_t packet_counter;

bool btsnoop_net_write(const void* data, size_t length);

#ifndef htonl
#define htonl( X ) \
    ( (uint32_t)( \
        ( ( ( (uint32_t)(X) ) << 24 ) & UINT32_C( 0xFF000000 ) ) | \
        ( ( ( (uint32_t)(X) ) <<  8 ) & UINT32_C( 0x00FF0000 ) ) | \
        ( ( ( (uint32_t)(X) ) >>  8 ) & UINT32_C( 0x0000FF00 ) ) | \
        ( ( ( (uint32_t)(X) ) >> 24 ) & UINT32_C( 0x000000FF ) ) ) )
#endif

static uint64_t htonll(uint64_t ll) 
{
    const uint32_t l = 1;
    if (*((uint8_t*)(&l)) == 1)
        return (uint64_t)((((uint64_t)htonl(ll & 0xffffffff)) << 32) | htonl(ll >> 32));

    return ll;
}

void btsnoop_mem_capture(const BT_HDR* packet, uint64_t timestamp_us)
{
    return;
}

static void delete_btsnoop_files(void)
{
    if (logfile_fd) {
        fclose(logfile_fd);
        logfile_fd = NULL;
    }
}

static bool is_btsnoop_enabled(void)
{
    return 1;
}

static void open_snoop_file()
{
    char *log_path = DEFAULT_BTSNOOP_PATH;
    char data[9] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0xea};
    
    if (logfile_fd) {
        fclose(logfile_fd);
        logfile_fd = NULL;
    }
    
    if (logfile_fd == NULL) {
        logfile_fd = fopen(log_path, "wb");
        if (logfile_fd == NULL) {
            HCI_TRACE_ERROR("%s unable to open '%s'", __func__, log_path);
            return;
        }
        packet_counter = 0;

        fprintf(logfile_fd, "btsnoop");
        fwrite(data, 1, 9, logfile_fd);
    }
    return;
}
static void btsnoop_write_packet(packet_type_t type, uint8_t* packet,
                                 bool is_received, uint64_t timestamp_us)
{
    uint32_t length_he = 0;
    uint32_t flags = 0;
    btsnoop_header_t header;
    
    switch (type) {
    case kCommandPacket:
        length_he = packet[2] + 4;
        flags = 2;
        break;
    case kAclPacket:
        length_he = (packet[3] << 8) + packet[2] + 5;
        flags = is_received;
        break;
    case kScoPacket:
        length_he = packet[2] + 4;
        flags = is_received;
        break;
    case kEventPacket:
        length_he = packet[1] + 3;
        flags = 3;
        break;
    }

    header.length_original = htonl(length_he);
    header.length_captured = header.length_original;
    header.flags = htonl(flags);
    header.dropped_packets = 0;
    header.timestamp = htonll(timestamp_us + BTSNOOP_EPOCH_DELTA);
    header.type = type;

    btsnoop_net_write(&header, sizeof(btsnoop_header_t));
    if (btsnoop_net_write(packet, length_he - 1)) {
        return;
    }

    if (logfile_fd != NULL) {
        packet_counter++;
        if (packet_counter > packets_per_file) {
            fclose(logfile_fd);
            logfile_fd = NULL;
            return;
        }

        fwrite(&header, sizeof(btsnoop_header_t), 1, logfile_fd);
        fwrite(packet, length_he - 1, 1, logfile_fd);
    }
}

btsnoop_net_callback btsnoop_net_get_callback(void)
{
    return g_btsnoop_net_callback;
}

void btsnoop_net_set_callback(btsnoop_net_callback callback)
{
    g_btsnoop_net_callback = callback;
}

void btsnoop_net_open()
{
    return;
}

void btsnoop_net_close()
{
    return;
}

bool btsnoop_net_write(const void* data, size_t length)
{
    btsnoop_net_callback callback = btsnoop_net_get_callback();
    
    if (callback) {
        callback(data, length);
        return 1;
    }
    return 0;
}

void start_up(void)
{
    osi_mutex_new(&btsnoop_mutex);

    if(!is_btsnoop_enabled())
        return;
    
    if (btsnoop_net_get_callback()) {
        return;
    }

    open_snoop_file();
    packets_per_file = PACKET_PER_FILE;
    btsnoop_net_open();

    return;
}

void shut_down(void)
{
    osi_mutex_lock(&btsnoop_mutex, LOCK_TIMEOUT);
    if (!is_btsnoop_enabled()) {
        delete_btsnoop_files();
    }

    if (logfile_fd != NULL) {
        fclose(logfile_fd);
    }
    logfile_fd = NULL;

    btsnoop_net_close();
    osi_mutex_unlock(&btsnoop_mutex);

    return;
}

// Interface functions
static void capture(const BT_HDR* buffer, bool is_received)
{
    uint8_t *p = (uint8_t *)(buffer->data + buffer->offset);
    uint64_t timestamp_us;
    struct timeval tv = {};

    osi_mutex_lock(&btsnoop_mutex, LOCK_TIMEOUT);
    
    gettimeofday(&tv, NULL);
    timestamp_us = (uint64_t)tv.tv_sec*1000000ULL+(uint64_t)tv.tv_usec;
    btsnoop_mem_capture(buffer, timestamp_us);

    if ((btsnoop_net_get_callback() == NULL) && (logfile_fd == NULL)) {
        osi_mutex_unlock(&btsnoop_mutex);
        return;
    }
    switch (buffer->event & MSG_EVT_MASK) {
    case MSG_HC_TO_STACK_HCI_EVT:
        btsnoop_write_packet(kEventPacket, p, false, timestamp_us);
        break;
    case MSG_HC_TO_STACK_HCI_ACL:
    case MSG_STACK_TO_HC_HCI_ACL:
        btsnoop_write_packet(kAclPacket, p, is_received, timestamp_us);
        break;
    case MSG_HC_TO_STACK_HCI_SCO:
    case MSG_STACK_TO_HC_HCI_SCO:
        btsnoop_write_packet(kScoPacket, p, is_received, timestamp_us);
        break;
    case MSG_STACK_TO_HC_HCI_CMD:
        btsnoop_write_packet(kCommandPacket, p, true, timestamp_us);
        break;
    }
    osi_mutex_unlock(&btsnoop_mutex);
}

static const btsnoop_t interface = {
    start_up,
    shut_down,
    capture,
};

const btsnoop_t* btsnoop_get_interface() {
    return &interface;
}
#endif

