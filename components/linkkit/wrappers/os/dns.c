
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <network/network.h>
#include "linkkit/wrappers/wrappers.h"
#include "dns.h"

/* dns header field */
#define DNS_ID_FIELD                 (0x6666)
#define DNS_CTRL_FIELD               (0x0100)
#define DNS_QUESTION_COUNT_FIELD     (0x0001)
#define DNS_ANSWER_COUNT_FIELD       (0x0000)
#define DNS_AUTHORITY_COUNT_FIELD    (0x0000)
#define DNS_ADDITIONAL_COUNT_FIELD   (0x0000)

/* dns record section */
#define DNS_QTYPE_FIELD              (0x0001)
#define DNS_QCLASS_FIELD             (0x0001)

/* dns server and resolve result */
#define DNS_SERVER_COUNT             (3)

static char g_dns_ip_list[DNS_RESULT_COUNT][16];

static char *g_dns_server_list[DNS_SERVER_COUNT] = {
    "223.5.5.5",
    "223.6.6.6",
    "8.8.8.8"
};

static int dns_domain_check(char *domain) {
    uint32_t idx = 0;

    for (idx = 0;idx < strlen(domain);idx++) {
        if (domain[idx] == '.') {
            continue;
        }
        if (domain[idx] < 0x30 || domain[idx] > 0x39) {
            return 0;
        }
    }

    return -1;
}

static void dns_uint2str(uint8_t *input, char *output)
{
    uint8_t idx = 0, i = 0, j = 0;
    uint8_t pos = 0;
    char temp[10] = {0};

    for (idx = 0;idx < 4;idx++) {
        i = 0;
        j = 0;
        pos = input[idx];
        memset(temp, 0, 10);
        do {
            temp[i++] = pos % 10 + '0';
        } while ((pos /= 10) > 0);

        do {
            output[--i + strlen(output)] = temp[j++];
        } while (i > 0);
        output[strlen(output)] = '.';
    }
    output[strlen(output) - 1] = 0x00;
}

void dns_request_message(char *domain, uint8_t buffer[1024], uint32_t *index)
{
    uint32_t idx = 0;

    /* identification */
    buffer[idx++] = (DNS_ID_FIELD >> 8) & 0x00FF;
    buffer[idx++] = (DNS_ID_FIELD) & 0x00FF;

    /* control */
    buffer[idx++] = (DNS_CTRL_FIELD >> 8) & 0x00FF;
    buffer[idx++] = (DNS_CTRL_FIELD) & 0x00FF;

    /* question count */
    buffer[idx++] = (DNS_QUESTION_COUNT_FIELD >> 8) & 0x00FF;
    buffer[idx++] = (DNS_QUESTION_COUNT_FIELD) & 0x00FF;

    /* answer count */
    buffer[idx++] = (DNS_ANSWER_COUNT_FIELD >> 8) & 0x00FF;
    buffer[idx++] = (DNS_ANSWER_COUNT_FIELD) & 0x00FF;

    /* authority count */
    buffer[idx++] = (DNS_AUTHORITY_COUNT_FIELD >> 8) & 0x00FF;
    buffer[idx++] = (DNS_AUTHORITY_COUNT_FIELD) & 0x00FF;

    /* additional count */
    buffer[idx++] = (DNS_ADDITIONAL_COUNT_FIELD >> 8) & 0x00FF;
    buffer[idx++] = (DNS_ADDITIONAL_COUNT_FIELD) & 0x00FF;

    /* qname */
    {
        uint32_t section_start = 0, qname_idx = 0;
        do {
            if (domain[qname_idx] == '.' || qname_idx == strlen(domain)) {
                buffer[idx++] = (uint32_t)(qname_idx - section_start);
                memcpy(&buffer[idx], &domain[section_start], (qname_idx - section_start));
                idx += (qname_idx - section_start);
                section_start = qname_idx + 1;
            }

            if (qname_idx == strlen(domain)) {
                break;
            }

            qname_idx++;
        } while(1);
        buffer[idx++] = 0x00;
    }

    /* qtype */
    buffer[idx++] = (DNS_QTYPE_FIELD >> 8) & 0x00FF;
    buffer[idx++] = (DNS_QTYPE_FIELD) & 0x00FF;

    /* qclass */
    buffer[idx++] = (DNS_QCLASS_FIELD >> 8) & 0x00FF;
    buffer[idx++] = (DNS_QCLASS_FIELD) & 0x00FF;

    *index = idx;
}

