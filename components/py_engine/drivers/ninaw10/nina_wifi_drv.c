/*
 * This file is part of the OpenMV project, https://openmv.io.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * NINA-W10 WiFi driver.
 */

#include "py/mphal.h"

#if MICROPY_PY_NETWORK_NINAW10

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "nina_bsp.h"
#include "nina_wifi_drv.h"

#define SPI_ACK                 (1)
#define SPI_ERR                 (0xFF)

#define NO_SOCKET_AVAIL         (255)

#define CMD_START               (0xE0)
#define CMD_END                 (0xEE)
#define CMD_ERROR               (0xEF)
#define CMD_REPLY               (1 << 7)

#define ARG_8BITS               (1)
#define ARG_16BITS              (2)

#define ARG_STR(x)              {strlen(x), (const void *)x}
#define ARG_BYTE(x)             {1, (uint8_t  [1]) {x}}
#define ARG_SHORT(x)            {2, (uint16_t [1]) {x}}
#define ARG_WORD(x)             {4, (uint32_t [1]) {x}}

#define NINA_ARGS(...)          (nina_args_t []) {__VA_ARGS__}
#define NINA_VALS(...)          (nina_vals_t []) {__VA_ARGS__}

#define NINA_SSELECT_TIMEOUT    (10000)
#define NINA_RESPONSE_TIMEOUT   (1000)
#define NINA_CONNECT_TIMEOUT    (10000)

#if NINA_DEBUG
#define debug_printf(...) mp_printf(&mp_plat_print, __VA_ARGS__)
#else
#define debug_printf(...)
#endif

#ifndef __REVSH
#define __REVSH(x) ((((uint16_t)x) << 8) | (((uint16_t)x) >> 8))
#endif

typedef struct {
    uint16_t size;
    const void *data;
} nina_args_t;

typedef struct {
    uint16_t *size;
    void *data;
} nina_vals_t;

typedef enum {
    // STA mode commands.
    NINA_CMD_CONNECT_OPEN           = 0x10,
    NINA_CMD_CONNECT_WEP            = 0x11,
    NINA_CMD_CONNECT_WPA            = 0x12,
    NINA_CMD_GET_SSID               = 0x23,
    NINA_CMD_GET_BSSID              = 0x24,
    NINA_CMD_GET_RSSI               = 0x25,
    NINA_CMD_GET_ENCRYPT            = 0x26,

    // AP mode commands.
    NINA_CMD_START_AP_OPEN          = 0x18,
    NINA_CMD_START_AP_WEP           = 0x19,

    // AP mode scan commands.
    NINA_CMD_AP_START_SCAN          = 0x36,
    NINA_CMD_AP_SCAN_RESULT         = 0x27,
    NINA_CMD_AP_GET_RSSI            = 0x32,
    NINA_CMD_AP_GET_ENCRYPT         = 0x33,
    NINA_CMD_AP_GET_BSSID           = 0x3C,
    NINA_CMD_AP_GET_CHANNEL         = 0x3D,

    // Disonnect/status commands.
    NINA_CMD_DISCONNECT             = 0x30,
    NINA_CMD_CONN_STATUS            = 0x20,

    // Interface config commands.
    NINA_CMD_SET_IF_CONFIG          = 0x14,
    NINA_CMD_GET_IF_CONFIG          = 0x21,
    NINA_CMD_SET_DNS_CONFIG         = 0x15,

    // Hostname/Resolv commands.
    NINA_CMD_SET_HOSTNAME           = 0x16,
    NINA_CMD_HOST_BY_NAME           = 0x34,
    NINA_CMD_GET_HOST_BY_NAME       = 0x35,

    // Misc commands.
    NINA_CMD_SET_POWER              = 0x17,
    NINA_CMD_PING                   = 0x3E,
    NINA_CMD_GET_TIME               = 0x3B,
    NINA_CMD_GET_FW_VERSION         = 0x37,
    NINA_CMD_DEBUG_MODE             = 0x1A,
    NINA_CMD_TEMP_SENSOR            = 0x1B,
    NINA_CMD_GET_MAC_ADDR           = 0x22,

    // Sockets commands.
    NINA_CMD_SOCKET_OPEN            = 0x3F,
    NINA_CMD_SOCKET_CLOSE           = 0x2E,
    NINA_CMD_SOCKET_CONNECT         = 0x2D,
    NINA_CMD_SOCKET_ACCEPT          = 0x2B,
    NINA_CMD_SOCKET_BIND            = 0x28,
    NINA_CMD_SOCKET_STATE           = 0x2F,
    NINA_CMD_SOCKET_REMOTE_ADDR     = 0x3A,

    // TCP commands
    NINA_CMD_TCP_SEND               = 0x44,
    NINA_CMD_TCP_RECV               = 0x45,
    NINA_CMD_TCP_ACK                = 0x2A,

    // UDP commands.
    NINA_CMD_UDP_SEND               = 0x46,
    NINA_CMD_UDP_RECV               = 0x45,
    NINA_CMD_UDP_ACK                = 0x39,

    // Pin control commands.
    NINA_CMD_SET_PIN_MODE           = 0x50,
    NINA_CMD_SET_DIGITAL_WRITE      = 0x51,
    NINA_CMD_GET_DIGITAL_READ       = 0x53,
    NINA_CMD_SET_ANALOG_WRITE       = 0x52,
    NINA_CMD_GET_ANALOG_READ        = 0x54,

    // File send/recv commands.
    NINA_CMD_CMD_WRITE_FILE         = 0x60,
    NINA_CMD_CMD_READ_FILE          = 0x61,
    NINA_CMD_CMD_DELETE_FILE        = 0x62,
    NINA_CMD_CMD_EXISTS_FILE        = 0x63,
    NINA_CMD_CMD_DOWNLOAD_FILE      = 0x64,

    // OTA upgrade commands.
    NINA_CMD_CMD_APPLY_OTA          = 0x65,
    NINA_CMD_CMD_RENAME_FILE        = 0x66,
    NINA_CMD_CMD_DOWNLOAD_OTA       = 0x67,
} nina_cmd_t;

