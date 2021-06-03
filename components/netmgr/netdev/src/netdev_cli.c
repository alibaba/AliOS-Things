/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <stdarg.h>

#include "k_api.h"
#include "ulog/ulog.h"

#define TAG "netdev_cli"

#include <netdev_ipaddr.h>
#include <netdev.h>
#include <aos/cli.h>
#include <aos/list.h>

#define NDEV_PRINT aos_cli_printf

#define NDEV_PRINT_BUFFER_SIZE     128

typedef struct netdev_fd_info {
    slist_t next;
    struct netdev* netdev;
    int fd;
} netdev_fd_info_t;

static slist_t g_netdev_fd_list_head;

static char netdev_res_print_buffer[NDEV_PRINT_BUFFER_SIZE];

static void netdev_res_print(int fd, const char *fmt, ...)
{
    va_list  args;

    va_start(args, fmt);
    memset(netdev_res_print_buffer, 0, NDEV_PRINT_BUFFER_SIZE);
    vsprintf(netdev_res_print_buffer, fmt, args);
    va_end(args);

    if (fd >= 0) {
        write(fd, netdev_res_print_buffer, strlen(netdev_res_print_buffer));
    } else {
        NDEV_PRINT(netdev_res_print_buffer);
    }
}

static void netdev_list_if(int fd)
{
#define NETDEV_IFCONFIG_MAC_MAX_LEN    6
#define NETDEV_IFCONFIG_IEMI_MAX_LEN   8

    int index;
    slist_t *node  = NULL;
    struct netdev *netdev = NULL;
    struct netdev *cur_netdev_list = netdev_list;

    if (cur_netdev_list == NULL)
    {
        NDEV_PRINT("ifconfig: network interface device list error.\n");
        return;
    }

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    slist_for_each_entry_safe(&(netdev_list->list), node, netdev, struct netdev, list)
    {
        /* line 1 */
        netdev_res_print(fd, "%-10s", netdev->name);
        netdev_res_print(fd, "HWaddr ");
		    /* 6 - MAC address, 8 - IEMI */
        if (netdev->hwaddr_len == NETDEV_IFCONFIG_MAC_MAX_LEN)
        {
            for (index = 0; index < netdev->hwaddr_len; index++)
            {
                netdev_res_print(fd, "%02x ", netdev->hwaddr[index]);
            }
        }
        else if (netdev->hwaddr_len == NETDEV_IFCONFIG_IEMI_MAX_LEN)
        {
            for (index = 0; index < netdev->hwaddr_len; index++)
            {
                /* two numbers are displayed at one time*/
                if (netdev->hwaddr[index] < 10 && index != netdev->hwaddr_len - 1)
                    netdev_res_print(fd, "0");

                netdev_res_print(fd, "%d", netdev->hwaddr[index]);
            }
        }
		netdev_res_print(fd, " %s\n",(netdev == netdev_default)?"(Default)":"");

		/* line 2 */
		netdev_res_print(fd, "          inet addr:%s", inet_ntoa(netdev->ip_addr));
		//NDEV_PRINT("  Bcast:");
		netdev_res_print(fd, "  Mask:%s", inet_ntoa(netdev->netmask));
	    netdev_res_print(fd, "\n");

		/* line 3 */
		netdev_res_print(fd, "          Gateway:%s", inet_ntoa(netdev->gw));
		for (index = 0; index < NETDEV_DNS_SERVERS_NUM; index++)
        {
            netdev_res_print(fd, "  DNS %d: %s", index+1, inet_ntoa(netdev->dns_servers[index]));
        }
		netdev_res_print(fd, "\n");

		/* line 4 */
		netdev_res_print(fd, "         ");
		if (netdev->flags & NETDEV_FLAG_UP) netdev_res_print(fd, " UP");
        else netdev_res_print(fd, " DOWN");
        if (netdev->flags & NETDEV_FLAG_LINK_UP) netdev_res_print(fd, " LINK_UP");
        else netdev_res_print(fd, " LINK_DOWN");
        if (netdev->flags & NETDEV_FLAG_INTERNET_UP) netdev_res_print(fd, " INTERNET_UP");
        else netdev_res_print(fd, " INTERNET_DOWN");
        if (netdev->flags & NETDEV_FLAG_DHCP) netdev_res_print(fd, " DHCP_ENABLE");
        else netdev_res_print(fd, " DHCP_DISABLE");
        if (netdev->flags & NETDEV_FLAG_ETHARP) netdev_res_print(fd, " ETHARP");
        if (netdev->flags & NETDEV_FLAG_BROADCAST) netdev_res_print(fd, " BROADCAST");
        if (netdev->flags & NETDEV_FLAG_IGMP) netdev_res_print(fd, " IGMP");
		netdev_res_print(fd, "  MTU:%d", netdev->mtu);
		netdev_res_print(fd, "\n");
/* Removed by chen_shiyu@anyka.oa */
#if 0
        NDEV_PRINT("network interface device: %s%s\n",
                   netdev->name,
                   (netdev == netdev_default)?" (Default)":"");
        NDEV_PRINT("MTU: %d\n", netdev->mtu);

        /* 6 - MAC address, 8 - IEMI */
        if (netdev->hwaddr_len == NETDEV_IFCONFIG_MAC_MAX_LEN)
        {
            NDEV_PRINT("MAC: ");
            for (index = 0; index < netdev->hwaddr_len; index++)
            {
                NDEV_PRINT("%02x ", netdev->hwaddr[index]);
            }
        }
        else if (netdev->hwaddr_len == NETDEV_IFCONFIG_IEMI_MAX_LEN)
        {
            NDEV_PRINT("IEMI: ");
            for (index = 0; index < netdev->hwaddr_len; index++)
            {
                /* two numbers are displayed at one time*/
                if (netdev->hwaddr[index] < 10 && index != netdev->hwaddr_len - 1)
                    NDEV_PRINT("0");

                NDEV_PRINT("%d", netdev->hwaddr[index]);
            }
        }

        NDEV_PRINT("\nFLAGS:");
        if (netdev->flags & NETDEV_FLAG_UP) NDEV_PRINT(" UP");
        else NDEV_PRINT(" DOWN");
        if (netdev->flags & NETDEV_FLAG_LINK_UP) NDEV_PRINT(" LINK_UP");
        else NDEV_PRINT(" LINK_DOWN");
        if (netdev->flags & NETDEV_FLAG_INTERNET_UP) NDEV_PRINT(" INTERNET_UP");
        else NDEV_PRINT(" INTERNET_DOWN");
        if (netdev->flags & NETDEV_FLAG_DHCP) NDEV_PRINT(" DHCP_ENABLE");
        else NDEV_PRINT(" DHCP_DISABLE");
        if (netdev->flags & NETDEV_FLAG_ETHARP) NDEV_PRINT(" ETHARP");
        if (netdev->flags & NETDEV_FLAG_BROADCAST) NDEV_PRINT(" BROADCAST");
        if (netdev->flags & NETDEV_FLAG_IGMP) NDEV_PRINT(" IGMP");
        NDEV_PRINT("\n");
        NDEV_PRINT("ip address: %s\n", inet_ntoa(netdev->ip_addr));
        NDEV_PRINT("gw address: %s\n", inet_ntoa(netdev->gw));
        NDEV_PRINT("net mask  : %s\n", inet_ntoa(netdev->netmask));

        for (index = 0; index < NETDEV_DNS_SERVERS_NUM; index++)
        {
            NDEV_PRINT("dns server #%d: %s\n", index, inet_ntoa(netdev->dns_servers[index]));
        }
#endif

        if (node && node->next)
        {
            netdev_res_print(fd, "\n");
        }
    }

    RHINO_CPU_INTRPT_ENABLE();
}

