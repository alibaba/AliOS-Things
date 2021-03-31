#include <das/service.h>
#ifdef DAS_PLATFORM_LINUX
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#if (DAS_SERVICE_NETFILER_ENABLED)

// https://gist.github.com/fffaraz/9d9170b57791c28ccda9255b48315168
// https://github.com/pflarr/dns_parse

#define DNS_PREFIX  "dns:"

#define T_A 1 //Ipv4 address
#define T_A6 28 // ipv6
#define T_NS 2 //Nameserver
#define T_CNAME 5 // canonical name
#define T_SOA 6 /* start of authority zone */
#define T_PTR 12 /* domain name pointer */
#define T_MX 15 //Mail server
 
 struct dns_ans_t
{
  uint16_t _nm;
  uint16_t _type;
  uint16_t _cls;
  uint16_t _ttl1; // if using uint32, compiler will pad struct.
  uint16_t _ttl2;
  uint16_t _datalen;
};

#define DNS_ANS_TYPE_CNAME 5
#define DNS_ANS_TYPE_A 1     // ipv4 address
#define DNS_ANS_TYPE_AAAA 28 // ipv6
#define DNS_ANS_CLASS_IN 1

// Reads a uint16_t and byte-swaps ntohs()
#define U16S(_PAYLOAD, _INDEX) \
  ((((uint8_t *)(_PAYLOAD))[_INDEX] << 8) + ((uint8_t *)(_PAYLOAD))[_INDEX + 1])

//DNS header structure
struct DNS_HEADER
{
    unsigned short id; // identification number
    /*

    unsigned char rd :1; // recursion desired
    unsigned char tc :1; // truncated message
    unsigned char aa :1; // authoritive answer
    unsigned char opcode :4; // purpose of message
    unsigned char qr :1; // query/response flag
 
    unsigned char rcode :4; // response code
    unsigned char cd :1; // checking disabled
    unsigned char ad :1; // authenticated data
    unsigned char z :1; // its z! reserved
    unsigned char ra :1; // recursion available
    */
    unsigned short flags;
    unsigned short q_count; // number of question entries
    unsigned short ans_count; // number of answer entries
    unsigned short auth_count; // number of authority entries
    unsigned short add_count; // number of resource entries
};
 
//Constant sized fields of query structure
struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};
 
//Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct R_DATA
{
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)
 
//Pointers to resource record contents
struct RES_RECORD
{
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};

//Structure of a Query
typedef struct
{
    unsigned char *name;
    struct QUESTION *ques;
} QUERY;

#define MAX_DOMAIN_LEN      128
#define MAX_IPS     20

struct ANS_RECORD
{
    char domain[MAX_DOMAIN_LEN];
    unsigned int ips[MAX_IPS];
};

#define DNS_FLAG_OPCODE(FLGS)   ((FLGS >> 11) & 0x0F)
#define DNS_FLAG_RESPONSE       0x8000

struct ANS_RECORD *parse_dns_package(void *package, int len);

static int skip_name(char *ptr, int remaining)
{
    char *p = ptr;
    char *end = p + remaining;
    while (p < end) {
        int dotLen = *p;
        if ((dotLen & 0xc0) == 0xc0) {
            // printf("skip_name not linear!\n");
        }
        if (dotLen < 0 || dotLen >= remaining)
            return -1;
        if (dotLen == 0)
            return (int)(p - ptr + 1);
        p += dotLen + 1;
        remaining -= dotLen + 1;
    }
    return -1;
}

static int dnsReadQueries(char *payload, 
                        int payloadLen, 
                        char *ptr, 
                        int remaining, 
                        int numQueries)
{
    (void)payload;
    (void)payloadLen;

    int rem = remaining;
    char *p = ptr;
    while (numQueries > 0) {
        int nameLen = skip_name(p, remaining);
        if (nameLen <= 0)
            return -1;
        remaining -= nameLen + 4;
        p += nameLen + 4;
        if (remaining < 0)
            return -1;
        numQueries--;
    }
    return (rem - remaining);
}

