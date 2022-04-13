/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <das/platform.h>
#include <das/service.h>
#include <das/configure.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <linux/filter.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <das/queue.h>

#if (DAS_SERVICE_NETFILER_ENABLED)

#define FILTE_DEVICE_ANY        "any"
#define NFI_INFO_KEY_NETFLOW 	"netflow:"

#define MAX_CUSTOM_INFO_SIZE    1024
#define MAX_INFO_COUNT          50

#define DIRECTION_IN            1
#define DIRECTION_OUT           2

const char *const prototo2str[IPPROTO_MAX] = {
    [IPPROTO_IP]			= "ip",
	[IPPROTO_TCP]			= "tcp",
	[IPPROTO_UDP]			= "udp",
	[IPPROTO_UDPLITE]		= "udplite",
	[IPPROTO_ICMP]			= "icmp",
	[IPPROTO_ICMPV6]		= "icmpv6",
	[IPPROTO_SCTP]			= "sctp",
	[IPPROTO_GRE]			= "gre",
	[IPPROTO_DCCP]			= "dccp",
    [IPPROTO_IGMP]          = "igmp",
};

typedef struct _netflow_info_entry {
    netflow_info_t info;
    TAILQ_ENTRY_NEW(_netflow_info_entry) link;
} netflow_info_entry_t;

typedef struct _custom_info_entry {
    char buf[MAX_CUSTOM_INFO_SIZE];
    uint32_t ret_size;
    TAILQ_ENTRY_NEW(_custom_info_entry) link;
} custom_info_entry_t;

typedef TAILQ_HEAD_NEW(netflow_info_entry_t, _netflow_info_entry) netflow_queue_t;
typedef TAILQ_HEAD_NEW(custom_info_entry_t, _custom_info_entry) custom_info_queue_t;

static netflow_queue_t netflow_queue = TAILQ_HEAD_INITIALIZER(netflow_queue);
static custom_info_queue_t custom_info_queue = TAILQ_HEAD_INITIALIZER(custom_info_queue);

static pthread_mutex_t netfilter_mutex;

static uint32_t netflow_info_cnt = 0;
static uint32_t custom_info_cnt = 0;

#if (DAS_SERVICE_NETFILER_USE_AFPACKET)
static int afpacket_socket = -1;

// instruction format:
// +----------------+--------+--------+
// |   16 bits      | 8 bits | 8 bits |
// | operation code |   jt   |   jf   |
// +----------------+--------+--------+
// | (MSB)         k:32         (LSB) |
// +----------------------------------+
static struct sock_filter afpacket_filter_code[] = {
    { 0x6, 0, 0, 0x00040000 },
};
#endif /* DAS_SERVICE_NETFILER_USE_AFPACKET */

static int _is_report_queue_full(int cnt) {
    int ret = 0;
    int lock = 1;
    
    if (pthread_mutex_lock(&netfilter_mutex) != 0) {
        lock = 0;
    }

    if (cnt >=MAX_INFO_COUNT) {
        ret = 1;
    }

    if (lock) {
        pthread_mutex_unlock(&netfilter_mutex);
    }

    return ret;
}

static int _is_netflow_already_have(netflow_info_entry_t *nf) {
    int ret = 0;
    int lock = 1;
    netflow_info_entry_t *nf_tmp, *nf_next_tmp;

     if (nf == NULL) {
        return ret;
    }

    if (pthread_mutex_lock(&netfilter_mutex) != 0) {
        lock = 0;
    }

    TAILQ_FOREACH_SAFE(nf_tmp, &netflow_queue, link, nf_next_tmp) {
        if (memcmp(&(nf_tmp->info), &(nf->info), sizeof(netflow_info_t)) == 0) {
            ret = 1;
            break;
        }

        if (nf_tmp->info.domain == nf->info.domain &&
            nf_tmp->info.protocol == nf->info.protocol &&
            nf_tmp->info.src_port == nf->info.dest_port && 
            nf_tmp->info.dest_port == nf->info.src_port) {
            if (nf->info.domain == AF_INET && 
                0 == memcmp(&nf_tmp->info.dest_ip.v4, &nf->info.src_ip.v4, sizeof(nf->info.src_ip.v4)) &&
                0 == memcmp(&nf_tmp->info.src_ip.v4, &nf->info.dest_ip.v4, sizeof(nf->info.dest_ip.v4))) {
                ret = 1;
                break;
            } else if (nf->info.domain == AF_INET6 && 
                0 == memcmp(&nf_tmp->info.dest_ip.v6, &nf->info.src_ip.v6, sizeof(nf->info.src_ip.v6)) &&
                0 == memcmp(&nf_tmp->info.src_ip.v6, &nf->info.dest_ip.v6, sizeof(nf->info.dest_ip.v6))) {
                ret = 1;
                break;
            }
        }
    }

    if (lock) {
        pthread_mutex_unlock(&netfilter_mutex);
    }

    return ret;
}

