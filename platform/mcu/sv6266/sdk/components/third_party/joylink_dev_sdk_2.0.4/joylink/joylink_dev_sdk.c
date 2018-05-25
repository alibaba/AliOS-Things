#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(__MTK_7687__)
#include <stdint.h>
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#elif defined(__ICOMM_6166__)
#include <stdint.h>
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#else
#include <stdarg.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netdb.h>
#endif

#include "joylink.h"
#include "joylink_utils.h"
#include "joylink_packets.h"
#include "joylink_crypt.h"
#include "joylink_json.h"
#include "joylink_dev.h"
#include "joylink_sub_dev.h"

JLDevice_t  _g_dev = {
    .server_socket = -1,
    .server_st = 0,
    .hb_lost_count = 0,
    .lan_socket = -1,
    .local_port = 80
};

JLDevice_t  *_g_pdev = &_g_dev;

extern int
joylink_proc_server_st();

extern void
joylink_proc_lan();

extern void
joylink_proc_server();

extern int 
joylink_ecc_contex_init(void);

/**
 * brief: 
 *
 * @Param: ver
 */
void
joylink_dev_set_ver(short ver)
{
    _g_pdev->jlp.version = ver;
    joylink_dev_set_attr_jlp(&_g_pdev->jlp);
}

/**
 * brief: 
 *
 * @Returns: 
 */
short
joylink_dev_get_ver()
{
    return _g_pdev->jlp.version;
}

extern int
joylink_is_usr_timestamp_ok(char *usr, int timestamp);

void 
joylink_test()
{
    /*int t = time(NULL);*/
    /*int t = 10;*/
    int ttret;
    static int index = 0; 
    char buff[64] = {0};
    
    sprintf(buff, "192.168.8.%d", index);
    ttret = joylink_is_usr_timestamp_ok(buff, index);
    index++;
    printf("----- user:%s\n", buff); 

    if(ttret == 0){
        printf("----- timestamp is invalid\n"); 
    }else{
        printf("----- timestamp is valid\n"); 
    }
}

/**
 * brief: 
 */
void 
joylink_main_loop(void *pdata)
{
    int ret;
	struct sockaddr_in sin;
    bzero(&sin, sizeof(sin));

	struct timeval  selectTimeOut;
	static uint32_t serverTimer;
	static int interval = 0;

	joylink_util_timer_reset(&serverTimer);

	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(_g_pdev->local_port);
	_g_pdev->lan_socket = socket(AF_INET, SOCK_DGRAM, 0);

	if (_g_pdev->lan_socket < 0){
		printf("socket() failed!\n");
		return;
	}

	int broadcastEnable = 1;
	if (setsockopt(_g_pdev->lan_socket, 
                SOL_SOCKET, SO_BROADCAST, 
                (uint8_t *)&broadcastEnable, 
                sizeof(broadcastEnable)) < 0){
		log_error("SO_BROADCAST ERR");
    }

	if(0 > bind(_g_pdev->lan_socket, (SOCKADDR*)&sin, sizeof(SOCKADDR))){
		perror("Bind lan socket error!");
    }
    else
        {
            log_notice("lan socket bind at port %d\n", sin.sin_port);
        }

	while (1){
        /*joylink_test();*/

		if (joylink_util_is_time_out(serverTimer, interval)){
			joylink_util_timer_reset(&serverTimer);
            if(joylink_dev_is_net_ok()){
                interval = joylink_proc_server_st();
            }else{
                log_notice("wait network connected....\n");
                interval = 1000 * 10;
            }
		}

		fd_set  readfds;
		FD_ZERO(&readfds);
		FD_SET(_g_pdev->lan_socket, &readfds);
        
        if (_g_pdev->server_socket != -1 
               && _g_pdev->server_st > 0){
            FD_SET(_g_pdev->server_socket, &readfds);
        }

        int maxFd = (int)_g_pdev->server_socket > 
            (int)_g_pdev->lan_socket ? 
            _g_pdev->server_socket : _g_pdev->lan_socket;

        selectTimeOut.tv_usec = 0L;
        selectTimeOut.tv_sec = (long)1;

		ret = select(maxFd + 1, &readfds, NULL, NULL, &selectTimeOut);
		if (ret < 0){
			printf("Select ERR: %s!\r\n", strerror(errno));
			continue;
		}else if (ret == 0){
			continue;
		}

		if (FD_ISSET(_g_pdev->lan_socket, &readfds)){
            joylink_proc_lan();
		}else if((_g_pdev->server_socket != -1) && 
            (FD_ISSET(_g_pdev->server_socket, &readfds))){
            joylink_proc_server();
		}
	}
#ifdef JD_CLOUD_EN
    OS_TaskDelete(NULL);
#endif
    
}

/**
 * brief: 
 */
static void 
joylink_dev_init()
{
    /**
     *NOTE: If your model_code_flag is configed in cloud, 
     *Please set _g_pdev->model_code_flag = 1. 
     */
    /*_g_pdev->model_code_flag = 1;*/
    joylink_dev_get_jlp_info(&_g_pdev->jlp);
    joylink_dev_get_idt(&_g_pdev->idt);
    log_debug("--->feedid:%s", _g_pdev->jlp.feedid);
    log_debug("--->uuid:%s", _g_pdev->jlp.uuid);
    log_debug("--->accesskey:%s", _g_pdev->jlp.accesskey);
    log_debug("--->localkey:%s", _g_pdev->jlp.localkey);
}

/**
 * brief: 
 *
 * @Returns: 
 */
int 
joylink_main_start()
{
    printf("joylink_main_start()\n");
    joylink_ecc_contex_init();
    joylink_dev_init();

#if defined(__ICOMM_6166__)
    OS_TaskCreate(joylink_main_loop, "joylink main loop", 3072, NULL, OS_TASK_PRIO3, NULL);
#else
    joylink_main_loop();
#endif
	return 0;
}

#ifdef _TEST_
int 
main()
{
    joylink_main_start();
	return 0;
}
#else

#endif