static int dnsReadName(char *name_buf,
                       int name_buf_len,
                       uint16_t nameOffset,
                       char *payload,
                       int payloadLen)
{
    if (name_buf == NULL || name_buf_len == 0 || 
        payload == NULL || nameOffset == 0 || 
        nameOffset >= payloadLen) {
        return -1;
    }

    char tmp[MAX_DOMAIN_LEN] = {0};
    char *dest = tmp;

    char *pstart = payload + nameOffset;
    char *p = pstart;
    char *end = payload + payloadLen;

    while (p < end) {
        uint16_t dotLen = *p;
        if ((dotLen & 0xc0) == 0xc0) {
            if (p > pstart) {
                if ((int)(p - pstart - 1) < name_buf_len) {
                    strncpy(name_buf, tmp, (int)(p - pstart - 1));
                } else {
                    return -1;
                }
            }

            p++;
            int subOff = (uint8_t)*p;
            char name_tmp[MAX_DOMAIN_LEN] = {0};
            if (dnsReadName(name_tmp, name_buf_len - strlen(name_buf) - 1,
                            subOff, payload, payloadLen) == 0) {
                strcat(name_buf, name_tmp);
            }

            return 0;
        }

        if (dotLen < 0 || ((p + dotLen) >= end))
            return -1;
        if (dotLen == 0) {
            if (p > pstart) {
                if ((int)(p - pstart - 1) < name_buf_len) {
                    strncpy(name_buf, tmp, (int)(p - pstart - 1));
                } else {
                    return -1;
                }
            }

            return 0;
        }

        // if we get here, dotLen > 0

        // sanity check on max length of temporary buffer
        if ((dest + dotLen + 1) >= (tmp + sizeof(tmp))) {
            return -1;
        }

        if (dest != tmp) {
            *dest++ = '.';
        }
        p++;
        memcpy(dest, p, dotLen);
        p += dotLen;
        dest += dotLen;
    }

    return -1;
}

static int dnsReadAnswers(char *payload,
                          int payloadLen,
                          char *ptr,
                          int remaining,
                          int numAnswers,
                          struct ANS_RECORD *ans_record)
{
    bool is_frist_name = true;
    int len = 0;
    int cnt_ip = 0;
    int ret = -1;

    if (payload == NULL || payloadLen == 0 || ptr == NULL ||
        remaining == 0 || numAnswers == 0 || ans_record == NULL) {
        return -1;
    }

    while (numAnswers > 0 && cnt_ip < MAX_IPS) {
        struct dns_ans_t ans;

        if ((remaining - len) <= (int)sizeof(ans)) {
            return -1;
        }

        char *p = ptr + len;

        ans._nm = U16S(p, 0);
        ans._type = U16S(p, 2);
        ans._cls = U16S(p, 4);
        ans._datalen = U16S(p, 10);

        int nameOffset = ans._nm & 0x3fff;

        char name_tmp[MAX_DOMAIN_LEN] = {0};
        if (dnsReadName(name_tmp, MAX_DOMAIN_LEN, nameOffset, payload, payloadLen) == -1) {
            return -1;
        }

        // check datalen bounds
        if ((remaining - len - sizeof(ans)) < ans._datalen) {
            return -1;
        }

        if (is_frist_name == true)
        {
            strncpy(ans_record->domain, name_tmp, MAX_DOMAIN_LEN - 1);
            is_frist_name = false;
        }

        // read data section
        switch (ans._type) {
        case DNS_ANS_TYPE_CNAME:
            break;
        case DNS_ANS_TYPE_A: {
            uint32_t ip = *((uint32_t *)(p + sizeof(ans)));
            ans_record->ips[cnt_ip++] = ip;
            ret = 0;
            break;
        }
        case DNS_ANS_TYPE_AAAA: // ipv6
            break;
        default:
            break;
        }

        len += sizeof(ans) + ans._datalen;
        numAnswers--;
    }

    return ret;
}

struct ANS_RECORD *parse_dns_package(void *package, int len)
{
    struct DNS_HEADER *dns_header = NULL;
    __uint16_t q_cnt, ans_cnt, auth_cnt, add_cnt, flags;
    char *buf = NULL;
    int recordOffset = 0;

    if (package == NULL || (size_t)len <= sizeof(struct DNS_HEADER))
        return NULL;

    buf = (char *)package;
    dns_header = (struct DNS_HEADER *)buf;

    flags = ntohs(dns_header->flags);
    q_cnt = ntohs(dns_header->q_count);
    ans_cnt = ntohs(dns_header->ans_count);
    auth_cnt = ntohs(dns_header->auth_count);
    add_cnt = ntohs(dns_header->add_count);

    if (DNS_FLAG_OPCODE(flags) != 0) {
        return NULL;
    }

    if ((flags & DNS_FLAG_RESPONSE) == 0) {
        return NULL;
    }
    
    if (q_cnt > 4 ||
        ans_cnt == 0 || ans_cnt > 20 ||
        auth_cnt >= 100 || add_cnt >= 100) {
        return NULL;
    }

    recordOffset = sizeof(struct DNS_HEADER);

