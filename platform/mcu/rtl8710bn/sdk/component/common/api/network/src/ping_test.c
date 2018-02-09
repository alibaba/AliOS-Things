//#include "FreeRTOS.h"
//#include "task.h"
#include "main.h"

#include <lwip/sockets.h>
#include <lwip/raw.h>
#include <lwip/icmp.h>
#include <lwip/inet_chksum.h>
#include <lwip/netdb.h>
#include <platform/platform_stdlib.h>

#include "osdep_service.h"

#define PING_IP		"192.168.159.1"
#define PING_TO		1000
#define PING_ID		0xABCD
#define BUF_SIZE	10000
#define STACKSIZE	1024

static unsigned short ping_seq = 0;
static int infinite_loop, ping_count, data_size, ping_interval, ping_call;
static char ping_ip[16];
static char host[64];

static int ping_total_time = 0, ping_received_count = 0;

struct task_struct g_ping_test_task;


static void generate_ping_echo(unsigned char *buf, int size)
{
	int i;
	struct icmp_echo_hdr *pecho;

	for(i = 0; i < size; i ++) {
		buf[sizeof(struct icmp_echo_hdr) + i] = (unsigned char) i;
	}

	pecho = (struct icmp_echo_hdr *) buf;
	ICMPH_TYPE_SET(pecho, ICMP_ECHO);
	ICMPH_CODE_SET(pecho, 0);
	pecho->chksum = 0;
	pecho->id = PING_ID;
	pecho->seqno = htons(++ ping_seq);

	//Checksum includes icmp header and data. Need to calculate after fill up icmp header
	pecho->chksum = inet_chksum(pecho, sizeof(struct icmp_echo_hdr) + size);
}

void ping_test(void *param)
{
	int i, ping_socket;
	int pint_timeout = PING_TO;
	struct sockaddr_in to_addr, from_addr;
	int from_addr_len = sizeof(struct sockaddr);
	int ping_size, reply_size;
	unsigned char *ping_buf, *reply_buf;
	unsigned int ping_time, reply_time;
	struct ip_hdr *iphdr;
	struct icmp_echo_hdr *pecho;
	unsigned int min_time = 1000, max_time = 0;
	struct hostent *server_host;

	ping_total_time = 0;
	ping_received_count = 0;
	
	if(data_size > BUF_SIZE){
	    printf("\n\r[ERROR] %s: data size error, can't exceed %d",__func__,BUF_SIZE);
	    return;
	}

	//Ping size = icmp header(8 bytes) + data size
	ping_size = sizeof(struct icmp_echo_hdr) + data_size;

	ping_buf = rtw_zmalloc(ping_size);
    if(NULL == ping_buf){
        printf("\n\r[ERROR] %s: Allocate ping_buf failed",__func__);
        return;
    }

    reply_buf = rtw_zmalloc(ping_size);
    if(NULL == reply_buf){
        rtw_free(ping_buf);
        printf("\n\r[ERROR] %s: Allocate reply_buf failed",__func__);
        return;
    }

	printf("\n\r[%s] PING %s %d(%d) bytes of data\n", __FUNCTION__, host, data_size, sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr) + data_size);			

	for(i = 0; (i < ping_count) || (infinite_loop == 1); i ++) {
		ping_socket = socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP);
#if defined(LWIP_SO_SNDRCVTIMEO_NONSTANDARD) && (LWIP_SO_SNDRCVTIMEO_NONSTANDARD == 0)	// lwip 1.5.0
		struct timeval timeout;
		timeout.tv_sec = pint_timeout / 1000;
		timeout.tv_usec = pint_timeout % 1000 * 1000;
		setsockopt(ping_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#else	// lwip 1.4.1
		setsockopt(ping_socket, SOL_SOCKET, SO_RCVTIMEO, &pint_timeout, sizeof(pint_timeout));
#endif
		to_addr.sin_len = sizeof(to_addr);
		to_addr.sin_family = AF_INET;
		if (inet_aton(host, &to_addr.sin_addr) == 0) {
			server_host = gethostbyname(host);
			printf("\r\n host name %s, server %x\r\n", host, server_host);
			if(server_host == NULL){
				printf("\n\r[%s] Get host name failed in the %d ping test\n", __FUNCTION__, (i + 1));
				close(ping_socket);
				rtw_msleep_os(ping_interval * configTICK_RATE_HZ);
				continue;
			}
			memcpy((void *) &to_addr.sin_addr, (void *) server_host->h_addr, server_host->h_length);
		}
		else	
			to_addr.sin_addr.s_addr = inet_addr(host);

		generate_ping_echo(ping_buf, data_size);
		sendto(ping_socket, ping_buf, ping_size, 0, (struct sockaddr *) &to_addr, sizeof(to_addr));
		
		ping_time = rtw_get_current_time();
		if((reply_size = recvfrom(ping_socket, reply_buf, ping_size, 0, (struct sockaddr *) &from_addr, (socklen_t *) &from_addr_len))
			>= (int)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr)) && (from_addr.sin_addr.s_addr == to_addr.sin_addr.s_addr)) {

			reply_time = rtw_get_current_time();
			iphdr = (struct ip_hdr *)reply_buf;
			pecho = (struct icmp_echo_hdr *)(reply_buf + (IPH_HL(iphdr) * 4));

			if((pecho->id == PING_ID) && (pecho->seqno == htons(ping_seq))) {
				printf("\n\r[%s] %d bytes from %s: icmp_seq=%d time=%d ms", __FUNCTION__, reply_size - sizeof(struct icmp_echo_hdr), inet_ntoa(from_addr.sin_addr), htons(pecho->seqno), (reply_time - ping_time));
				ping_received_count++;
				ping_total_time += (reply_time - ping_time);
				if((reply_time - ping_time) > max_time) max_time = (reply_time - ping_time);
				if((reply_time - ping_time) < min_time) min_time = (reply_time - ping_time);
			}
		}
		else
			printf("\n\r[%s] Request timeout for icmp_seq %d\n", __FUNCTION__, ping_seq);

		close(ping_socket);
		rtw_msleep_os(ping_interval * configTICK_RATE_HZ);
	}

	printf("\n\r[%s] %d packets transmitted, %d received, %d%% packet loss, average %d ms", __FUNCTION__, ping_count, ping_received_count, (ping_count-ping_received_count)*100/ping_count, ping_total_time/ping_received_count);
	printf("\n\r[%s] min: %d ms, max: %d ms\n\r", __FUNCTION__, min_time, max_time);
	rtw_free(ping_buf);
	rtw_free(reply_buf);

	if(!ping_call)
		rtw_delete_task(&g_ping_test_task);
}

