#include "include.h"

#if CFG_USE_LWIP_NETSTACK
#include "lwip/tcp.h"
#include "app_lwip_tcp.h"
#include "app_led.h"
#include "uart_pub.h"
#include "mem_pub.h"

#include "FreeRTOS.h"
#include "task.h"
#include "rtos_pub.h"
#include "error.h"
#include "lwip/sockets.h"

#include "app_drone.h"

#include "video_transfer.h"


#if CFG_SUPPORT_TIANZHIHENG_DRONE

#define LWIP_TCP_DEBUG        1
#if LWIP_TCP_DEBUG
#define LWIP_TCP_PRT      warning_prf
#define LWIP_TCP_WARN     warning_prf
#define LWIP_TCP_FATAL    fatal_prf
#else
#define LWIP_TCP_PRT      null_prf
#define LWIP_TCP_WARN     null_prf
#define LWIP_TCP_FATAL    null_prf
#endif

#define APP_TCP_LISTEN_MAX              1  
#define APP_TCP_RCV_BUF_LEN             1460

xTaskHandle app_tcp_hdl = NULL;
int watch_fd_list[APP_TCP_LISTEN_MAX];
int tcp_server_fd;
volatile int app_tcp_run = 0;

static void app_lwip_tcp_set_keepalive(int fd)
{
    int opt = 1, ret;
    // open tcp keepalive
    ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(int));

    opt = 30;  // 5 second
    ret = setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &opt, sizeof(int)); 

    opt = 1;  // 1s second for intval
    ret = setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &opt, sizeof(int)); 

    opt = 3;  // 3 times
    ret = setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &opt, sizeof(int));  
    ret = ret;
}

static void app_lwip_tcp_main( beken_thread_arg_t data )
{
    GLOBAL_INT_DECLARATION();
    OSStatus err = kNoErr;
    int maxfd = -1;
    int ret = 0, i = 0;
    int snd_len = 0, rcv_len = 0;
    struct sockaddr_in server_addr;
    socklen_t srvaddr_len = 0;
    fd_set watchfd, watchwd;
    u8 *rcv_buf = NULL;

    (void)(data);  

    LWIP_TCP_FATAL("app_lwip_tcp_main entry\r\n");

    rcv_buf = (u8*) os_malloc((APP_TCP_RCV_BUF_LEN + 1) * sizeof(u8));
    if(!rcv_buf) {
        LWIP_TCP_PRT("tcp os_malloc failed\r\n");
        goto app_tcp_exit;
    }

    tcp_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_server_fd == -1) {
        LWIP_TCP_PRT("socket failed\r\n");
        goto app_tcp_exit;
    }
 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(APP_LWIP_TCP_SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    srvaddr_len = (socklen_t)sizeof(server_addr);
    if (bind(tcp_server_fd, (struct sockaddr *)&server_addr, srvaddr_len) == -1) {
        LWIP_TCP_PRT("bind failed\r\n");
        goto app_tcp_exit;
    }
 
    if (listen(tcp_server_fd, APP_TCP_LISTEN_MAX) == -1) {
        LWIP_TCP_PRT("listen failed\r\n");
        goto app_tcp_exit;
    }

    maxfd = tcp_server_fd;
    for (i=0; i<APP_TCP_LISTEN_MAX; i++) {
        watch_fd_list[i] = -1;
    }

    GLOBAL_INT_DISABLE();
    app_tcp_run = 1;
    GLOBAL_INT_RESTORE();
    
    while ( app_tcp_run )
    {
        FD_ZERO(&watchfd);
        FD_SET(tcp_server_fd, &watchfd);

        for (i=0; i<APP_TCP_LISTEN_MAX; i++) {
            if(watch_fd_list[i] != -1) {
                FD_SET(watch_fd_list[i], &watchfd);
                if (maxfd < watch_fd_list[i]) 
                {
                    maxfd = watch_fd_list[i];
                }
            }
        }        

        ret = select(maxfd+1, &watchfd, NULL, NULL, NULL);
        if (ret <= 0) {
            LWIP_TCP_PRT("select ret:%d\r\n", ret);
            break;
        }
        else 
        {
            // is new connection 
            if (FD_ISSET(tcp_server_fd, &watchfd)) 
            { 
                int new_cli_sockfd = -1;
                struct sockaddr_in client_addr;
                socklen_t cliaddr_len = 0;
            
                cliaddr_len = sizeof(client_addr);
                new_cli_sockfd = accept(tcp_server_fd, (struct sockaddr *)&client_addr, &cliaddr_len);
                if (new_cli_sockfd < 0) { 
                    LWIP_TCP_PRT("accept return fd:%d\r\n", new_cli_sockfd);
                    break;
                }

                LWIP_TCP_PRT("new accept fd:%d\r\n", new_cli_sockfd);

                for (i=0; i<APP_TCP_LISTEN_MAX; i++) {
                    if (watch_fd_list[i] == -1) 
                    {
                        watch_fd_list[i] = new_cli_sockfd;

                        app_lwip_tcp_set_keepalive(new_cli_sockfd);
                        
                        #if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)
                        video_transfer_init(TVIDEO_SND_TCP);
                        #endif
                        app_drone_send_msg(DMSG_APP_CONECTED);
                        break;
                    }
                }

                if(i == APP_TCP_LISTEN_MAX) {
                    LWIP_TCP_PRT("only accept %d clients\r\n", APP_TCP_LISTEN_MAX);
                    close(new_cli_sockfd);
                } 
            }

            // search those added fd  
            for (i=0; i<APP_TCP_LISTEN_MAX; i++) 
            { 
                if (watch_fd_list[i] == -1) {
                    continue;
                }
                if (!FD_ISSET(watch_fd_list[i], &watchfd)) {
                    continue;
                }

                rcv_len = recv(watch_fd_list[i], rcv_buf, APP_TCP_RCV_BUF_LEN, 0);
                if(rcv_len <= 0) 
                {
                    int j;
                    // close this socket
                    LWIP_TCP_PRT("recv close fd:%d\r\n", watch_fd_list[i]);
                    close(watch_fd_list[i]);
                    watch_fd_list[i] = -1;

                    for (j=0; j<APP_TCP_LISTEN_MAX; j++) {
                        if (watch_fd_list[j] != -1) 
                        {
                            break;
                        }
                    }
                    if(j == APP_TCP_LISTEN_MAX) {
                        LWIP_TCP_PRT("not client left, close spidma\r\n");
                        
                        #if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)
                        video_transfer_deinit();
                        #endif
                        app_drone_send_msg(DMSG_APP_DISCONECTED);
                    } 
                } 
                else 
                {
                    rcv_len = (rcv_len > APP_TCP_RCV_BUF_LEN)? APP_TCP_RCV_BUF_LEN: rcv_len;
                    rcv_buf[rcv_len] = 0;

                    snd_len = send(watch_fd_list[i], rcv_buf, rcv_len, 0);
                    if (snd_len < 0) {
                        /* err */
                        LWIP_TCP_PRT("send return fd:%d\r\n", snd_len);
                    }
                }
                FD_CLR(watch_fd_list[i], &watchfd);
            }
        }// ret = select
    }
	
app_tcp_exit:
    
    LWIP_TCP_FATAL("app_lwip_tcp_main exit\r\n");

#if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)
    video_transfer_deinit();
#endif
    //app_drone_send_msg(DRONE_TCP_DISCONECTED);

    if(rcv_buf) {
        os_free(rcv_buf);
        rcv_buf = NULL;
    }

    for (i=0; i<APP_TCP_LISTEN_MAX; i++) {
        if(watch_fd_list[i] != -1) 
        {
            close(watch_fd_list[i]);
            watch_fd_list[i] = -1;
        }
    }

    if(tcp_server_fd != -1) {
        close(tcp_server_fd);
        tcp_server_fd = -1;
    }

    GLOBAL_INT_DISABLE();
    app_tcp_run = 0;
    GLOBAL_INT_RESTORE();

    app_tcp_hdl = NULL;
    rtos_delete_thread(NULL);
}

