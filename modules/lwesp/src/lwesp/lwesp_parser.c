/**
 * \file            lwesp_parser.c
 * \brief           Parse incoming data from AT port
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#include "lwesp/lwesp_private.h"
#include "lwesp/lwesp_parser.h"
#include "lwesp/lwesp_mem.h"

/* Increase pointer if matches character value */
#define INC_IF_CHAR_EQUAL(p, ch)            if (*(p) == (ch)) { ++(p); }

/**
 * \brief           Parse number from string
 * \note            Input string pointer is changed and number is skipped
 * \param[in,out]   str: Pointer to pointer to string to parse
 * \return          Parsed number
 */
int32_t
lwespi_parse_number(const char** str) {
    int32_t val = 0;
    uint8_t minus = 0;
    const char* p = *str;                       /*  */

    INC_IF_CHAR_EQUAL(p, '"');                  /* Skip leading quotes */
    INC_IF_CHAR_EQUAL(p, ',');                  /* Skip leading comma */
    INC_IF_CHAR_EQUAL(p, '"');                  /* Skip leading quotes */
    if (*p == '-') {                            /* Check negative number */
        minus = 1;
        ++p;
    }
    while (LWESP_CHARISNUM(*p)) {               /* Parse until character is valid number */
        val = val * 10 + LWESP_CHARTONUM(*p);
        ++p;
    }
    INC_IF_CHAR_EQUAL(p, ',');                  /* Go to next entry if possible */
    *str = p;                                   /* Save new pointer with new offset */

    return minus ? -val : val;
}

/**
 * \brief           Parse port from string
 * \note            Input string pointer is changed and number is skipped
 * \param[in,out]   str: Pointer to pointer to string to parse
 * \return          Parsed port number
 */
lwesp_port_t
lwespi_parse_port(const char** str) {
    lwesp_port_t p;

    p = (lwesp_port_t)lwespi_parse_number(str); /* Parse port */
    return p;
}

/**
 * \brief           Parse number from string as hex
 * \note            Input string pointer is changed and number is skipped
 * \param[in,out]   str: Pointer to pointer to string to parse
 * \return          Parsed number
 */
uint32_t
lwespi_parse_hexnumber(const char** str) {
    int32_t val = 0;
    const char* p = *str;                       /*  */

    INC_IF_CHAR_EQUAL(p, '"');                  /* Skip leading quotes */
    INC_IF_CHAR_EQUAL(p, ',');                  /* Skip leading comma */
    INC_IF_CHAR_EQUAL(p, '"');                  /* Skip leading quotes */
    while (LWESP_CHARISHEXNUM(*p)) {            /* Parse until character is valid number */
        val = val * 16 + LWESP_CHARHEXTONUM(*p);
        ++p;
    }
    INC_IF_CHAR_EQUAL(p, ',');                  /* Go to next entry if possible */
    *str = p;                                   /* Save new pointer with new offset */
    return val;
}

/**
 * \brief           Parse input string as string part of AT command
 * \param[in,out]   src: Pointer to pointer to string to parse from
 * \param[in]       dst: Destination pointer.
 *                      Set to `NULL` in case you want to skip string in source
 * \param[in]       dst_len: Length of destination buffer,
 *                      including memory for `NULL` termination
 * \param[in]       trim: Set to `1` to process entire string,
 *                      even if no memory anymore
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_string(const char** src, char* dst, size_t dst_len, uint8_t trim) {
    const char* p = *src;
    size_t i;

    INC_IF_CHAR_EQUAL(p, ',');                  /* Skip leading comma */
    INC_IF_CHAR_EQUAL(p, '"');                  /* Skip leading quotes */
    i = 0;
    if (dst_len > 0) {
        --dst_len;
    }
    while (*p) {
        if ((*p == '"' && (p[1] == ',' || p[1] == '\r' || p[1] == '\n'))
            || (*p == '\r' || *p == '\n')) {
            ++p;
            break;
        }
        if (dst != NULL) {
            if (i < dst_len) {
                *dst++ = *p;
                ++i;
            } else if (!trim) {
                break;
            }
        }
        ++p;
    }
    if (dst != NULL) {
        *dst = 0;
    }
    *src = p;
    return 1;
}