static void netdev_set_if(char* netdev_name, char* ip_addr, char* gw_addr, char* nm_addr)
{
    struct netdev *netdev = NULL;
    ip_addr_t addr;

    netdev = netdev_get_by_name(netdev_name);
    if (netdev == NULL)
    {
        LOGE(TAG, "bad network interface device name(%s).\n", netdev_name);
        return;
    }

    /* set IP address */
    if ((ip_addr != NULL) && inet_aton(ip_addr, &addr))
    {
        netdev_set_ipaddr(netdev, &addr);
    }

    /* set gateway address */
    if ((gw_addr != NULL) && inet_aton(gw_addr, &addr))
    {
        netdev_set_gw(netdev, &addr);
    }

    /* set netmask address */
    if ((nm_addr != NULL) && inet_aton(nm_addr, &addr))
    {
        netdev_set_netmask(netdev, &addr);
    }
}

static int add_fd_info(struct netdev* netdev, int fd)
{
    netdev_fd_info_t* cur;

    cur = malloc(sizeof(netdev_fd_info_t));
    if(cur == NULL) {
        return -1;
    }
    memset(cur, 0, sizeof(netdev_fd_info_t));

    cur->fd = fd;
    cur->netdev = netdev;

    slist_add_tail(&cur->next, &g_netdev_fd_list_head);

    return 0;
}