typedef enum {
    NINA_STATUS_IDLE      = 0,
    NINA_STATUS_NO_SSID_AVAIL,
    NINA_STATUS_SCAN_COMPLETED,
    NINA_STATUS_CONNECTED,
    NINA_STATUS_CONNECT_FAILED,
    NINA_STATUS_CONNECTION_LOST,
    NINA_STATUS_DISCONNECTED,
    NINA_STATUS_AP_LISTENING,
    NINA_STATUS_AP_CONNECTED,
    NINA_STATUS_AP_FAILED
} nina_status_t;

typedef enum  {
    SOCKET_STATE_CLOSED = 0,
    SOCKET_STATE_LISTEN,
    SOCKET_STATE_SYN_SENT,
    SOCKET_STATE_SYN_RCVD,
    SOCKET_STATE_ESTABLISHED,
    SOCKET_STATE_FIN_WAIT_1,
    SOCKET_STATE_FIN_WAIT_2,
    SOCKET_STATE_CLOSE_WAIT,
    SOCKET_STATE_CLOSING,
    SOCKET_STATE_LAST_ACK,
    SOCKET_STATE_TIME_WAIT
} nina_sock_state_t;

static uint8_t nina_bsp_spi_read_byte(void) {
    uint8_t byte = 0;
    nina_bsp_spi_transfer(NULL, &byte, 1);
    return byte;
}

static int nina_wait_for_cmd(uint8_t cmd, uint32_t timeout) {
    uint8_t buf = 0;
    for (mp_uint_t start = mp_hal_ticks_ms(); ;) {
        buf = nina_bsp_spi_read_byte();
        if (buf == CMD_ERROR || buf == cmd
            || ((mp_hal_ticks_ms() - start) >= timeout)) {
            break;
        }
        mp_hal_delay_ms(1);
    }

    return (buf == cmd) ? 0 : -1;
}