/**
 * \brief           Parse string as IP address
 * \param[in,out]   src: Pointer to pointer to string to parse from
 * \param[out]      ip: Pointer to IP memory
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_ip(const char** src, lwesp_ip_t* ip) {
    const char* p = *src;
#if LWESP_CFG_IPV6
    char c;
#endif /* LWESP_CFG_IPV6 */

    INC_IF_CHAR_EQUAL(p, '"');                  /* Skip leading quotes */

#if LWESP_CFG_IPV6
    /* Find first separator */
    c = 0;
    for (size_t i = 0; i < 6; ++i) {
        if (p[i] == ':' || p[i] == ',') {
            c = p[i];
            break;
        }
    }
#endif /* LWESP_CFG_IPV6 */

    /* Go to original value */
    if (0) {
#if LWESP_CFG_IPV6
    } else if (c == ':') {
        ip->type = LWESP_IPTYPE_V6;

        /*
         * Reset structure first.
         *
         * IPv6 IP can have omitted zeros to the end
         * so it is important to cleanup structure first,
         * not to keep wrong address
         */
        memset(&ip->addr, 0x00, sizeof(ip->addr));
        for (size_t i = 0; i < LWESP_ARRAYSIZE(ip->addr.ip6.addr); ++i, ++p) {
            ip->addr.ip6.addr[i] = (uint16_t)lwespi_parse_hexnumber(&p);
            if (*p != ':') {
                break;
            }
        }
#endif /* LWESP_CFG_IPV6 */
    } else {
        ip->type = LWESP_IPTYPE_V4;
        for (size_t i = 0; i < LWESP_ARRAYSIZE(ip->addr.ip4.addr); ++i, ++p) {
            ip->addr.ip4.addr[i] = lwespi_parse_number(&p);
            if (*p != '.') {
                break;
            }
        }
    }
    INC_IF_CHAR_EQUAL(p, '"');                  /* Skip trailing quotes */

    *src = p;                                   /* Set new pointer */
    return 1;
}

/**
 * \brief           Parse string as MAC address
 * \param[in,out]   src: Pointer to pointer to string to parse from
 * \param[out]      mac: Pointer to MAC memory
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_mac(const char** src, lwesp_mac_t* mac) {
    const char* p = *src;

    INC_IF_CHAR_EQUAL(p, '"');                  /* Skip leading quotes */
    mac->mac[0] = lwespi_parse_hexnumber(&p);
    ++p;
    mac->mac[1] = lwespi_parse_hexnumber(&p);
    ++p;
    mac->mac[2] = lwespi_parse_hexnumber(&p);
    ++p;
    mac->mac[3] = lwespi_parse_hexnumber(&p);
    ++p;
    mac->mac[4] = lwespi_parse_hexnumber(&p);
    ++p;
    mac->mac[5] = lwespi_parse_hexnumber(&p);
    INC_IF_CHAR_EQUAL(p, '"');                  /* Skip trailing quotes */
    INC_IF_CHAR_EQUAL(p, ',');                  /* Go to next entry if possible */
    *src = p;                                   /* Set new pointer */
    return 1;
}

/**
 * \brief           Parse +CIPSTATUS or +CIPSTATE response from ESP device
 * \param[in]       str: Input string to parse
 * \return          Member of \ref lwespr_t enumeration
 */
lwespr_t
lwespi_parse_cipstatus_cipstate(const char* str) {
    uint8_t cn_num = 0;

    cn_num = lwespi_parse_number(&str);         /* Parse connection number */
    esp.m.active_conns |= 1 << cn_num;          /* Set flag as active */

    lwespi_parse_string(&str, NULL, 0, 1);      /* Parse string and ignore result */
    lwespi_parse_ip(&str, &esp.m.conns[cn_num].remote_ip);
    esp.m.conns[cn_num].remote_port = lwespi_parse_number(&str);
    esp.m.conns[cn_num].local_port = lwespi_parse_number(&str);
    esp.m.conns[cn_num].status.f.client = !lwespi_parse_number(&str);

    return lwespOK;
}

#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE || __DOXYGEN__
/**
 * \brief           Parse +CIPRECVDATA statement
 * \param[in]       str: Input string to parse
 * \return          Member of \ref lwespr_t enumeration
 */