UINT32 app_lwip_tcp_init(void)
{
    int ret;

    LWIP_TCP_PRT("app_lwip_tcp_init\r\n");
    if(!app_tcp_hdl)
    {
        ret = rtos_create_thread(&app_tcp_hdl,
                                      4,
                                      "app_tcp",
                                      (beken_thread_function_t)app_lwip_tcp_main,
                                      1024,
                                      (beken_thread_arg_t)NULL);
        if (ret != kNoErr)
        {
            LWIP_TCP_PRT("Error: Failed to create spidma_intfer: %d\r\n", ret);
            return kGeneralErr;
        }
    }

    return kNoErr;
}

void app_lwip_tcp_deinit(void)
{
    GLOBAL_INT_DECLARATION();

    LWIP_TCP_PRT("app_lwip_tcp_deinit\r\n");
    if(app_tcp_run == 0)
        return;

    GLOBAL_INT_DISABLE();
    app_tcp_run = 0;
    GLOBAL_INT_RESTORE();
}

/*---------------------------------------------------------------------------*/
int app_lwip_tcp_send_packet(UINT8 *data, UINT32 len)
{
    int i = 0, snd_len = 0;

    if((!app_tcp_hdl) || (tcp_server_fd == -1))
        return 0;

    
    for (i=0; i<APP_TCP_LISTEN_MAX; i++) 
    { 
        if (watch_fd_list[i] == -1) {
            continue;
        }

        snd_len = send(watch_fd_list[i], data, len, MSG_DONTWAIT|MSG_MORE);
        if (snd_len < 0) {
            /* err */
            //LWIP_TCP_PRT("send return fd:%d\r\n", snd_len);
            snd_len = 0;
        }
    }

    return snd_len;
}
/*---------------------------------------------------------------------------*/
#endif  // CFG_SUPPORT_TIANZHIHENG_DRONE
#endif  // CFG_USE_LWIP_NETSTACK

// EOF