static int nina_send_command(uint32_t cmd, uint32_t nargs, uint32_t width, nina_args_t *args) {
    int ret = -1;
    uint32_t length = 4; // 3 bytes header + 1 end byte

    debug_printf("nina_send_command (cmd 0x%x nargs %d width %d): ", cmd, nargs, width);

    if (nina_bsp_spi_slave_select(NINA_SSELECT_TIMEOUT) != 0) {
        return -1;
    }

    // Send command header.
    uint8_t cmdbuf_hdr[3] = {CMD_START, cmd, nargs};
    if (nina_bsp_spi_transfer(cmdbuf_hdr, NULL, sizeof(cmdbuf_hdr)) != 0) {
        goto error_out;
    }

    // Send command arg(s).
    for (uint32_t i = 0; i < nargs; i++) {
        // Send size MSB first if 2 bytes.
        uint16_t size = (width == ARG_8BITS) ? args[i].size : __REVSH(args[i].size);

        // Send arg length.
        if (nina_bsp_spi_transfer((uint8_t *)&size, NULL, width) != 0) {
            goto error_out;
        }

        // Send arg value.
        if (nina_bsp_spi_transfer(args[i].data, NULL, args[i].size) != 0) {
            goto error_out;
        }
        length += args[i].size + width;
    }

    // Send END byte + padding to multiple of 4.
    uint8_t cmdbuf_end[4] = {CMD_END, 0xFF, 0xFF, 0xFF};
    if (nina_bsp_spi_transfer(cmdbuf_end, NULL, 1 + (length % 4)) != 0) {
        goto error_out;
    }

    // All good
    ret = 0;

error_out:
    debug_printf("\n");
    nina_bsp_spi_slave_deselect();
    return ret;
}

static int nina_read_response(uint32_t cmd, uint32_t nvals, uint32_t width, nina_vals_t *vals) {
    int ret = -1;

    debug_printf("nina_read_response(cmd 0x%x nvals %d width %d): ", cmd, nvals, width);

    // Read reply
    if (nina_bsp_spi_slave_select(NINA_SSELECT_TIMEOUT) != 0) {
        return -1;
    }

    // Wait for CMD_START
    if (nina_wait_for_cmd(CMD_START, NINA_RESPONSE_TIMEOUT) != 0) {
        goto error_out;
    }

    // Should return CMD + REPLY flag.
    if (nina_bsp_spi_read_byte() != (cmd | CMD_REPLY)) {
        goto error_out;
    }

    // Sanity check the number of returned values.
    // NOTE: This is to handle the special case for the scan command.
    uint32_t rvals = nina_bsp_spi_read_byte();
    if (nvals > rvals) {
        nvals = rvals;
    }

    // Read return value(s).
    for (uint32_t i = 0; i < nvals; i++) {
        // Read return value size.
        uint16_t bytes = nina_bsp_spi_read_byte();
        if (width == ARG_16BITS) {
            bytes = (bytes << 8) | nina_bsp_spi_read_byte();
        }

        // Check the val fits the buffer.
        if (*(vals[i].size) < bytes) {
            goto error_out;
        }

        // Read the returned value.
        if (nina_bsp_spi_transfer(NULL, vals[i].data, bytes) != 0) {
            goto error_out;
        }

        // Set the size.
        *(vals[i].size) = bytes;
    }

    if (nina_bsp_spi_read_byte() != CMD_END) {
        goto error_out;
    }

    // All good
    ret = 0;

error_out:
    debug_printf("\n");
    nina_bsp_spi_slave_deselect();
    return ret;
}

static int nina_send_command_read_ack(uint32_t cmd, uint32_t nargs, uint32_t width, nina_args_t *args) {
    uint16_t size = 1;
    uint8_t rval = SPI_ERR;
    if (nina_send_command(cmd, nargs, width, args) != 0 ||
        nina_read_response(cmd, 1, ARG_8BITS, NINA_VALS({&size, &rval})) != 0) {
        return -1;
    }
    return rval;
}

static int nina_send_command_read_vals(uint32_t cmd, uint32_t nargs,
    uint32_t argsw, nina_args_t *args, uint32_t nvals, uint32_t valsw, nina_vals_t *vals) {

    if (nina_send_command(cmd, nargs, argsw, args) != 0 ||
        nina_read_response(cmd, nvals, valsw, vals) != 0) {
        return -1;
    }
    return 0;
}

static void nina_fix_mac_addr(uint8_t *mac) {
    for (int i = 0; i < 3; i++) {
        uint8_t b = mac[i];
        mac[i] = mac[5 - i];
        mac[5 - i] = b;
    }
}

int nina_init(void) {
    // Initialize the BSP.
    nina_bsp_init();
    return 0;
}

int nina_deinit(void) {
    return nina_bsp_deinit();
}

static int nina_connection_status() {
    return nina_send_command_read_ack(NINA_CMD_CONN_STATUS, 0, ARG_8BITS, NULL);
}

static int nina_socket_status(uint8_t fd) {
    return nina_send_command_read_ack(NINA_CMD_SOCKET_STATE,
        1, ARG_8BITS, NINA_ARGS(ARG_BYTE(fd)));
}