static int _insert_netflow_to_queue(netflow_info_entry_t *nf) {
    int ret = -1;
    int lock = 1;

    if (nf == NULL) {
        return ret;
    }

	if (pthread_mutex_lock(&netfilter_mutex) != 0) {
        lock = 0;
    }

    TAILQ_INSERT_TAIL(&netflow_queue, nf, link);
    netflow_info_cnt++;
    ret= 0;

// #ifdef DAS_DEBUG    
#if 0
    char buf[1024] = {0};
    char src_ipv4[INET_ADDRSTRLEN] = {0};
    char dest_ipv4[INET_ADDRSTRLEN] = {0};
    char src_ipv6[INET6_ADDRSTRLEN] = {0};
    char dest_ipv6[INET6_ADDRSTRLEN] = {0};
    char *src_buf = NULL, *dest_buf = NULL;

    if (nf->info.domain == AF_INET) {
        inet_ntop(AF_INET, &nf->info.src_ip.v4, src_ipv4, sizeof(src_ipv4));
        inet_ntop(AF_INET, &nf->info.dest_ip.v4, dest_ipv4, sizeof(dest_ipv4));
        src_buf = src_ipv4;
        dest_buf = dest_ipv4;
    } else {
        inet_ntop(AF_INET6, &nf->info.src_ip.v6, src_ipv6, sizeof(src_ipv6));
        inet_ntop(AF_INET6, &nf->info.dest_ip.v6, dest_ipv6, sizeof(dest_ipv6));
        src_buf = src_ipv6;
        dest_buf = dest_ipv6;
    }

    snprintf(buf, 1024, "%s|%u|%s|%u|%u|%s", 
            src_buf, ntohs(nf->info.src_port),
            dest_buf, ntohs(nf->info.dest_port),
            nf->info.direction,
            prototo2str[nf->info.protocol] ? prototo2str[nf->info.protocol] : "unkonw");
    DAS_LOG("insert %s\n", buf);
#endif

    if (lock) {
        pthread_mutex_unlock(&netfilter_mutex);
    }

    return ret;
}

static int _is_custom_info_already_have(const char *info, size_t len) {
    int ret = 0, lock = 1;
    custom_info_entry_t *ci_tmp = NULL, *ci_tmp_next = NULL;

    if (info == NULL || len == 0) {
        return ret;
    }

    if (pthread_mutex_lock(&netfilter_mutex) != 0) {
        lock = 0;
    }
    
    TAILQ_FOREACH_SAFE(ci_tmp, &custom_info_queue, link, ci_tmp_next) {
        if (len == ci_tmp->ret_size && strncmp(info, ci_tmp->buf, len) == 0) {
            ret = 1;
            break;
        }
    }

    if (lock) {
        pthread_mutex_unlock(&netfilter_mutex);
    }

    return ret;
}