static int del_fd_by_netdev(struct netdev* netdev)
{
    netdev_fd_info_t* cur;
    int found = 0;

    slist_for_each_entry(&g_netdev_fd_list_head, cur, netdev_fd_info_t, next) {
        if(cur->netdev == netdev) {
            found = 1;
            break;
        }
    }

    if(1 == found) {
        slist_del(&cur->next, &g_netdev_fd_list_head);
        return 0;
    } else {
        return -1;
    }
}

static int get_fd_by_netdev(struct netdev* netdev)
{
    netdev_fd_info_t* cur;
    int found = 0;

    slist_for_each_entry(&g_netdev_fd_list_head, cur, netdev_fd_info_t, next) {
        if(cur->netdev == netdev) {
            found = 1;
            break;
        }
    }

    if(1 == found) {
        return cur->fd;
    } else {
        return -1;
    }
}
extern int netdev_dhcpd_enabled(struct netdev *netdev, bool is_enabled);

void netdev_ifconfig(char *pwbuf, int blen, int argc, char **argv)
{
    char usage[] = "usage:  \r\n"
                   "  ifconfig <eth0|wlan0> <up|down> \r\n"
                   "eg:\r\n"
                   "  1.enable ethernet:\r\n"
                   "    ifconfig eth0 up\r\n\r\n"
                   "  2.disable ethernet:\r\n"
                   "    ifconfig eth0 down\r\n";

    char dev_name[32];

    if ((argc == 1) || (!strcmp(argv[1], ">") && argc == 3))
    {
        int fd = -1;
        if (argc == 3) {
            fd = open(argv[2], O_RDWR | O_CREAT);
        }
        netdev_list_if(fd);
        if (fd >= 0) {
            close(fd);
        }
		return ;
    }

	if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "help") == 0)
	{
		NDEV_PRINT("%s \n", usage);
		return ;
	}

	struct netdev * dev = NULL;
	dev = netdev_get_by_name(argv[1]);
	if(!dev)
	{
		NDEV_PRINT("can't find %s !\n", argv[1]);
		return ;
	}

	if (argc == 3)
    {
        if (strcmp(argv[2], "up") == 0)
        {
            int fd = -1;

            fd = get_fd_by_netdev(dev);
            if((fd != -1) && (!netdev_is_up(dev))) {
                del_fd_by_netdev(dev);
                fd = -1;
            }

            if(fd == -1) {
                snprintf(dev_name, sizeof(dev_name), "/dev/%s", argv[1]);
                if((fd = open(dev_name, O_RDWR)) < 0)
            {
                NDEV_PRINT("open %s fail !\n",argv[1]);
                return ;
            }
            netdev_set_up(dev);
                add_fd_info(dev, fd);
            } else {
                NDEV_PRINT("%s is already up\n",argv[1]);
            }
        }
        else if (strcmp(argv[2], "down") == 0)
        {
            int fd = -1;

            fd = get_fd_by_netdev(dev);
            if(fd != -1) {
                del_fd_by_netdev(dev);
                close(fd);
                fd = -1;
            }

            if(netdev_is_up(dev)) {
                netdev_set_down(dev);
            } else {
                NDEV_PRINT("%s is already down\n",argv[1]);
            }
        }
        else if (strcmp(argv[2], "def") == 0)
        {
            netdev_set_default(dev);
        }
        else
        {
            NDEV_PRINT("Invalid argument: %s\n", argv[1]);
        }
    }
	else if(argc == 4 && strcmp(argv[2], "dhcp") == 0)
	{
		if(strcmp(argv[3], "on") == 0)
		{
			netdev_dhcp_enabled(dev, true);
		}
		else if(strcmp(argv[3], "off") == 0)
		{
			netdev_dhcp_enabled(dev, false);
		}
	}
    else if(argc == 4 && strcmp(argv[2], "dhcpd") == 0)
    {
        if(strcmp(argv[3], "on") == 0)
        {
            netdev_dhcpd_enabled(dev, true);
        }
        else if(strcmp(argv[3], "off") == 0)
        {
            netdev_dhcpd_enabled(dev, false);
        }
    }
    else if (argc == 5)
    {
        NDEV_PRINT("config : %s\n", argv[1]);
        NDEV_PRINT("IP addr: %s\n", argv[2]);
        NDEV_PRINT("Gateway: %s\n", argv[3]);
        NDEV_PRINT("netmask: %s\n", argv[4]);
        netdev_set_if(argv[1], argv[2], argv[3], argv[4]);
    }
    else
    {
        LOGE(TAG, "bad parameter! \n %s \n", usage);
    }
}