static int nina_server_socket_status(uint8_t fd) {
    return nina_send_command_read_ack(NINA_CMD_SOCKET_STATE & 0xF9,
        1, ARG_8BITS, NINA_ARGS(ARG_BYTE(fd)));
}

int nina_connect(const char *ssid, uint8_t security, const char *key, uint16_t channel) {
    uint8_t status = NINA_STATUS_CONNECT_FAILED;

    if (key == NULL && security != NINA_SEC_OPEN) {
        return -1;
    }

    switch (security) {
        case NINA_SEC_OPEN:
            if (nina_send_command_read_ack(NINA_CMD_CONNECT_OPEN,
                1, ARG_8BITS, NINA_ARGS(ARG_STR(ssid))) != SPI_ACK) {
                return -1;
            }
            break;
        case NINA_SEC_WEP:
            if (nina_send_command_read_ack(NINA_CMD_CONNECT_WEP,
                2, ARG_8BITS, NINA_ARGS(ARG_STR(ssid), ARG_STR(key))) != SPI_ACK) {
                return -1;
            }
            break;
        case NINA_SEC_WPA_PSK:
            if (nina_send_command_read_ack(NINA_CMD_CONNECT_WPA,
                3, ARG_8BITS, NINA_ARGS(ARG_STR(ssid), ARG_BYTE(0), ARG_STR(key))) != SPI_ACK) {
                return -1;
            }
            break;
        default:
            return -1;
    }

    for (mp_uint_t start = mp_hal_ticks_ms(); ; mp_hal_delay_ms(10)) {
        status = nina_connection_status();
        if ((status != NINA_STATUS_IDLE) && (status != NINA_STATUS_NO_SSID_AVAIL) && (status != NINA_STATUS_SCAN_COMPLETED)) {
            break;
        }

        if ((mp_hal_ticks_ms() - start) >= NINA_CONNECT_TIMEOUT) {
            break;
        }
    }

    return (status == NINA_STATUS_CONNECTED) ? 0 : -1;
}

int nina_start_ap(const char *ssid, uint8_t security, const char *key, uint16_t channel) {
    uint8_t status = NINA_STATUS_AP_FAILED;

    if ((key == NULL && security != NINA_SEC_OPEN) ||
        (security != NINA_SEC_OPEN && security != NINA_SEC_WEP)) {
        return -1;
    }

    switch (security) {
        case NINA_SEC_OPEN:
            if (nina_send_command_read_ack(NINA_CMD_START_AP_OPEN,
                2, ARG_8BITS, NINA_ARGS(ARG_STR(ssid), ARG_BYTE(channel))) != SPI_ACK) {
                return -1;
            }
            break;
        case NINA_SEC_WEP:
            if (nina_send_command_read_ack(NINA_CMD_START_AP_WEP,
                3, ARG_8BITS, NINA_ARGS(ARG_STR(ssid), ARG_STR(key), ARG_BYTE(channel))) != SPI_ACK) {
                return -1;
            }
            break;
        default:
            return -1;
    }

    for (mp_uint_t start = mp_hal_ticks_ms(); ; mp_hal_delay_ms(10)) {
        status = nina_connection_status();
        if ((status != NINA_STATUS_IDLE) && (status != NINA_STATUS_NO_SSID_AVAIL) && (status != NINA_STATUS_SCAN_COMPLETED)) {
            break;
        }

        if ((mp_hal_ticks_ms() - start) >= NINA_CONNECT_TIMEOUT) {
            break;
        }
    }

    return (status == NINA_STATUS_AP_LISTENING) ? 0 : -1;
}

int nina_disconnect(void) {
    if (nina_send_command_read_ack(NINA_CMD_DISCONNECT,
        1, ARG_8BITS, NINA_ARGS(ARG_BYTE(0xFF))) != SPI_ACK) {
        return -1;
    }
    return 0;
}

int nina_isconnected(void) {
    int status = nina_connection_status();
    if (status == -1) {
        return -1;
    }
    return status == NINA_STATUS_CONNECTED;
}

int nina_connected_sta(uint32_t *sta_ip) {
    return -1;
}

int nina_wait_for_sta(uint32_t *sta_ip, uint32_t timeout) {
    return NINA_ERROR_TIMEOUT;
}