lwespr_t
lwespi_parse_ciprecvdata(const char* str) {
    size_t len;

    if (*str == '+') {
        str += 13;
    }

    /* Check data length */
    if ((len = lwespi_parse_number(&str)) > 0) {/* Get number of bytes to read */
        lwesp_ip_t ip;
        lwesp_port_t port;

        esp.m.ipd.read = 1;                     /* Start reading network data */
        esp.m.ipd.tot_len = len;                /* Total number of bytes in this received packet */
        esp.m.ipd.rem_len = len;                /* Number of remaining bytes to read */

        lwespi_parse_ip(&str, &ip);
        port = lwespi_parse_port(&str);
        lwesp_pbuf_set_ip(esp.m.ipd.buff, &ip, port);
    }
    return lwespOK;
}

/**
 * \brief           Parse +CIPRECVLEN statement
 * \param[in]       str: Input string to parse
 * \return          Member of \ref lwespr_t enumeration
 */
lwespr_t
lwespi_parse_ciprecvlen(const char* str) {
    int32_t len;
    if (*str == '+') {
        str += 12;
    }

    for (size_t i = 0; i < LWESP_CFG_MAX_CONNS; ++i) {
        len = lwespi_parse_number(&str);
        if (len < 0) {
            continue;
        }
        esp.m.conns[i].tcp_available_bytes = len;
    }

    return lwespOK;
}
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE || __DOXYGEN__ */

/**
 * \brief           Parse +IPD statement
 * \param[in]       str: Input string to parse
 * \return          Member of \ref lwespr_t enumeration
 */
lwespr_t
lwespi_parse_ipd(const char* str) {
    uint8_t conn, is_data_ipd;
    size_t len;
    lwesp_conn_p c;

    if (*str == '+') {
        str += 5;
    }

    conn = lwespi_parse_number(&str);           /* Parse number for connection number */
    len = lwespi_parse_number(&str);            /* Parse number for number of available_bytes/bytes_to_read */

    c = conn < LWESP_CFG_MAX_CONNS ? &esp.m.conns[conn] : NULL; /* Get connection handle */
    if (c == NULL) {                            /* Invalid connection number */
        return lwespERR;
    }

    /*
     * First check if this string is "notification only" or actual "data packet".
     *
     * Take decision based on ':' character before data. We can expect 3 types of format:
     *
     * +IPD,conn_num,available_bytes<CR><LF>                    : Notification only, for TCP connection
     * +IPD,conn_num,bytes_in_packet:data                       : Data packet w/o remote ip/port,
     *                                                              as response on manual TCP read or if AT+CIPDINFO=0
     * +IPD,conn_num,bytes_in_packet,remote_ip,remote_port:data : Data packet w/ remote ip/port,
     *                                                              as response on automatic read of all connection types
     */
    is_data_ipd = strchr(str, ':') != NULL;     /* Check if we have ':' in string */

#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
    /*
     * Check if +IPD is only notification and not actual data packet
     */
    if (!is_data_ipd) {                         /* If not data packet */
        c->tcp_available_bytes = len;           /* Set new value for number of bytes available to read from device */
    } else
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */
        /*
         * If additional information are enabled (IP and PORT),
         * parse them and save.
         *
         * Even if information is enabled, in case of manual TCP
         * receive, these information are not present.
         *
         * Check for ':' character if it is end of string and determine how to proceed
         */
        if (*str != ':') {
            lwespi_parse_ip(&str, &esp.m.ipd.ip);   /* Parse incoming packet IP */
            esp.m.ipd.port = lwespi_parse_port(&str);   /* Get port on IPD data */

            LWESP_MEMCPY(&esp.m.conns[conn].remote_ip, &esp.m.ipd.ip, sizeof(esp.m.ipd.ip));
            LWESP_MEMCPY(&esp.m.conns[conn].remote_port, &esp.m.ipd.port, sizeof(esp.m.ipd.port));
        }

    /*
     * Data read procedure may only happen in case there is
     * data packet available, otherwise do nothing further about this information
     *
     * This check should be always positive when LWESP_CFG_CONN_MANUAL_TCP_RECEIVE is disabled
     */
    esp.m.ipd.tot_len = len;                    /* Total number of bytes in this received packet or notification message */
    esp.m.ipd.conn = c;                         /* Pointer to connection we have data for or notification message */
    if (is_data_ipd) {                          /* Shall we start IPD read procedure? */
        esp.m.ipd.read = 1;                     /* Start reading network data */
        esp.m.ipd.rem_len = len;                /* Number of remaining bytes to read */
    }

    return lwespOK;
}