static struct cli_command ifconfig_cmd = {
    .name     = "ifconfig",
    .help     = "list the information of all network interfaces",
    .function = netdev_ifconfig,
};

static int netdev_cmd_ping(char* target_name, uint32_t times, size_t size, int fd)
{
#define NETDEV_PING_DATA_SIZE       32
/** ping receive timeout - in milliseconds */
#define NETDEV_PING_RECV_TIMEO      2000 //(2 * RT_TICK_PER_SECOND)
/** ping delay - in milliseconds */
#define NETDEV_PING_DELAY           1000 //(1 * RT_TICK_PER_SECOND)
/* check netdev ping options */
#define NETDEV_PING_IS_COMMUNICABLE(netdev) \
    ((netdev) && (netdev)->ops && (netdev)->ops->ping && \
        netdev_is_up(netdev) && netdev_is_link_up(netdev)) \

    struct netdev *netdev = NULL;
    struct netdev_ping_resp ping_resp;
    uint32_t index, ret = 0;
    uint32_t resp_cnt = 0;
    //rt_tick_t start_time = rt_tick_get();
    uint64_t start_time = aos_now_ms();
	uint64_t min_us=0,avg_us=0,max_us=0,mdev_us=0,sum_us=0;
	uint64_t d_sum = 0;

    if (size == 0)
    {
        size = NETDEV_PING_DATA_SIZE;
    }

    if (NETDEV_PING_IS_COMMUNICABLE(netdev_default))
    {
        /* using default network interface device for ping */
        netdev = netdev_default;
    }
    else
    {
        /* using first internet up status network interface device */
        netdev = netdev_get_first_by_flags(NETDEV_FLAG_LINK_UP);
        printf("%s,%d, netdev %p, (netdev)->ops %p, (netdev)->ops->ping %p, is_up %d, is_link_up %d\r\n",
                __func__,__LINE__,netdev,(netdev)->ops, (netdev)->ops->ping, netdev_is_up(netdev),
                netdev_is_link_up(netdev));
        if (netdev == NULL || NETDEV_PING_IS_COMMUNICABLE(netdev) == 0)
        {
            LOGE(TAG, "ping: network interface device get error.\n");
            return -1;
        }
    }

    for (index = 0; index < times; index++)
    {
        memset(&ping_resp, 0x00, sizeof(struct netdev_ping_resp));
        ret = netdev->ops->ping(netdev, (const char *)target_name, size, NETDEV_PING_RECV_TIMEO, &ping_resp);
        if (ret == -2)
        {
            netdev_res_print(fd, "ping: from %s icmp_seq=%d timeout\n",
                (ip_addr_isany(&(ping_resp.ip_addr))) ? target_name : inet_ntoa(ping_resp.ip_addr), index);
        }
        else if (ret == -1)
        {
            netdev_res_print(fd, "ping: unknown %s %s\n",
                (ip_addr_isany(&(ping_resp.ip_addr))) ? "host" : "address",
                (ip_addr_isany(&(ping_resp.ip_addr))) ? target_name : inet_ntoa(ping_resp.ip_addr));
        }
        else
        {
            resp_cnt++;
			sum_us += ping_resp.ticks;
			d_sum += ping_resp.ticks * ping_resp.ticks;

			if(ping_resp.ticks > max_us || max_us == 0)
				max_us = ping_resp.ticks;

			if(ping_resp.ticks < min_us || min_us == 0)
				min_us = ping_resp.ticks;

            if (ping_resp.ttl == 0)
            {
                netdev_res_print(fd, "%d bytes from %s icmp_seq=%d time=%lld ms\n",
                            ping_resp.data_len, inet_ntoa(ping_resp.ip_addr), index, ping_resp.ticks);
            }
            else
            {
                netdev_res_print(fd, "%d bytes from %s icmp_seq=%d ttl=%d time=%d.%03d ms\n",
                            ping_resp.data_len, inet_ntoa(ping_resp.ip_addr), index, ping_resp.ttl, (int)ping_resp.ticks/1000,(int)ping_resp.ticks%1000);

            }
        }
        aos_msleep(NETDEV_PING_DELAY);
    }

	//int time = (int)(rt_tick_get() - start_time) * 5 - NETDEV_PING_DELAY;
	int time = (int)(aos_now_ms() - start_time) - NETDEV_PING_DELAY;
    netdev_res_print(fd, "\n--- %s ping %s statistics --- \n",netdev->name,target_name);
	netdev_res_print(fd, "%d packets transmitted, %d packets received, %d%s packets lost, time %d ms\n", \
		times, resp_cnt,(int)((times - resp_cnt)*100/times),"%",time);

    if(resp_cnt != 0) {
	    avg_us = sum_us/resp_cnt;

	    //mdev = sqrt(sum(rtt^2)/n - (sum(rtt)/n)^2)
	    mdev_us = sqrt(d_sum / resp_cnt - avg_us * avg_us);
    }
	netdev_res_print(fd, "min/avg/max/mdev = %d.%03d/%d.%03d/%d.%03d/%d.%03d ms \n",
		(int)min_us/1000, (int)min_us%1000,(int)avg_us/1000,(int)avg_us%1000,(int)max_us/1000,(int)max_us%1000,(int)mdev_us/1000,(int)mdev_us%1000);

    return 0;
}

