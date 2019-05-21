#include "include.h"

#if CFG_USE_LWIP_NETSTACK
#include "FreeRTOS.h"
#include "task.h"
#include "rtos_pub.h"
#include "error.h"
#include "lwip/sockets.h"

#include "app_lwip_udp.h"
#include "BkDriverUart.h"
#include "app_led.h"
#include "uart_pub.h"
#include "mem_pub.h"

#include "app_drone.h"
#include "video_transfer.h"


#if CFG_SUPPORT_TIANZHIHENG_DRONE
#define LWIP_UDP_SERVICER       1

#define CMD_HEADER_CODE         0x66
#define CMD_TAIL_CODE           0x99
#define CMD_LEN                 8

#define CMD_IMG_HEADER          0x42
#define CMD_START_IMG           0x76
#define CMD_STOP_IMG            0x77

#define LWIP_UDP_DEBUG          1
#if LWIP_UDP_DEBUG
#define LWIP_UDP_PRT            warning_prf
#define LWIP_UDP_WARN           warning_prf
#define LWIP_UDP_FATAL          fatal_prf
#else
#define LWIP_UDP_PRT            null_prf
#define LWIP_UDP_WARN           null_prf
#define LWIP_UDP_FATAL          null_prf
#endif

#define APP_LWIP_UDP_IMG_PORT           8080 
#define APP_LWIP_UDP_CMD_PORT           8090 
#define APP_UDP_RCV_BUF_LEN             1472
#define APP_UDP_SOCKET_TIMEOUT          100  // ms

int udp_img_fd = -1;
volatile int udp_romote_connected = 0;
volatile int app_udp_run = 0;
xTaskHandle app_upd_hdl = NULL;
struct sockaddr_in *udp_remote = NULL;


static void app_lwip_udp_handle_cmd_data(UINT8 *data, UINT16 len)
{
    uint8_t crc_cal;
    uint32_t wrlen = 0;
    
    if((data[0] != 0x66) && (len != CMD_LEN))
        return;

    crc_cal = (data[1]^data[2]^data[3]^data[4]^data[5]);
    
    if(crc_cal != data[6]) {
        if(((crc_cal == CMD_HEADER_CODE) || (crc_cal == CMD_TAIL_CODE)) 
            && (crc_cal+1 == data[6]))
            // drop this paket for crc is the same with Header or Tailer
            return;
        else // change to right crc
            data[6] = crc_cal;
    }

    bk_uart_send(1, data, len);
        
}

static void app_lwip_udp_receiver(UINT8 *data, UINT32 len, struct sockaddr_in *udp_remote)
{
    GLOBAL_INT_DECLARATION();
    
    if(len < 2)
        return;
    
    if(data[0] == CMD_IMG_HEADER) {
        if(data[1] == CMD_START_IMG) {
 
            UINT8 *src_ipaddr = (char *)&udp_remote->sin_addr.s_addr;
            LWIP_UDP_PRT("src_ipaddr: %d.%d.%d.%d\r\n", src_ipaddr[0], src_ipaddr[1],
                                                   src_ipaddr[2], src_ipaddr[3]);
            LWIP_UDP_PRT("udp connect to new port:%d\r\n", udp_remote->sin_port);

            GLOBAL_INT_DISABLE();
            udp_romote_connected = 1;
            GLOBAL_INT_RESTORE();    

            #if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)  
            video_transfer_init(TVIDEO_SND_UDP);
            #endif

            app_drone_send_msg(DMSG_APP_CONECTED);
        }
        else if(data[1] == CMD_STOP_IMG) {
            LWIP_UDP_PRT("udp close\r\n");

            #if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)  
            video_transfer_deinit();               
            #endif

            app_drone_send_msg(DMSG_APP_DISCONECTED);

            GLOBAL_INT_DISABLE();
            udp_romote_connected = 0;
            GLOBAL_INT_RESTORE(); 

        }
    }
}