/**
 * \brief           Parse AT and SDK versions from AT+GMR response
 * \param[in]       str: String starting with version numbers
 * \param[out]      version_out: Output variable to save version
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_at_sdk_version(const char* str, lwesp_sw_version_t* version_out) {
    version_out->major = ((uint8_t)lwespi_parse_number(&str));
    ++str;
    version_out->minor = ((uint8_t)lwespi_parse_number(&str));
    ++str;
    version_out->patch = ((uint8_t)lwespi_parse_number(&str));

    return 1;
}

/**
 * \brief           Parse +LINK_CONN received string for new connection active
 * \param[in]       str: Pointer to input string starting with +LINK_CONN
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_link_conn(const char* str) {
    if (str == NULL) {
        return 0;
    }
    if (*str == '+') {
        str += 11;
    }
    esp.m.link_conn.failed = lwespi_parse_number(&str);
    esp.m.link_conn.num = lwespi_parse_number(&str);
    if (!strncmp(str, "\"TCP\"", 5)) {
        esp.m.link_conn.type = LWESP_CONN_TYPE_TCP;
    } else if (!strncmp(str, "\"UDP\"", 5)) {
        esp.m.link_conn.type = LWESP_CONN_TYPE_UDP;
    } else if (!strncmp(str, "\"SSL\"", 5)) {
        esp.m.link_conn.type = LWESP_CONN_TYPE_SSL;
#if LWESP_CFG_IPV6
    } else if (!strncmp(str, "\"TCPv6\"", 7)) {
        esp.m.link_conn.type = LWESP_CONN_TYPE_TCPV6;
    } else if (!strncmp(str, "\"SSLv6\"", 7)) {
        esp.m.link_conn.type = LWESP_CONN_TYPE_SSLV6;
#endif /* LWESP_CFG_IPV6 */
    } else {
        return 0;
    }
    str += 6;
    esp.m.link_conn.is_server = lwespi_parse_number(&str);
    lwespi_parse_ip(&str, &esp.m.link_conn.remote_ip);
    esp.m.link_conn.remote_port = lwespi_parse_port(&str);
    esp.m.link_conn.local_port = lwespi_parse_port(&str);
    return 1;
}

#if LWESP_CFG_MODE_STATION || __DOXYGEN__
/**
 * \brief           Parse received message for list access points
 * \param[in]       str: Pointer to input string starting with +CWLAP
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_cwlap(const char* str, lwesp_msg_t* msg) {
    if (!CMD_IS_DEF(LWESP_CMD_WIFI_CWLAP) ||    /* Do we have valid message here and enough memory to save everything? */
        msg->msg.ap_list.aps == NULL || msg->msg.ap_list.apsi >= msg->msg.ap_list.apsl) {
        return 0;
    }
    if (*str == '+') {                          /* Does string contain '+' as first character */
        str += 7;                               /* Skip this part */
    }
    if (*str != '(') {                          /* We must start with opening bracket */
        return 0;
    }
    ++str;

    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].ecn = (lwesp_ecn_t)lwespi_parse_number(&str);
    lwespi_parse_string(&str, msg->msg.ap_list.aps[msg->msg.ap_list.apsi].ssid, sizeof(msg->msg.ap_list.aps[msg->msg.ap_list.apsi].ssid), 1);
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].rssi = (int16_t)lwespi_parse_number(&str);
    lwespi_parse_mac(&str, &msg->msg.ap_list.aps[msg->msg.ap_list.apsi].mac);
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].ch = (uint8_t)lwespi_parse_number(&str);
#if LWESP_CFG_ACCESS_POINT_STRUCT_FULL_FIELDS
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].scan_type = (uint8_t)lwespi_parse_number(&str);     /* Scan type */
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].scan_time_min = (uint16_t)lwespi_parse_number(&str);/* Scan time minimum */
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].scan_time_max = (uint16_t)lwespi_parse_number(&str);/* Scan time maximum */
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].freq_offset = (int16_t)lwespi_parse_number(&str);   /* Freq offset */
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].freq_cal = (int16_t)lwespi_parse_number(&str);      /* Freqcal value */
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].pairwise_cipher = (lwesp_ap_cipher_t)lwespi_parse_number(&str); /* Pairwise cipher */
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].group_cipher = (lwesp_ap_cipher_t)lwespi_parse_number(&str);    /* Group cipher */
#else
    /* Read and ignore values */
    lwespi_parse_number(&str);
    lwespi_parse_number(&str);
    lwespi_parse_number(&str);
    lwespi_parse_number(&str);
    lwespi_parse_number(&str);
    lwespi_parse_number(&str);
    lwespi_parse_number(&str);