int nina_ifconfig(nina_ifconfig_t *ifconfig, bool set) {
    uint16_t ip_len = NINA_IPV4_ADDR_LEN;
    uint16_t sub_len = NINA_IPV4_ADDR_LEN;
    uint16_t gw_len = NINA_IPV4_ADDR_LEN;
    uint16_t dns_len = NINA_IPV4_ADDR_LEN;

    if (set) {
        if (nina_send_command_read_ack(NINA_CMD_SET_IF_CONFIG,
            4, ARG_8BITS,
            NINA_ARGS(
                ARG_BYTE(3),         // Valid number of args.
                {ip_len,  ifconfig->ip_addr},
                {gw_len,  ifconfig->gateway_addr},
                {sub_len, ifconfig->subnet_addr})) != 0) {
            return -1;
        }

        uint8_t dns2[4] = {8, 8, 8, 8};
        if (nina_send_command_read_ack(NINA_CMD_SET_DNS_CONFIG,
            3, ARG_8BITS,
            NINA_ARGS(
                ARG_BYTE(1),         // Valid number of args.
                {dns_len, ifconfig->dns_addr},
                {dns_len, dns2})) != SPI_ACK) {
            return -1;
        }

    } else {
        if (nina_send_command_read_vals(NINA_CMD_GET_IF_CONFIG,
            1, ARG_8BITS, NINA_ARGS(ARG_BYTE(0xFF)),
            3, ARG_8BITS,
            NINA_VALS(
                {&ip_len,  ifconfig->ip_addr},
                {&sub_len, ifconfig->subnet_addr},
                {&gw_len,  ifconfig->gateway_addr})) != 0) {
            return -1;
        }
        // No command to get DNS ?
        memcpy(ifconfig->dns_addr, ifconfig->gateway_addr, NINA_IPV4_ADDR_LEN);
    }
    return 0;
}

int nina_netinfo(nina_netinfo_t *netinfo) {
    uint16_t rssi_len = 4;
    uint16_t sec_len = 1;
    uint16_t ssid_len = NINA_MAX_SSID_LEN;
    uint16_t bssid_len = NINA_MAC_ADDR_LEN;

    if (nina_send_command_read_vals(NINA_CMD_GET_RSSI,
        1, ARG_8BITS, NINA_ARGS(ARG_BYTE(0xFF)),
        1, ARG_8BITS, NINA_VALS({&rssi_len, &netinfo->rssi})) != 0) {
        return -1;
    }

    if (nina_send_command_read_vals(NINA_CMD_GET_ENCRYPT,
        1, ARG_8BITS, NINA_ARGS(ARG_BYTE(0xFF)),
        1, ARG_8BITS, NINA_VALS({&sec_len, &netinfo->security})) != 0) {
        return -1;
    }

    if (nina_send_command_read_vals(NINA_CMD_GET_SSID,
        1, ARG_8BITS, NINA_ARGS(ARG_BYTE(0xFF)),
        1, ARG_8BITS, NINA_VALS({&ssid_len, &netinfo->ssid})) != 0) {
        return -1;
    }

    // Null terminate SSID.
    netinfo->ssid[MIN((NINA_MAX_SSID_LEN - 1), ssid_len)] = 0;

    if (nina_send_command_read_vals(NINA_CMD_GET_BSSID,
        1, ARG_8BITS, NINA_ARGS(ARG_BYTE(0xFF)),
        1, ARG_8BITS, NINA_VALS({&bssid_len, &netinfo->bssid})) != 0) {
        return -1;
    }

    // The MAC address is read in reverse from the firmware.
    nina_fix_mac_addr(netinfo->bssid);

    return 0;
}

