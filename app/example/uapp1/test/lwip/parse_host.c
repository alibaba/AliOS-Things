#include <lwip/sockets.h>
#include <lwip/netdb.h>

typedef struct {
    char *name;
    char *port;
} host_info_t;

host_info_t hosts[] = {
    {"www.alibaba.com", NULL},
    {"www.alipay.com", NULL},
    {"www.taobao.com", NULL},
    {"www.tianmao.com", NULL},
    {"www.cainiao.com", NULL},
    {"www.baidu.com", NULL},
    {"www.souhu.com", NULL},

};

static int parse_host(void)
{
    struct addrinfo hints;
    struct addrinfo *res, *cur;
    struct sockaddr_in *addr;
    struct hostent *host;
    char ipbuf[16];
    int ret;
    int i;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_socktype = SOCK_STREAM;

    printf("============  getaddrinfo ============\r\n");
    for (i = 0; i < sizeof(hosts)/sizeof(host_info_t); i++) {
        ret = getaddrinfo(hosts[i].name, hosts[i].port, &hints, &res);
        if (ret != 0)
            return -1;

        for (cur = res; cur != NULL; cur = cur->ai_next) {
            addr = (struct sockaddr_in *)cur->ai_addr;
            printf("\t%-26s ipaddr: ",hosts[i].name);
            printf("%s\r\n", inet_ntop(AF_INET, &addr->sin_addr, ipbuf, 16));
        }

        freeaddrinfo(res);
    }

    printf("============  gethostbyname ===========\r\n");
    for (i = 0; i < sizeof(hosts)/sizeof(host_info_t); i++) {
        host = gethostbyname(hosts[i].name);
        if (host == NULL) {
            return -2;
        }
        printf("\t%-26s ipaddr: ", hosts[i].name);
        printf("%s\r\n", inet_ntoa(*((ip_addr_t *)host->h_addr_list[0])));
    }

    return 0;
}

int net_parse_host(void)
{
    return parse_host();
}