int dns_response_message(uint8_t buffer[1024], uint32_t buffer_len, char *ip[DNS_RESULT_COUNT])
{
    int res = 0;
    uint32_t idx = 0, rd_len = 0, dns_count = 0;

    /* skip dns header */
    idx += 12;

    /* skip question qname section */
    while(buffer[idx] != 0x00) {
        idx += buffer[idx] + 1;
    }

    if (idx >= buffer_len) {
        return -1;
    }

    /* skip 0x00, question qtype and qclass section */
    idx += 1 + 4;

    memset(g_dns_ip_list, 0, DNS_RESULT_COUNT * 16);

    while (idx < buffer_len) {
        /* name, type, class and TTL */
        idx += 2 + 2 + 2 + 4;

        rd_len = (buffer[idx++] << 8);
        rd_len |= buffer[idx++];

        if (idx >= buffer_len) {
            return -1;
        }

        if (rd_len == 4) {
            if (dns_count < DNS_RESULT_COUNT) {
                dns_uint2str(&buffer[idx], g_dns_ip_list[dns_count]);
                ip[dns_count] = g_dns_ip_list[dns_count];
                dns_count++;
            }else{
                break;
            }
        }

        idx += rd_len;
    }

    if (dns_count == 0) {
        return -1;
    }    

    return 0;
}

int dns_resolve(char dns[16], char *domain, char *ip[DNS_RESULT_COUNT])
{
    int res = 0, sock_fd = 0;
    struct sockaddr_in dest;
    uint32_t dest_len = 0;
    uint8_t send_message[1024] = {0}, recv_message[1024] = {0};
    uint32_t idx = 0;
    fd_set send_recv_sets;
    struct timeval timeselect;

    res = dns_domain_check(domain);
    if (res < 0) {
        if (strlen(domain) >= 16) {
            return -1;
        }
        memset(g_dns_ip_list, 0, DNS_RESULT_COUNT * 16);
        memcpy(g_dns_ip_list[0], domain, strlen(domain));
        ip[0] = g_dns_ip_list[0];
        return 0;
    }

    sock_fd = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);
    if (sock_fd < 0) {
        HAL_Printf("dns socket: \r\n");
        return -1;
    }

    dest.sin_family = AF_INET;
	dest.sin_port = htons(53);
	dest.sin_addr.s_addr = inet_addr(dns);

    /* set select timeout */
    timeselect.tv_sec = 3;
    timeselect.tv_usec = 0;

    /* dns request message */
    dns_request_message(domain, send_message, &idx);

    /* send to dns server */
    FD_ZERO(&send_recv_sets);
    FD_SET(sock_fd, &send_recv_sets);
    res = select(sock_fd + 1, NULL, &send_recv_sets, NULL, &timeselect);
    if (res <= 0) {
        close(sock_fd);
        return -1;
    }
    if (FD_ISSET(sock_fd, &send_recv_sets)) {
        if ((res = sendto(sock_fd, (void *)send_message, (size_t)idx, 0, (struct sockaddr*)&dest, sizeof(dest))) < 0) {
            HAL_Printf("send dns request message failed.\r\n");
            close(sock_fd);
            return -1;
        }

        if (res != idx) {
            close(sock_fd);
            return -1;
        }
    }

    /* recv from dns server */
    FD_ZERO(&send_recv_sets);
    FD_SET(sock_fd, &send_recv_sets);
    res = select(sock_fd + 1, &send_recv_sets, NULL, NULL, &timeselect);
    if (res <= 0) {
        close(sock_fd);
        return -1;
    }
    if (FD_ISSET(sock_fd, &send_recv_sets)) {
        dest_len = sizeof(dest);
        if ((res = recvfrom(sock_fd, (void *)recv_message, 1024, 0, (struct sockaddr*)&dest, &dest_len)) < 0) {
            HAL_Printf("send dns request message failed;\r\n");
            close(sock_fd);
            return -1;
        }
    }

    close(sock_fd);

    return dns_response_message(recv_message, res, ip);
}

int dns_getaddrinfo(char *domain, char *ip[DNS_RESULT_COUNT])
{
    int res = 0;
    uint8_t idx = 0;

    memset(g_dns_ip_list, 0, DNS_RESULT_COUNT * 16);
    for (idx = 0;idx < DNS_SERVER_COUNT;idx++) {
        HAL_Printf("[prt] dns server: %s\n", g_dns_server_list[idx]);
        res = dns_resolve(g_dns_server_list[idx], domain, ip);
        if (res < 0) {
            continue;
        }    
        return 0;
    }
    return -1;
}