void netdev_ping(char *pwbuf, int blen, int argc, char **argv)
{
    uint32_t times = 10;
    int size = 32;
    int i;
	char ping_usage[] = "usage:\n\t ping <host address> [-c count] [-s packetssize] \
    \n\t e.g:\n\t ping 192.168.1.1 -s 1400 \n\t ping 192.168.1.1 -c 0 (run forerver) \n";
    int fd = -1;

    if (argc == 1 || (strcmp(argv[1], "-h") == 0))
    {
        NDEV_PRINT("%s\n", ping_usage);
		return ;
    }
//add by li_zhihai@any.oa
    if(argc >= 2)
    {
        for (i = 2 ; i < argc; i++)
        {
            if(strcmp(argv[i], "-c") == 0)
            {
                i++;
                if(argv[i] == NULL)
                    NDEV_PRINT("%s \n",ping_usage);
                else
                    times = atoi(argv[i]) > 0 ? atoi(argv[i]) : 0xffffffff;

            }else if(strcmp(argv[i], "-s") == 0)
            {
                i++;
                if(argv[i] == NULL)
                    NDEV_PRINT("%s \n",ping_usage);
                else
                    size = atoi(argv[i]);
            }else if(strcmp(argv[i], ">") == 0)
            {
                i++;

                if (fd >= 0) {
                    close(fd);
                }

                fd = open(argv[i], O_RDWR | O_CREAT);
            }
            else
            {
                NDEV_PRINT("invalid para:[%s]\n %s \n", argv[i], ping_usage);
                if (fd >= 0) {
                    close(fd);
                }
				return ;
            }
        }
        //NDEV_PRINT("ping times = %d, size = %d \n", times, size);
//end add
        netdev_cmd_ping(argv[1], times, size, fd);

        if (fd >= 0) {
            close(fd);
        }
    }
}
static struct cli_command ping_cmd = {
    .name     = "ping",
    .help     = "ping network host",
    .function = netdev_ping,
};