static pre_process_status_t _pre_process(netflow_info_t *info, void *payload, size_t payload_size) 
{
    int i = 0;
    pre_process_status_t status = PRE_PROCESS_GENERAL_ERROR;
    
    if (_is_report_queue_full(custom_info_cnt) == 1) {
        status = PRE_PROCESS_REPORT_Q_FULL;
        return status;
    }

    for (i = 0; i < DAS_MAX_PREPROCESSOR_NUMBER; i++) {
        netfilter_pre_processor_t *preprocessor = NULL;
        int init_ret = 0;

        preprocessor = netfilter_pre_processor_table[i];
        if (preprocessor == NULL) {
            break;
        }

        if (!preprocessor->name) {
            DAS_LOG("invalid preprocessor %d\n", i);
            break;
        }

        if (preprocessor->init) {
            init_ret = preprocessor->init();
        }

        if (init_ret == 0 && preprocessor->pre_process) {
            char buf[MAX_CUSTOM_INFO_SIZE] = {0};
            status = preprocessor->pre_process(info, payload, payload_size, buf, MAX_CUSTOM_INFO_SIZE);
            if (status == PRE_PROCESS_REPORT_IGNORE) {
                DAS_LOG("preprocessor %s return ignored.\n", preprocessor->name);
                break;
            } else if (status == PRE_PROCESS_REPORT_CUSTOM && strlen(buf) > 0) {
                size_t len = strlen(buf) >= MAX_CUSTOM_INFO_SIZE ? MAX_CUSTOM_INFO_SIZE - 1 : strlen(buf);
                if (_is_custom_info_already_have(buf, len) == 0) {
                    custom_info_entry_t *cf = (custom_info_entry_t *)malloc(sizeof(custom_info_entry_t));
                    if (cf != NULL) {
                        int lock = 1;
                        
                        memset(cf, 0, sizeof(custom_info_entry_t));
                        strncpy(cf->buf, buf, len);
                        cf->ret_size = len;

                        if (pthread_mutex_lock(&netfilter_mutex) != 0) {
                            lock = 0;
                        }
                    
                        TAILQ_INSERT_TAIL(&custom_info_queue, cf, link);
                        custom_info_cnt++;
                        DAS_LOG("insert custom info to queue: %s, length: %zu.\n", buf, strlen(buf));

                        if (lock) {
                            pthread_mutex_unlock(&netfilter_mutex);
                        }
                    }
                }
            }
        }
    }

    return status;
}

// netflow:local_ip|local_port|remote_ip|remote_port|direction|protocol
// netflow:srcip|srcport|destip|destport|direction(2)|protocol
static das_result_t _fill_netflow_info(char *buf, size_t size, netflow_info_entry_t *nf) {
    size_t len = 0, off = 0, remain = size;
    char ipv4[INET_ADDRSTRLEN] = {0};
    char ipv6[INET6_ADDRSTRLEN] = {0};
    char *ip = NULL;

    if (buf == NULL || size == 0 || nf == NULL) {
        return DAS_ERROR_BAD_PARAMETERS;
    }

    memset(buf, 0, size * sizeof(char));

    // prefix: "netflow:"
    len = strlen(NFI_INFO_KEY_NETFLOW);
    if (len >= size) {
        DAS_LOG("No buffer size for netflow info: %lx\n", (long)size);
        return DAS_ERROR_SHORT_BUFFER;
    }

    memcpy(buf + off, NFI_INFO_KEY_NETFLOW, len * sizeof(char));
    off += len;
    remain -= len;
    if (remain <= 0) {
        return DAS_ERROR_SHORT_BUFFER;
    }

    // srcIP|srcPort|
    if (nf->info.domain == AF_INET) {
        inet_ntop(AF_INET, &nf->info.src_ip.v4, ipv4, sizeof(ipv4));
        ip = ipv4;
    } else {
        inet_ntop(AF_INET6, &nf->info.src_ip.v6, ipv6, sizeof(ipv6));
        ip = ipv6;
    }
    len = snprintf(buf + off, remain, "%s|%u|", ip, ntohs(nf->info.src_port));
    off += len;
    remain -= len;
    if (remain <= 0) {
        return DAS_ERROR_SHORT_BUFFER;
    }

    // destIP|destPort|
    if (nf->info.domain == AF_INET) {
        inet_ntop(AF_INET, &nf->info.dest_ip.v4, ipv4, sizeof(ipv4));
        ip = ipv4;
    } else {
        inet_ntop(AF_INET6, &nf->info.dest_ip.v6, ipv6, sizeof(ipv6));
        ip = ipv6;
    }
    len = snprintf(buf + off, remain, "%s|%u|", ip, ntohs(nf->info.dest_port));
    off += len;
    remain -= len;
    if (remain <= 0) {
        return DAS_ERROR_SHORT_BUFFER;
    }

    // direction|
    len = snprintf(buf + off, remain, "%u|", nf->info.direction);
    off += len;
    remain -= len;
    if (remain <= 0) {
        return DAS_ERROR_SHORT_BUFFER;
    }

    // protocol
    const char *proto = prototo2str[nf->info.protocol] == NULL ? "unknow": prototo2str[nf->info.protocol];
    len = snprintf(buf + off, remain, "%s", proto);
    off += len;
    remain -= len;
    if (remain <= 0) {
        return DAS_ERROR_SHORT_BUFFER;
    }

    return DAS_SUCCESS;
}