int nina_scan(nina_scan_callback_t scan_callback, void *arg, uint32_t timeout) {
    uint16_t sizes[NINA_MAX_NETWORK_LIST];
    char ssids[NINA_MAX_NETWORK_LIST][NINA_MAX_SSID_LEN];
    nina_vals_t vals[NINA_MAX_NETWORK_LIST];

    // Initialize the values list.
    for (int i = 0; i < NINA_MAX_NETWORK_LIST; i++) {
        sizes[i] = NINA_MAX_SSID_LEN - 1;
        memset(ssids[i], 0, NINA_MAX_SSID_LEN);
        vals[i].size = &sizes[i];
        vals[i].data = ssids[i];
    }

    if (nina_send_command_read_ack(NINA_CMD_AP_START_SCAN,
        0, ARG_8BITS, NULL) != SPI_ACK) {
        return -1;
    }

    for (mp_uint_t start = mp_hal_ticks_ms(); ;) {
        if (nina_send_command_read_vals(NINA_CMD_AP_SCAN_RESULT,
            0, ARG_8BITS, NULL,
            NINA_MAX_NETWORK_LIST, ARG_8BITS, vals) != 0) {
            return -1;
        }

        if (ssids[0][0] != 0) {
            // Found at least 1 network.
            break;
        }

        if (timeout && (mp_hal_ticks_ms() - start) >= timeout) {
            // Timeout, no networks.
            return NINA_ERROR_TIMEOUT;
        }

        mp_hal_delay_ms(100);
    }

    for (int i = 0; i < NINA_MAX_NETWORK_LIST; i++) {
        uint16_t rssi_len = 4;
        uint16_t sec_len = 1;
        uint16_t chan_len = 1;
        uint16_t bssid_len = NINA_MAC_ADDR_LEN;
        nina_scan_result_t scan_result;

        if (ssids[i][0] == 0) {
            break;
        }

        // Set AP SSID
        strncpy(scan_result.ssid, ssids[i], NINA_MAX_SSID_LEN);

        // Read AP RSSI
        if (nina_send_command_read_vals(NINA_CMD_AP_GET_RSSI,
            1, ARG_8BITS, NINA_ARGS(ARG_BYTE(i)),
            1, ARG_8BITS, NINA_VALS({&rssi_len, &scan_result.rssi})) != 0) {
            return -1;
        }

        // Read AP encryption type
        if (nina_send_command_read_vals(NINA_CMD_AP_GET_ENCRYPT,
            1, ARG_8BITS, NINA_ARGS(ARG_BYTE(i)),
            1, ARG_8BITS, NINA_VALS({&sec_len, &scan_result.security})) != 0) {
            return -1;
        }

        // Read AP channel
        if (nina_send_command_read_vals(NINA_CMD_AP_GET_CHANNEL,
            1, ARG_8BITS, NINA_ARGS(ARG_BYTE(i)),
            1, ARG_8BITS, NINA_VALS({&chan_len, &scan_result.channel})) != 0) {
            return -1;
        }

        // Read AP bssid
        if (nina_send_command_read_vals(NINA_CMD_AP_GET_BSSID,
            1, ARG_8BITS, NINA_ARGS(ARG_BYTE(i)),
            1, ARG_8BITS, NINA_VALS({&bssid_len, scan_result.bssid})) != 0) {
            return -1;
        }

        // The MAC address is read in reverse from the firmware.
        nina_fix_mac_addr(scan_result.bssid);

        scan_callback(&scan_result, arg);
    }

    return 0;
}

int nina_get_rssi(void) {
    uint16_t size = 4;
    int32_t rssi = 0;
    if (nina_send_command_read_vals(NINA_CMD_GET_RSSI,
        1, ARG_8BITS, NINA_ARGS(ARG_BYTE(0xFF)),
        1, ARG_8BITS, NINA_VALS({&size, &rssi})) != 0) {
        return -1;
    }

    return rssi;
}

int nina_fw_version(uint8_t *fw_ver) {
    uint16_t size = NINA_FW_VER_LEN;
    if (nina_send_command_read_vals(NINA_CMD_GET_FW_VERSION,
        0, ARG_8BITS, NULL,
        1, ARG_8BITS, NINA_VALS({&size, fw_ver})) != 0) {
        return -1;
    }
    return 0;
}

int nina_set_hostname(const char *hostname) {
    if (nina_send_command_read_ack(NINA_CMD_SET_HOSTNAME,
        1, ARG_8BITS, NINA_ARGS(ARG_STR(hostname))) != SPI_ACK) {
        return -1;
    }
    return 0;
}

int nina_gethostbyname(const char *name, uint8_t *out_ip) {
    uint16_t size = 4;

    if (nina_send_command_read_ack(NINA_CMD_HOST_BY_NAME,
        1, ARG_8BITS, NINA_ARGS(ARG_STR(name))) != SPI_ACK) {
        return -1;
    }

    if (nina_send_command_read_vals(NINA_CMD_GET_HOST_BY_NAME,
        0, ARG_8BITS, NULL,
        1, ARG_8BITS, NINA_VALS({&size, out_ip})) != 0) {
        return -1;
    }
    return 0;
}

int nina_socket_socket(uint8_t type) {
    uint16_t size = 1;
    uint8_t sock = 0;

    if (nina_send_command_read_vals(NINA_CMD_SOCKET_OPEN,
        0, ARG_8BITS, NULL,
        1, ARG_8BITS, NINA_VALS({&size, &sock})) != 0) {
        return -1;
    }
    return sock;
}