void do_ping_call(char *ip, int loop, int count)
{
	ping_call = 1;
	ping_seq = 0;
	data_size = 120;
	ping_interval = 1;
	infinite_loop = loop;
	ping_count = count;
	strcpy(ping_ip, ip);
	ping_test(NULL);
}

int get_ping_report(int *ping_lost){
	*ping_lost = ping_count - ping_received_count;
	return 0;
}

void cmd_ping(int argc, char **argv)
{
    int argv_count = 2;
    printf("\n\r[ATWI]  argc %d, argv, 1 %s, 2 %s, 3 %s\n", argc, argv[0], argv[1], argv[2]);
	
    if(argc < 2)
        goto Exit;

    //ping cmd default value
    infinite_loop = 0;
    ping_count    = 4;
    data_size     = 32;
    ping_interval = 1;
    ping_call     = 1;
    ping_seq      = 0;

    while(argv_count<=argc){
        //first operation
        if(argv_count == 2){
            memset(host, 0, sizeof(host));
            strncpy(host, argv[argv_count-1], strlen(argv[argv_count-1]));
            argv_count++;
        }
        else{
            if(strcmp(argv[argv_count-1], "-t") == 0){
                infinite_loop = 1;
                argv_count++;
            }
            else if(strcmp(argv[argv_count-1], "-n") == 0){
                if(argc < (argv_count+1))
                    goto Exit;
                ping_count = (int) atoi(argv[argv_count]);
                argv_count+=2;
            }
            else if(strcmp(argv[argv_count-1], "-l") == 0){
                if(argc < (argv_count+1))
                    goto Exit;
                data_size = (int) atoi(argv[argv_count]);
                argv_count+=2;
            }
            else{
                goto Exit;
            }
        }
    }

    ping_test(NULL);

    return;

Exit:
    printf("\n\r[ATWI] Usage: ATWI=[host],[options]\n");
    printf("\n\r     -t        Ping the specified host until stopped\n");
    printf("  \r     -n    #   Number of echo requests to send (default 4 times)\n");
    printf("  \r     -l    #   Send buffer size (default 32 bytes)\n");
    printf("\n\r   Example:\n");
    printf("  \r     ATWI=192.168.1.2,-n,100,-l,5000\n");
    return;
}

void do_ping_test(char *ip, int size, int count, int interval)
{
	if((sizeof(struct icmp_echo_hdr) + size) > BUF_SIZE) {
		printf("\n\r%s BUF_SIZE(%d) is too small", __FUNCTION__, BUF_SIZE);
		return;
	}

	if(ip == NULL)
		strcpy(ping_ip, PING_IP);
	else
		strcpy(ping_ip, ip);

	ping_call = 0;
	ping_seq = 0;
	data_size = size;
	ping_interval = interval;

	if(count == 0) {
		infinite_loop = 1;
		ping_count = 0;
	}
	else {
		infinite_loop = 0;
		ping_count = count;
	}

	//if(xTaskCreate(ping_test, ((const signed char*)"ping_test"), STACKSIZE, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
	//	printf("\n\r%s xTaskCreate failed", __FUNCTION__);
	if(rtw_create_task(&g_ping_test_task, "ping_test", STACKSIZE, 5, ping_test, NULL) != pdPASS)
		printf("\r\nUDP ERROR: Create ping test task failed.");									
}