#endif /* !LWESP_CFG_ACCESS_POINT_STRUCT_FULL_FIELDS */
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].bgn = lwespi_parse_number(&str);
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].wps = lwespi_parse_number(&str);

    /* Go to next entry */
    ++msg->msg.ap_list.apsi;                    /* Increase number of found elements */
    if (msg->msg.ap_list.apf != NULL) {         /* Set pointer if necessary */
        *msg->msg.ap_list.apf = msg->msg.ap_list.apsi;
    }
    return 1;
}

/**
 * \brief           Parse received message for current AP information
 * \param[in]       str: Pointer to input string starting with +CWJAP
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_cwjap(const char* str, lwesp_msg_t* msg) {
    if (!CMD_IS_DEF(LWESP_CMD_WIFI_CWJAP_GET)) {/* Do we have valid message here and enough memory to save everything? */
        return 0;
    }
    if (*str == '+') {                          /* Does string contain '+' as first character */
        str += 7;                               /* Skip this part */
    }
    if (*str != '"') {                          /* We must start with quotation mark */
        return 0;
    }
    lwespi_parse_string(&str, esp.msg->msg.sta_info_ap.info->ssid, LWESP_CFG_MAX_SSID_LENGTH, 1);
    lwespi_parse_mac(&str, &esp.msg->msg.sta_info_ap.info->mac);
    esp.msg->msg.sta_info_ap.info->ch = lwespi_parse_number(&str);
    esp.msg->msg.sta_info_ap.info->rssi = lwespi_parse_number(&str);

    LWESP_UNUSED(msg);

    return 1;
}

#endif /* LWESP_CFG_MODE_STATION || __DOXYGEN__ */

#if LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
/**
 * \brief           Parse received message for list stations
 * \param[in]       str: Pointer to input string starting with +CWLAP
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_cwlif(const char* str, lwesp_msg_t* msg) {
    if (!CMD_IS_DEF(LWESP_CMD_WIFI_CWLIF)       /* Do we have valid message here and enough memory to save everything? */
        || msg->msg.sta_list.stas == NULL || msg->msg.sta_list.stai >= msg->msg.sta_list.stal) {
        return 0;
    }

    if (*str == '+') {
        str += 7;
    }

    lwespi_parse_ip(&str, &msg->msg.sta_list.stas[msg->msg.sta_list.stai].ip);
    lwespi_parse_mac(&str, &msg->msg.sta_list.stas[msg->msg.sta_list.stai].mac);

    ++msg->msg.sta_list.stai;                   /* Increase number of found elements */
    if (msg->msg.sta_list.staf != NULL) {       /* Set pointer if necessary */
        *msg->msg.sta_list.staf = msg->msg.sta_list.stai;
    }
    return 1;
}

/**
 * \brief           Parse MAC address and send to user layer
 * \param[in]       str: Input string excluding `+DIST_STA_IP:` part
 * \param[in]       is_conn: Set to `1` if station connected or `0` if station disconnected
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_ap_conn_disconn_sta(const char* str, uint8_t is_conn) {
    lwesp_mac_t mac;

    lwespi_parse_mac(&str, &mac);               /* Parse MAC address */

    esp.evt.evt.ap_conn_disconn_sta.mac = &mac;
    lwespi_send_cb(is_conn ? LWESP_EVT_AP_CONNECTED_STA : LWESP_EVT_AP_DISCONNECTED_STA);   /* Send event function */
    return 1;
}