    if (q_cnt > 0) {
        int size = dnsReadQueries(buf,
                                  len,
                                  buf + recordOffset,
                                  len - recordOffset,
                                  q_cnt);
        if (size < 0)
            return NULL;

        recordOffset += size;
        if ((len - recordOffset) <= 0)
            return NULL;
    }

    if (ans_cnt > 0) {
        struct ANS_RECORD *ans_record = (struct ANS_RECORD *)malloc(sizeof(struct ANS_RECORD));
        if (ans_record == NULL)
            return NULL;

        if (-1 == dnsReadAnswers(buf,
                                 len,
                                 buf + recordOffset,
                                 len - recordOffset,
                                 ans_cnt,
                                 ans_record)) {
            free(ans_record);
            ans_record = NULL;
        }

        return ans_record;
    }

    return NULL;
}

static int _dns_parse_init (void) {
    return 0;
}

static void format_dns_info(struct ANS_RECORD *record, char *buf, size_t buf_size) {
    if (record && buf && buf_size > 0) {
        int used = 0;
        for(int j = 0; j < MAX_IPS && record->ips[j]; j++) {
            if (j == 0) {
                if (buf_size <= strlen(record->domain) + strlen(DNS_PREFIX)) {
                    break;
                }

                strncat(buf, DNS_PREFIX, buf_size - 1 - used);
                used += strlen(DNS_PREFIX);
                strncat(buf, record->domain, buf_size - 1 - used);
                used += strlen(record->domain);
            }

            if (buf_size - used <= INET_ADDRSTRLEN + 1) {
                break;
            }
            
            char ip_str[INET_ADDRSTRLEN] = { 0 };
            struct in_addr ip = { .s_addr = record->ips[j] };
            if (!inet_ntop(AF_INET, &ip, ip_str, sizeof(ip_str))) {
                break;
            }

            strncat(buf, "|", buf_size - 1 - used);
            used += strlen("|");
            strncat(buf, ip_str, buf_size - 1 - used);
            used += strlen(ip_str);
        }
    }
}

static pre_process_status_t _dns_parse_processor (netflow_info_t *info, 
                                        void *payload, 
                                        size_t payload_size, 
                                        char *buf, 
                                        size_t buf_size)
{   
    pre_process_status_t ret = PRE_PROCESS_CALL_NEXT;
    int off = 0, remain = payload_size;
    struct ANS_RECORD *record = NULL;

    if (info == NULL || payload == NULL || payload_size == 0) {
        ret = PRE_PROCESS_GENERAL_ERROR;
        return ret;
    }

    if (payload_size <= sizeof(uint32_t)) {
        ret = PRE_PROCESS_CALL_NEXT;
        return ret;
    }

    if (ntohs(info->src_port) != 53 && ntohs(info->dest_port) != 53) {
        ret = PRE_PROCESS_CALL_NEXT;
        return ret;
    }

    if (info->protocol != IPPROTO_TCP && info->protocol != IPPROTO_UDP) {
        ret = PRE_PROCESS_CALL_NEXT;
        return ret;
    }
    
    switch(info->protocol) {
    case IPPROTO_TCP: {
        struct tcphdr *tcph = (struct tcphdr *) (payload);
        /* No room for IPv4 header plus TCP header. */
        if (remain < sizeof(struct tcphdr) || remain < (tcph->doff << 2)) {
            DAS_LOG("no room for IPv4 + TCP header, skip.\n");
            break;
        }

        off += (tcph->doff << 2);
        remain -= (tcph->doff << 2);

        if(remain > 0) {
            record = parse_dns_package((char *)payload + off, remain);
            if (record) {
                format_dns_info(record, buf, buf_size);
                ret = PRE_PROCESS_REPORT_CUSTOM;
            }
        }
        break;
	}
    case IPPROTO_UDP: {
        // struct udphdr *udph = (struct udphdr *) (payload);
        /* No room for IPv4 header plus UDP header. */
        if (remain < sizeof(struct udphdr)) {
            DAS_LOG("no room for IPv4 + UDP header, skip\n");
            break;
        }

        off += sizeof(struct udphdr);
        remain -= sizeof(struct udphdr);

        if(remain > 0) {
            record = parse_dns_package((char *)payload + off, remain);
            if (record) {
                format_dns_info(record, buf, buf_size);
                ret = PRE_PROCESS_REPORT_CUSTOM;
            }
        }
        break;
	}
    default:
        break;
    }

    return ret;
}

netfilter_pre_processor_t dns_parser_pre_processor = {
    .name = "DNS_PARSE_PROCESSOR",
    .init = _dns_parse_init,
    .pre_process = _dns_parse_processor,
};

#endif
#endif