static void netdev_list_dns(void)
{
    int index = 0;
    struct netdev *netdev = NULL;
    slist_t *node  = NULL;

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    slist_for_each_entry_safe(&(netdev_list->list), node, netdev, struct netdev, list)
    {
        NDEV_PRINT("network interface device: %s%s\n",
                netdev->name,
                (netdev == netdev_default)?" (Default)":"");

        for(index = 0; index < NETDEV_DNS_SERVERS_NUM; index++)
        {
            NDEV_PRINT("dns server #%d: %s\n", index, inet_ntoa(netdev->dns_servers[index]));
        }

        if (node && node->next)
        {
            NDEV_PRINT("\n");
        }
    }

    RHINO_CPU_INTRPT_ENABLE();
}

static void netdev_set_dns(char *netdev_name, uint8_t dns_num, char *dns_server)
{
    struct netdev *netdev = NULL;
    ip_addr_t dns_addr;

    netdev = netdev_get_by_name(netdev_name);
    if (netdev == NULL)
    {
        NDEV_PRINT("bad network interface device name(%s).\n", netdev_name);
        return;
    }

    if ((dns_server != NULL) && inet_aton(dns_server, &dns_addr)) {
        netdev_set_dns_server(netdev, dns_num, &dns_addr);
    }

    NDEV_PRINT("set network interface device(%s) dns server #0: %s\n", netdev_name, dns_server);
}

void netdev_dns(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc == 1)
    {
        netdev_list_dns();
    }
    else if (argc == 3)
    {
        netdev_set_dns(argv[1], 0, argv[2]);
    }
    else if (argc == 4)
    {
        netdev_set_dns(argv[1], atoi(argv[2]), argv[3]);
    }
    else
    {
        NDEV_PRINT("bad parameter! input: dns <netdev_name> [dns_num] <dns_server>\n");
    }
}

static struct cli_command dns_cmd = {
    .name     = "dns",
    .help     = "list and set the information of dns",
    .function = netdev_dns,
};

static void netdev_cmd_netstat(void)
{
    slist_t *node  = NULL;
    struct netdev *netdev = NULL;
    struct netdev *cur_netdev_list = netdev_list;

    if (cur_netdev_list == NULL)
    {
        NDEV_PRINT("netstat: network interface device list error.\n");
        return;
    }

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    slist_for_each_entry_safe(&(netdev_list->list), node, netdev, struct netdev, list)
    {
        if (netdev && netdev->ops && netdev->ops->netstat)
        {
            break;
        }
    }

    RHINO_CPU_INTRPT_ENABLE();
    if(netdev && netdev->ops) {
        netdev->ops->netstat(netdev);
    }
}

void netdev_netstat(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc != 1)
    {
        NDEV_PRINT("Please input: netstat \n");
    }
    else
    {
        netdev_cmd_netstat();
    }
}

static struct cli_command netstat_cmd = {
    .name     = "netstat",
    .help     = "list the information of TCP/IP",
    .function = netdev_netstat,
};

void netdev_cli_register(void)
{
    aos_cli_register_command(&ifconfig_cmd);
    aos_cli_register_command(&ping_cmd);
    aos_cli_register_command(&dns_cmd);
    aos_cli_register_command(&netstat_cmd);
}