int nina_socket_close(int fd) {
    if (fd > 0 && fd < 255) {
        if (nina_send_command_read_ack(NINA_CMD_SOCKET_CLOSE,
            1, ARG_8BITS, NINA_ARGS(ARG_BYTE(fd))) != SPI_ACK) {
            return -1;
        }
        for (mp_uint_t start = mp_hal_ticks_ms(); ; mp_hal_delay_ms(10)) {
            if (nina_socket_status(fd) == SOCKET_STATE_CLOSED) {
                break;
            }
            if ((mp_hal_ticks_ms() - start) >= 5000) {
                return NINA_ERROR_TIMEOUT;
            }
        }
    }
    return 0;
}

int nina_socket_bind(int fd, uint8_t *ip, uint16_t port, int type) {
    if (nina_send_command_read_ack(NINA_CMD_SOCKET_BIND,
        3, ARG_8BITS,
        NINA_ARGS(
            ARG_SHORT(__REVSH(port)),
            ARG_BYTE(fd),
            ARG_BYTE(type))) != SPI_ACK) {
        return -1;
    }

    // Only TCP sockets' states should be checked.
    if (type == NINA_SOCKET_TYPE_TCP &&
        nina_server_socket_status(fd) != SOCKET_STATE_LISTEN) {
        return -1;
    }
    return 0;
}

int nina_socket_listen(int fd, uint32_t backlog) {
    return 0; // No listen ?
}

int nina_socket_accept(int fd, uint8_t *ip, uint16_t *port, int *fd_out, uint32_t timeout) {
    uint16_t size = 2;
    uint16_t sock = NO_SOCKET_AVAIL;

    if (nina_server_socket_status(fd) != SOCKET_STATE_LISTEN) {
        return -1;
    }

    for (mp_uint_t start = mp_hal_ticks_ms(); sock == 0 || sock == NO_SOCKET_AVAIL; mp_hal_delay_ms(10)) {
        if (nina_send_command_read_vals(NINA_CMD_SOCKET_ACCEPT,
            1, ARG_8BITS, NINA_ARGS(ARG_BYTE(fd)),
            1, ARG_8BITS, NINA_VALS({&size, &sock})) != 0) {
            return -1;
        }

        if (timeout && (mp_hal_ticks_ms() - start) >= timeout) {
            return NINA_ERROR_TIMEOUT;
        }
    }

    uint16_t port_len = 2;
    uint16_t ip_len = NINA_IPV4_ADDR_LEN;
    if (nina_send_command_read_vals(NINA_CMD_SOCKET_REMOTE_ADDR,
        1, ARG_8BITS, NINA_ARGS(ARG_BYTE(sock)),
        2, ARG_8BITS, NINA_VALS({&ip_len, ip}, {&port_len, port})) != 0) {
        return -1;
    }
    *fd_out = sock;
    *port = __REVSH(*port);
    return 0;
}

int nina_socket_connect(int fd, uint8_t *ip, uint16_t port, uint32_t timeout) {
    if (nina_send_command_read_ack(NINA_CMD_SOCKET_CONNECT,
        4, ARG_8BITS,
        NINA_ARGS(
            ARG_WORD((*(uint32_t *)ip)),
            ARG_SHORT(__REVSH(port)),
            ARG_BYTE(fd),
            ARG_BYTE(NINA_SOCKET_TYPE_TCP))) != SPI_ACK) {
        return -1;
    }

    for (mp_uint_t start = mp_hal_ticks_ms(); ; mp_hal_delay_ms(10)) {
        int state = nina_socket_status(fd);
        if (state == -1) {
            return -1;
        }

        if (state == SOCKET_STATE_ESTABLISHED) {
            break;
        }

        if (timeout && (mp_hal_ticks_ms() - start) >= timeout) {
            return NINA_ERROR_TIMEOUT;
        }
    }

    return 0;
}