/**
 * \brief           Parse received string "+DIST_STA_IP" and send notification to user layer
 * \param[in]       str: Input string excluding "+DIST_STA_IP:" part
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_ap_ip_sta(const char* str) {
    lwesp_mac_t mac;
    lwesp_ip_t ip;

    lwespi_parse_mac(&str, &mac);               /* Parse MAC address */
    lwespi_parse_ip(&str, &ip);                 /* Parse IP address */

    esp.evt.evt.ap_ip_sta.mac = &mac;
    esp.evt.evt.ap_ip_sta.ip = &ip;
    lwespi_send_cb(LWESP_EVT_AP_IP_STA);        /* Send event function */
    return 1;
}

/**
 * \brief           Parse received message for current AP information
 * \param[in]       str: Pointer to input string starting with +CWSAP
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_cwsap(const char* str, lwesp_msg_t* msg) {
    if (!CMD_IS_DEF(LWESP_CMD_WIFI_CWSAP_GET)) {/* Do we have valid message here and enough memory to save everything? */
        return 0;
    }
    if (*str == '+') {                          /* Does string contain '+' as first character */
        str += 7;                               /* Skip this part */
    }
    if (*str != '"') {                          /* We must start with quotation mark */
        return 0;
    }
    lwespi_parse_string(&str, esp.msg->msg.ap_conf_get.ap_conf->ssid, LWESP_CFG_MAX_SSID_LENGTH, 1);
    lwespi_parse_string(&str, esp.msg->msg.ap_conf_get.ap_conf->pwd, LWESP_CFG_MAX_PWD_LENGTH, 1);
    esp.msg->msg.ap_conf_get.ap_conf->ch = lwespi_parse_number(&str);
    esp.msg->msg.ap_conf_get.ap_conf->ecn = lwespi_parse_number(&str);
    esp.msg->msg.ap_conf_get.ap_conf->max_cons = lwespi_parse_number(&str);
    esp.msg->msg.ap_conf_get.ap_conf->hidden = lwespi_parse_number(&str);

    LWESP_UNUSED(msg);

    return 1;
}
#endif /* LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */

#if LWESP_CFG_PING || __DOXYGEN__

/**
 * \brief           Parse received time for ping
 * \param[in]       str: Pointer to input string starting with +time
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_ping_time(const char* str, lwesp_msg_t* msg) {
    if (!CMD_IS_DEF(LWESP_CMD_TCPIP_PING)) {
        return 0;
    }
    if (*str == '+') {
        str += 6;
    }
    msg->msg.tcpip_ping.time = lwespi_parse_number(&str);
    if (msg->msg.tcpip_ping.time_out != NULL) {
        *msg->msg.tcpip_ping.time_out = msg->msg.tcpip_ping.time;
    }
    return 1;
}

#endif /* LWESP_CFG_PING || __DOXYGEN__ */

#if LWESP_CFG_DNS || __DOXYGEN__

/**
 * \brief           Parse received message domain DNS name
 * \param[in]       str: Pointer to input string starting with +CWLAP
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_cipdomain(const char* str, lwesp_msg_t* msg) {
    if (!CMD_IS_DEF(LWESP_CMD_TCPIP_CIPDOMAIN)) {
        return 0;
    }
    if (*str == '+') {
        str += 11;
    }
    lwespi_parse_ip(&str, msg->msg.dns_getbyhostname.ip);   /* Parse IP address */
    return 1;
}

#endif /* LWESP_CFG_DNS || __DOXYGEN__ */

#if LWESP_CFG_SNTP || __DOXYGEN__