static void app_lwip_udp_main( beken_thread_arg_t data )
{
    OSStatus err = kNoErr;
    GLOBAL_INT_DECLARATION();
    int maxfd, udp_cmd_fd, ret = 0;
    int snd_len = 0, rcv_len = 0;
    struct sockaddr_in cmd_remote;
    socklen_t srvaddr_len = 0;
    fd_set watchfd;
    struct timeval timeout;
    u8 *rcv_buf = NULL;

    (void)(data);

    rcv_buf = (u8*) os_malloc((APP_UDP_RCV_BUF_LEN + 1) * sizeof(u8));
    if(!rcv_buf) {
        LWIP_UDP_PRT("udp os_malloc failed\r\n");
        goto app_udp_exit;
    }

    udp_remote = (struct sockaddr_in *)os_malloc(sizeof(struct sockaddr_in));
    if(!udp_remote) {
        LWIP_UDP_PRT("udp os_malloc failed\r\n");
        goto app_udp_exit;
    }    

    // for data transfer
    udp_img_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_img_fd == -1) {
        LWIP_UDP_PRT("socket failed\r\n");
        goto app_udp_exit;
    }
 
    udp_remote->sin_family = AF_INET;
    udp_remote->sin_port = htons(APP_LWIP_UDP_IMG_PORT);
    udp_remote->sin_addr.s_addr = htonl(INADDR_ANY);

    srvaddr_len = (socklen_t)sizeof(struct sockaddr_in);
    if (bind(udp_img_fd, (struct sockaddr *)udp_remote, srvaddr_len) == -1) {
        LWIP_UDP_PRT("bind failed\r\n");
        goto app_udp_exit;
    }

    //  for recv uart cmd 
    udp_cmd_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_cmd_fd == -1) {
        LWIP_UDP_PRT("socket failed\r\n");
        goto app_udp_exit;
    }
 
    cmd_remote.sin_family = AF_INET;
    cmd_remote.sin_port = htons(APP_LWIP_UDP_CMD_PORT);
    cmd_remote.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(udp_cmd_fd, (struct sockaddr *)&cmd_remote, srvaddr_len) == -1) {
        LWIP_UDP_PRT("bind failed\r\n");
        goto app_udp_exit;
    }

    maxfd = (udp_cmd_fd > udp_img_fd)? udp_cmd_fd : udp_img_fd;

    timeout.tv_sec = APP_UDP_SOCKET_TIMEOUT / 1000;
    timeout.tv_usec = (APP_UDP_SOCKET_TIMEOUT % 1000) * 1000;

    GLOBAL_INT_DISABLE();
    udp_romote_connected = 0;
    app_udp_run = 1;
    GLOBAL_INT_RESTORE(); 

    while (app_udp_run)
    {    
        FD_ZERO(&watchfd);
        FD_SET(udp_img_fd, &watchfd);
        FD_SET(udp_cmd_fd, &watchfd);
        
        ret = select(maxfd+1, &watchfd, NULL, NULL, &timeout);
        if (ret < 0) {
            LWIP_UDP_PRT("select ret:%d\r\n", ret);
            break;
        } 
        else 
        {
            // is img fd, data transfer
            if(FD_ISSET(udp_img_fd, &watchfd)) 
            { 
                rcv_len = recvfrom(udp_img_fd, rcv_buf, APP_UDP_RCV_BUF_LEN, 0,
                    (struct sockaddr *)udp_remote, &srvaddr_len);
                
                if(rcv_len <= 0) 
                {
                    // close this socket
                    LWIP_UDP_PRT("recv close fd:%d\r\n", udp_img_fd);
                    break;
                } 
                else 
                {
                    rcv_len = (rcv_len > APP_UDP_RCV_BUF_LEN)? APP_UDP_RCV_BUF_LEN: rcv_len;
                    rcv_buf[rcv_len] = 0;

                    app_lwip_udp_receiver(rcv_buf, rcv_len, udp_remote);
                }
            }
            else if(FD_ISSET(udp_cmd_fd, &watchfd)) 
            {
                rcv_len = recvfrom(udp_cmd_fd, rcv_buf, APP_UDP_RCV_BUF_LEN, 0,
                    (struct sockaddr *)&cmd_remote, &srvaddr_len);
                
                if(rcv_len <= 0) 
                {
                    // close this socket
                    LWIP_UDP_PRT("recv close fd:%d\r\n", udp_cmd_fd);
                    break;
                } 
                else 
                {
                    rcv_len = (rcv_len > APP_UDP_RCV_BUF_LEN)? APP_UDP_RCV_BUF_LEN: rcv_len;
                    rcv_buf[rcv_len] = 0;
                    
                    app_lwip_udp_handle_cmd_data(rcv_buf, rcv_len);
                }

            }

        }
    }
	
app_udp_exit:
    
    LWIP_UDP_FATAL("app_lwip_udp_main exit %d\r\n", app_udp_run);

#if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)
    video_transfer_deinit();
#endif

    if(rcv_buf) {
        os_free(rcv_buf);
        rcv_buf = NULL;
    }

    if(udp_remote) {
        os_free(udp_remote);
        udp_remote = NULL;
    }
    
    if(udp_img_fd != -1) {
        close(udp_img_fd);
        udp_img_fd = -1;
    }

    if(udp_cmd_fd != -1) {
        close(udp_cmd_fd);
        udp_cmd_fd = -1;
    }    

    GLOBAL_INT_DISABLE();
    udp_romote_connected = 0;
    app_udp_run = 0;
    GLOBAL_INT_RESTORE(); 

    app_upd_hdl = NULL;
    rtos_delete_thread(NULL);
}

UINT32 app_lwip_udp_init(void)
{
    int ret;

    LWIP_UDP_PRT("app_lwip_udp_init\r\n");
    if(!app_upd_hdl)
    {
        ret = rtos_create_thread(&app_upd_hdl,
                                      4,
                                      "app_udp",
                                      (beken_thread_function_t)app_lwip_udp_main,
                                      1024,
                                      (beken_thread_arg_t)NULL);
        if (ret != kNoErr)
        {
            LWIP_UDP_PRT("Error: Failed to create spidma_intfer: %d\r\n", ret);
            return kGeneralErr;
        }
    }

    return kNoErr;
}

int app_lwip_udp_send_packet (UINT8 *data, UINT32 len)
{
    int send_byte = 0;

    if(!udp_romote_connected)
        return 0;

    send_byte = sendto(udp_img_fd, data, len, MSG_DONTWAIT|MSG_MORE,
        (struct sockaddr *)udp_remote, sizeof(struct sockaddr_in));
    
    if (send_byte < 0) {
        /* err */
        //LWIP_UDP_PRT("send return fd:%d\r\n", send_byte);
        send_byte = 0;
    }

	return send_byte;
}

void app_lwip_udp_deinit(void)
{
    GLOBAL_INT_DECLARATION();

    LWIP_UDP_PRT("app_lwip_udp_deinit\r\n");
    if(app_udp_run == 0)
        return;

    GLOBAL_INT_DISABLE();
    app_udp_run = 0;
    GLOBAL_INT_RESTORE(); 
}

#endif  // CFG_SUPPORT_TIANZHIHENG_DRONE
#endif  // CFG_USE_LWIP_NETSTACK

// EOF