int nina_socket_send(int fd, const uint8_t *buf, uint32_t len, uint32_t timeout) {
    uint16_t size = 2;
    uint16_t bytes = 0;

    if (nina_socket_status(fd) != SOCKET_STATE_ESTABLISHED) {
        return -1;
    }

    if (nina_send_command_read_vals(NINA_CMD_TCP_SEND,
        2, ARG_16BITS, NINA_ARGS(ARG_BYTE(fd), {len, buf}),
        1, ARG_8BITS, NINA_VALS({&size, &bytes})) != 0 || bytes <= 0) {
        return -1;
    }

    for (mp_uint_t start = mp_hal_ticks_ms(); ;) {
        int resp = nina_send_command_read_ack(NINA_CMD_TCP_ACK,
            1, ARG_8BITS, NINA_ARGS(ARG_BYTE(fd)));

        if (resp == -1) {
            return -1;
        }

        if (resp == SPI_ACK) {
            break;
        }

        if (timeout && (mp_hal_ticks_ms() - start) >= timeout) {
            return NINA_ERROR_TIMEOUT;
        }
        mp_hal_delay_ms(1);
    }

    return bytes;
}

int nina_socket_recv(int fd, uint8_t *buf, uint32_t len, uint32_t timeout) {
    uint16_t bytes = 0;

    if (nina_socket_status(fd) != SOCKET_STATE_ESTABLISHED) {
        return -1;
    }

    for (mp_uint_t start = mp_hal_ticks_ms(); bytes == 0; mp_hal_delay_ms(1)) {
        bytes = len;
        if (nina_send_command_read_vals(NINA_CMD_TCP_RECV,
            2, ARG_16BITS, NINA_ARGS(ARG_BYTE(fd), ARG_SHORT(bytes)),
            1, ARG_16BITS, NINA_VALS({&bytes, buf})) != 0) {
            return -1;
        }

        if (timeout && (mp_hal_ticks_ms() - start) >= timeout) {
            return NINA_ERROR_TIMEOUT;
        }
    }
    return bytes;
}

// Check from the upper layer if the socket is bound, if not then auto-bind it first.
int nina_socket_sendto(int fd, const uint8_t *buf, uint32_t len, uint8_t *ip, uint16_t port, uint32_t timeout) {
    // TODO do we need to split the packet somewhere?
    if (nina_send_command_read_ack(NINA_CMD_SOCKET_CONNECT,
        4, ARG_8BITS,
        NINA_ARGS(
            ARG_WORD((*(uint32_t *)ip)),
            ARG_SHORT(__REVSH(port)),
            ARG_BYTE(fd),
            ARG_BYTE(NINA_SOCKET_TYPE_UDP))) != SPI_ACK) {
        return -1;
    }

    // Buffer length and socket number are passed as 16bits.
    if (nina_send_command_read_ack(NINA_CMD_UDP_SEND,
        2, ARG_16BITS, NINA_ARGS(ARG_BYTE(fd), {len, buf})) != SPI_ACK) {
        return -1;
    }

    if (nina_send_command_read_ack(NINA_CMD_UDP_ACK,
        1, ARG_8BITS, NINA_ARGS(ARG_BYTE(fd))) != SPI_ACK) {
        return -1;
    }

    return 0;
}

// Check from the upper layer if the socket is bound, if not then auto-bind it first.
int nina_socket_recvfrom(int fd, uint8_t *buf, uint32_t len, uint8_t *ip, uint16_t *port, uint32_t timeout) {
    uint16_t bytes = 0;
    uint16_t port_len = 2;
    uint16_t ip_len = NINA_IPV4_ADDR_LEN;

    for (mp_uint_t start = mp_hal_ticks_ms(); bytes == 0; mp_hal_delay_ms(1)) {
        bytes = len;
        if (nina_send_command_read_vals(NINA_CMD_UDP_RECV,
            2, ARG_16BITS, NINA_ARGS(ARG_BYTE(fd), ARG_SHORT(bytes)),
            1, ARG_16BITS, NINA_VALS({&bytes, buf})) != 0) {
            return -1;
        }

        if (timeout && (mp_hal_ticks_ms() - start) >= timeout) {
            return NINA_ERROR_TIMEOUT;
        }
    }
    if (nina_send_command_read_vals(NINA_CMD_SOCKET_REMOTE_ADDR,
        1, ARG_8BITS, NINA_ARGS(ARG_BYTE(fd)),
        2, ARG_8BITS, NINA_VALS({&ip_len, ip}, {&port_len, port})) != 0) {
        return -1;
    }

    return bytes;
}

int nina_socket_setsockopt(int fd, uint32_t level, uint32_t opt, const void *optval, uint32_t optlen) {
    return -1;
}

#endif // MICROPY_PY_NINAW10
