#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <aos/aos.h>
#include <aos/errno.h>
#include <string.h>
#include "lwip/netif.h"
#include "lwip/netdb.h"
#define    PORT        80
static char remote_server_ip[16]="111.111.111.111";
char msg[] = "hello, you are connected!\n";
char domain[] = "www.baidu.com";
//static uint32_t get_default_ip(void);

extern struct hostent *lwip_gethostbyname1 (const char *__name);

static void tcp_client_thread(void * arg)
{
    struct sockaddr_in server_addr;
    int sock_fd;
    int err;
    int i=0;
    int send_err;

    struct hostent *hent = lwip_gethostbyname1(domain);
    //server_addr.sin_addr.s_addr = inet_addr("192.168.0.100");
    printf("remote h_addr = %s\n", hent->h_addr);
    printf("remote h_name = %s\n", hent->h_name);

    printf("tcp_client_thread entry...\n");
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        //Uart_Printf("failed to create sock_fd!\n");
        printf("failed to create sock_fd!\n");
        //RAW_ASSERT(0);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    //server_addr.sin_addr.s_addr = inet_addr("192.168.0.100");
    //printf("remote h_addr = %d\n", ntohl(inet_addr(hent->h_addr)));
    //printf("remote h_name = %d\n", ntohl(inet_addr(hent->h_name)));
    server_addr.sin_addr.s_addr = *(u32_t *)(hent->h_addr);//get_default_ip();//inet_addr(remote_server_ip);
    //printf("remote_server_ip = %d\n", (get_default_ip()));
    //server_addr.sin_addr.s_addr = (get_default_ip());//get_default_ip();//inet_addr(remote_server_ip);
    server_addr.sin_port = htons(PORT);
    err = connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    //printf("err = %d errno=%d\n", err, errno);

    while (i<20) {
        send_err = send(sock_fd, (char *)msg, sizeof(msg), 0);
        aos_msleep(2000);//raw_sleep(100);
        printf("\n\n\n\n send_err = %d \n\n\n\n", send_err);
        printf("\n\n\n\n tcp_client \n\n\n\n");
    }
//    close(sock_fd);
}
void tcp_client_init(void)
{
    sys_thread_new("tcp_client", tcp_client_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
void start_tcp_client(void)
{
	tcp_client_thread(NULL);
}