#if (DAS_SERVICE_NETFILER_USE_NFC)
static uint16_t _nfc_get_protocol(const struct nf_conntrack *ct) 
{
    uint16_t protocol = 0;
    if (ct == NULL) {
        return protocol;
    }

    protocol = (uint16_t)(ct->head.orig.protonum);
    return protocol;
}

static int _nfc_get_address_v4(const struct nf_conntrack *ct, struct in_addr *src_ip, struct in_addr *dest_ip) 
{
    int ret = -1;
    
    if ((ct == NULL) || (src_ip == NULL && dest_ip == NULL)) {
        return ret;
    }

    if (src_ip) {
        memcpy(src_ip, &(ct->head.orig.src.v4), sizeof(uint32_t));
    }
    
    if (dest_ip) {
        memcpy(dest_ip, &(ct->head.orig.dst.v4), sizeof(uint32_t));
    }

    ret = 0;
    return ret;
}

static int _nfc_get_address_v6(const struct nf_conntrack *ct, struct in6_addr *src_ip, struct in6_addr *dest_ip) 
{
    int ret = -1;
    
    if ((ct == NULL) || (src_ip == NULL && dest_ip == NULL)) {
        return ret;
    }

    if (src_ip) {
        memcpy(src_ip, &(ct->head.orig.src.v6), sizeof(struct in6_addr));
	
    }
    
    if (dest_ip) {
        memcpy(dest_ip, &(ct->head.orig.dst.v6), sizeof(struct in6_addr));
    }

    ret = 0;
    return ret;
}


static int _nfc_get_port(const struct nf_conntrack *ct, uint16_t *src_port, uint16_t *dest_port) 
{
    int ret = -1;
    
    if ((ct == NULL) || (src_port == NULL && dest_port == NULL)) {
        return ret;
    }

    if (src_port) {
        *src_port = ct->head.orig.l4src.tcp.port;
    }
    
    if (dest_port) {
        *dest_port = ct->head.orig.l4dst.tcp.port;
    }

    ret = 0;
    return ret;
}

// IN = 1; OUT = 2
static uint16_t _nfc_get_direction(const struct nf_conntrack *ct) 
{
    uint16_t ret = DIRECTION_OUT;
    
    if (ct == NULL) {
        return ret;
    }

    return ret;
}

static int _nfc_cb(const struct nlmsghdr *nlh, void *data_arg)
{
	struct nf_conntrack *ct;
	uint32_t type = NFCT_T_UNKNOWN;

	switch(nlh->nlmsg_type & 0xFF) {
	case IPCTNL_MSG_CT_NEW:
		if (nlh->nlmsg_flags & (NLM_F_CREATE|NLM_F_EXCL))
			type = NFCT_T_NEW;
		else
			type = NFCT_T_UPDATE;
		break;
	case IPCTNL_MSG_CT_DELETE:
		type = NFCT_T_DESTROY;
		break;
	}

	ct = nfct_new();
	if (ct == NULL) {
        DAS_LOG("nfct_new error.\n");
        return MNL_CB_OK;
    }
	
	nfct_nlmsg_parse(nlh, ct);

    // support ipv4 & ipv6.
    if (ct->head.orig.l3protonum == AF_INET || ct->head.orig.l3protonum == AF_INET6) {
        netflow_info_entry_t *nf = (netflow_info_entry_t *)malloc(sizeof(netflow_info_entry_t));
        if (nf) {
            memset(nf, 0, sizeof(netflow_info_entry_t));

            nf->info.domain = ct->head.orig.l3protonum;
            nf->info.protocol = _nfc_get_protocol(ct);
            if (nf->info.domain == AF_INET) {
                _nfc_get_address_v4(ct, &nf->info.src_ip.v4, &nf->info.dest_ip.v4);
            } else {
                _nfc_get_address_v6(ct, &nf->info.src_ip.v6, &nf->info.dest_ip.v6);
            }
            _nfc_get_port(ct, &nf->info.src_port, &nf->info.dest_port);
            nf->info.direction = _nfc_get_direction(ct);
            
            pre_process_status_t ret = _pre_process(&nf->info, &type, sizeof(type));
            if (ret != PRE_PROCESS_REPORT_IGNORE && 
                type == NFCT_T_NEW && 
                _is_netflow_already_have(nf) == 0 && 
                _is_report_queue_full(netflow_info_cnt) == 0) {
                if (_insert_netflow_to_queue(nf) == -1) {
                    free(nf);
                    nf = NULL;
                }
            } else {
                free(nf);
                nf = NULL;
            }
        }
    }

#ifdef DAS_DEBUG
    char buf[4096] = {0};
	nfct_snprintf(buf, sizeof(buf), ct, type, NFCT_O_DEFAULT, 0);
	DAS_LOG("%s\n", buf);
#endif

	nfct_destroy(ct);

	return MNL_CB_OK;
}