/**
 * \brief           Parse received message for SNTP time
 * \param[in]       str: Pointer to input string starting with +CWLAP
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_cipsntptime(const char* str, lwesp_msg_t* msg) {
    if (!CMD_IS_DEF(LWESP_CMD_TCPIP_CIPSNTPTIME)) {
        return 0;
    }
    if (*str == '+') {                          /* Check input string */
        str += 13;
    }

    /* Scan for day in a week */
    if (!strncmp(str, "Mon", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 1;
    } else if (!strncmp(str, "Tue", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 2;
    } else if (!strncmp(str, "Wed", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 3;
    } else if (!strncmp(str, "Thu", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 4;
    } else if (!strncmp(str, "Fri", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 5;
    } else if (!strncmp(str, "Sat", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 6;
    } else if (!strncmp(str, "Sun", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 7;
    }
    str += 4;

    /* Scan for month in a year */
    if (!strncmp(str, "Jan", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 1;
    } else if (!strncmp(str, "Feb", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 2;
    } else if (!strncmp(str, "Mar", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 3;
    } else if (!strncmp(str, "Apr", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 4;
    } else if (!strncmp(str, "May", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 5;
    } else if (!strncmp(str, "Jun", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 6;
    } else if (!strncmp(str, "Jul", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 7;
    } else if (!strncmp(str, "Aug", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 8;
    } else if (!strncmp(str, "Sep", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 9;
    } else if (!strncmp(str, "Oct", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 10;
    } else if (!strncmp(str, "Nov", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 11;
    } else if (!strncmp(str, "Dec", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 12;
    }
    str += 4;
    if (*str == ' ') {                          /* Numbers < 10 could have one more space */
        ++str;
    }
    msg->msg.tcpip_sntp_time.dt->date = lwespi_parse_number(&str);
    ++str;
    msg->msg.tcpip_sntp_time.dt->hours = lwespi_parse_number(&str);
    ++str;
    msg->msg.tcpip_sntp_time.dt->minutes = lwespi_parse_number(&str);
    ++str;
    msg->msg.tcpip_sntp_time.dt->seconds = lwespi_parse_number(&str);
    ++str;
    msg->msg.tcpip_sntp_time.dt->year = lwespi_parse_number(&str);
    return 1;
}

#endif /* LWESP_CFG_SNTP || __DOXYGEN__ */

#if LWESP_CFG_HOSTNAME || __DOXYGEN__

/**
 * \brief           Parse received message for HOSTNAME
 * \param[in]       str: Pointer to input string starting with +CWHOSTNAME
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_hostname(const char* str, lwesp_msg_t* msg) {
    size_t i;
    if (!CMD_IS_DEF(LWESP_CMD_WIFI_CWHOSTNAME_GET)) {
        return 0;
    }
    if (*str == '+') {                          /* Check input string */
        str += 12;
    }
    msg->msg.wifi_hostname.hostname_get[0] = 0;
    if (*str != '\r') {
        i = 0;
        for (; i < (msg->msg.wifi_hostname.length - 1) && *str && *str != '\r'; ++i, ++str) {
            msg->msg.wifi_hostname.hostname_get[i] = *str;
        }
        msg->msg.wifi_hostname.hostname_get[i] = 0;
    }
    return 1;
}

#endif /* LWESP_CFG_HOSTNAME || __DOXYGEN__ */

/**
 * \brief           Parse received message for DHCP
 * \param[in]       str: Pointer to input string starting with +CWDHCP
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_parse_cwdhcp(const char* str) {
    uint8_t val;

    if (!CMD_IS_CUR(LWESP_CMD_WIFI_CWDHCP_GET)) {
        return 0;
    }
    if (*str == '+') {
        str += 8;
    }

    val = lwespi_parse_number(&str);

#if LWESP_CFG_MODE_ACCESS_POINT
    esp.m.ap.dhcp = (val & 0x01) == 0x01;
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
#if LWESP_CFG_MODE_STATION
    esp.m.sta.dhcp = (val & 0x02) == 0x02;
#endif /* LWESP_CFG_MODE_STATION */

    return 1;
}

#if LWESP_CFG_WEBSERVER || __DOXYGEN__

/**
 * \brief           Parse +WEBSERVER response from ESP device
 * \param[in]       str: Input string to parse
 * \return          Member of \ref lwespr_t enumeration
 */
uint8_t
lwespi_parse_webserver(const char* str) {

    esp.evt.evt.ws_status.code = lwespi_parse_number(&str);
    lwespi_send_cb(LWESP_EVT_WEBSERVER);        /* Send event function */
    return 1;
}

#endif /* LWESP_CFG_WEBSERVER || __DOXYGEN__ */