static void* _nfc_monitor_thread(void *arg) 
{
	struct mnl_socket *nl;
	char buf[MNL_SOCKET_BUFFER_SIZE];
	int ret;

	nl = mnl_socket_open(NETLINK_NETFILTER);
	if (nl == NULL) {
		DAS_LOG("mnl_socket_open error.\n");
		exit(EXIT_FAILURE);
	}

    if (mnl_socket_bind(nl, NF_NETLINK_CONNTRACK_NEW | 
				NF_NETLINK_CONNTRACK_UPDATE |
				NF_NETLINK_CONNTRACK_DESTROY,
				MNL_SOCKET_AUTOPID) < 0) {
		DAS_LOG("mnl_socket_bind error.\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
		if (ret == -1) {
			DAS_LOG("mnl_socket_recvfrom error.\n");
			exit(EXIT_FAILURE);
		}

		ret = mnl_cb_run(buf, ret, 0, 0, _nfc_cb, NULL);
		if (ret == -1) {
			DAS_LOG("mnl_cb_run error.\n");
			exit(EXIT_FAILURE);
		}
	}

	mnl_socket_close(nl);

	return 0;
}

static int _nfc_init(void)
{
    int ret;
    pthread_t pid;

    ret = pthread_create(&pid, NULL, _nfc_monitor_thread, NULL);
    if (ret != 0) {
        DAS_LOG("Create _nfc_monitor_thread error.\n");
        return -1;
    }
    
    return ret;
}
#endif // DAS_SERVICE_NETFILER_USE_NFC

#if (DAS_SERVICE_NETFILER_USE_AFPACKET)
static int _afpacket_is_new_tcp_connect(netflow_info_t *info, void *payload, size_t payload_size) {
    int ret = 0;

    if (info == NULL || payload == NULL || payload_size == 0) {
        return ret;
    }

    if (info->protocol != IPPROTO_TCP && info->protocol != IPPROTO_IP) {
        ret = 1;
        return ret;
    }

    struct tcphdr *tcph = (struct tcphdr *)payload;
    if (payload_size < sizeof(struct tcphdr) || payload_size < (size_t)(tcph->doff << 2)) {
        DAS_LOG("no room for IPv4 + TCP header, skip.\n");
        return ret;
    }

    if ((tcph->th_flags & TH_SYN) && (tcph->th_flags & TH_ACK) == 0) {
        ret = 1;
    }

    return ret;
}

static void* _afpacket_monitor_thread(void *arg)
{
    void* ret = (void*)-1;
    (void)arg;

    if (afpacket_socket == -1) {
        return ((void*)ret);
    }

    for (;;) {
		struct ether_header *eh = NULL;
		int bytes = 0;
		uint16_t proto = 0;
		u_int16_t protocol = 0;
		char *pkt = NULL;
		char buf[4096] = {0};
		int off = 0;
        int domain = AF_UNSPEC;
        uint16_t sport = 0, dport = 0;
        struct iphdr *iph = NULL;
        struct ip6_hdr *ip6h = NULL;

		bytes = recv(afpacket_socket, buf, sizeof(buf), 0);
		if (bytes < 1) {
			DAS_LOG("recv date failed!\n");
			break;
		}
		
		pkt = buf;
		eh = (struct ether_header *)pkt;
		proto = ntohs(eh->ether_type);
		off = sizeof(struct ether_header);

		switch (proto) {
		case ETHERTYPE_IP: {
			iph = (struct iphdr *) (pkt + off);
            domain = AF_INET;

			/* No room for IPv4 header. */
			if (bytes - off < (int)sizeof(struct iphdr)) {
				DAS_LOG("no room for IPv4 header.\n");
				continue;
			}

			/* this is not IPv4, skip. */
			if (iph->version != 4) {
				DAS_LOG("not IPv4, skipping\n");
				continue;
			}

			off += 4 * iph->ihl;
			protocol = iph->protocol;
			break;
		}
		case ETHERTYPE_IPV6: {
			iph = (struct iphdr *) (pkt + off);
			ip6h = (struct ip6_hdr *) (pkt + off);
            domain = AF_INET6;

			/* No room for IPv6 header. */
			if (bytes - off < (int)sizeof(struct ip6_hdr)) {
				DAS_LOG("no room for IPv6 header.\n");
				continue;
			}

			/* this is not IPv6, skip. */
			if (iph->version != 6) {
				DAS_LOG("not IPv6, skipping\n");
				continue;
			}

			off += sizeof(struct ip6_hdr);
			protocol = ip6h->ip6_nxt;
			break;
		}
		default:
			/* ignore other protocols. */
			continue;
		}

        switch (protocol) {
        case IPPROTO_TCP:
        case IPPROTO_IP: {
            struct tcphdr *tcph = (struct tcphdr *) (pkt + off);
            /* No room for IPv4 header plus TCP header. */
            if (bytes - off < (int)sizeof(struct tcphdr) || bytes - off < (tcph->doff << 2)) {
                DAS_LOG("no room for IPv4 + TCP header, skip.\n");
                continue;
            }

            sport = tcph->th_sport;
            dport = tcph->th_dport;
            break;
        }
        case IPPROTO_UDP: {
            struct udphdr *udph = (struct udphdr *) (pkt + off);
            /* No room for IPv4 header plus UDP header. */
            if (bytes - off < (int)sizeof(struct udphdr)) {
                DAS_LOG("no room for IPv4 + UDP header, skip\n");
                continue;
            }

            sport = udph->uh_sport;
            dport = udph->uh_dport;
            break;
        }
        default:
            DAS_LOG("not TCP/UDP, protocol: %d\n", protocol);
            break;
        }

        netflow_info_entry_t *nf = (netflow_info_entry_t *)malloc(sizeof(netflow_info_entry_t));
        if (nf) {
            memset(nf, 0, sizeof(netflow_info_entry_t));

            nf->info.domain = domain;
            nf->info.protocol = protocol;
            if (nf->info.domain == AF_INET) {
                memcpy(&nf->info.src_ip.v4, &iph->saddr, sizeof(uint32_t));
                memcpy(&nf->info.dest_ip.v4, &iph->daddr, sizeof(uint32_t));
            } else {
                memcpy(&nf->info.src_ip.v6, &ip6h->ip6_src, sizeof(struct in6_addr));
                memcpy(&nf->info.dest_ip.v6, &ip6h->ip6_dst, sizeof(struct in6_addr));
            }
            nf->info.src_port = sport;
            nf->info.dest_port = dport;
            nf->info.direction = DIRECTION_OUT;
            
            pre_process_status_t ret = _pre_process(&nf->info, (void *)(pkt + off), bytes - off);
            if (ret != PRE_PROCESS_REPORT_IGNORE && 
                _afpacket_is_new_tcp_connect(&nf->info, (void *)(pkt + off), bytes - off) &&
                _is_netflow_already_have(nf) == 0 && 
                _is_report_queue_full(netflow_info_cnt) == 0) {
                if (_insert_netflow_to_queue(nf) == -1) {
                    free(nf);
                    nf = NULL;
                }
            } else {
                free(nf);
                nf = NULL;
            }
        }
    }

    ret = (void*)0;
    return ((void*)ret);
}

static int _afpacket_init(void)
{
    int ret = -1;
    pthread_t pid;
	struct sockaddr_ll addr;
	const char *name = FILTE_DEVICE_ANY;
	struct sock_fprog bpf = {sizeof(afpacket_filter_code) / sizeof(struct sock_filter), afpacket_filter_code};

    afpacket_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (afpacket_socket < 0) {
		DAS_LOG("Create socket failed!\n");
		goto __exit;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sll_ifindex = if_nametoindex(name);
	addr.sll_family = AF_PACKET;
	addr.sll_protocol = htons(ETH_P_ALL);
	if (bind(afpacket_socket, (struct sockaddr *)&addr, sizeof(addr))) {
		DAS_LOG("bind to device %s failed!\n", name);
		goto __exit;
	}

	if (setsockopt(afpacket_socket, SOL_SOCKET, SO_ATTACH_FILTER, &bpf, sizeof(bpf))) {
		DAS_LOG("Attaching filter failed!\n");
		goto __exit;
	}

    ret = pthread_create(&pid, NULL, _afpacket_monitor_thread, NULL);
    if (ret != 0) {
        DAS_LOG("service netfilter pthread_create error.\n");
        goto __exit;
    } else {
        return ret;
    }

__exit:
    if (afpacket_socket >= 0) {
        close(afpacket_socket);
    }

    return ret;
}
#endif // DAS_SERVICE_NETFILER_USE_AFPACKET

static das_result_t _netfilter_info(char *buffer, size_t size, das_service_state_t *state)
{
    int lock = 1;
    static int report_netflow_cnt = 0;
    static int report_custom_cnt = 0;
    das_result_t ret = DAS_ERROR_GENERIC;

    if (buffer == NULL || size == 0 || state == NULL) {
        return DAS_ERROR_BAD_PARAMETERS;
    }

	if (pthread_mutex_lock(&netfilter_mutex) != 0) {
        lock = 0;
    }

    if (state->status == DAS_SRV_STATUS_START) {
        report_netflow_cnt = netflow_info_cnt;
        report_custom_cnt = custom_info_cnt;

        netflow_info_cnt = 0;
        custom_info_cnt = 0;
    }

    if (report_netflow_cnt > 0) {
        netflow_info_entry_t *nf = TAILQ_FIRST(&netflow_queue);
        if (nf) {
            TAILQ_REMOVE(&netflow_queue, nf, link);
            ret = _fill_netflow_info(buffer, size, nf);
            free(nf);
        }
        report_netflow_cnt--;
    } else if (report_custom_cnt > 0) {
        custom_info_entry_t *cf = TAILQ_FIRST(&custom_info_queue);
        if (cf) {
            TAILQ_REMOVE(&custom_info_queue, cf, link);
            size_t len = cf->ret_size >= size ? size - 1 : cf->ret_size;
            strncpy(buffer, cf->buf, len);
            free(cf);
            ret = DAS_SUCCESS;
        }
        report_custom_cnt--;
    }

    if (lock) {
		pthread_mutex_unlock(&netfilter_mutex);
	}

    if (ret == DAS_SUCCESS) {
        if (report_netflow_cnt == 0 && report_custom_cnt == 0) {
            DAS_LOG("service netfilter report finished.\n");
            state->status = DAS_SRV_STATUS_FINISH;
        } else {
            state->status = DAS_SRV_STATUS_PUBLISH;
        }
    } else {
        memset(buffer, 0, size * sizeof(char));
    }

    return ret;
}

static int _netfilter_init(void)
{
    int ret = -1;

    if (pthread_mutex_init(&netfilter_mutex, NULL) != 0){
		DAS_LOG("Creating netfilter_mutex failed.\n");
		return ret;
	}
#if (DAS_SERVICE_NETFILER_USE_NFC)
    ret = _nfc_init();
#endif
#if (DAS_SERVICE_NETFILER_USE_AFPACKET)
    ret = _afpacket_init();
#endif
    return ret;
}

das_service_t service_netfilter = {
    .name = "NETFILTER",
    .init = _netfilter_init,
    .info = _netfilter_info,
    .attest  = NULL,
    .measure = NULL,
};

#endif /* DAS_SERVICE_NETFILER_ENABLED */
