
#include "time.h"
//#include "k_sys.h"
//#include "time_svc.h"
#include "lwip/sockets.h"
#include "lwip/debug.h"
#include <stdio.h>
#include "lwip/netdb.h"
#include <string.h>
#include "lwip/api.h"
//#include "ftp.h"
//#include "cmsis_os.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "lwip/dns.h"
#include "lwip/def.h"
#include "string.h"
//extern tick_t  g_tick_count;

//#include "Custom_UsrCallBack.h"

#define BUFSIZE 300
char data_buf[BUFSIZE];

#define SIZE_TYPE 64
#define SIZE_FILE 64

#define HEAD_INFO       "http://"

#define HEAD_INFO_S     "https://"

struct resp_header 
{
    int status_code;
    char content_type[SIZE_TYPE];
    long content_length;
    char file_name[SIZE_FILE];
};

void parse_url(const char *url, char *domain, int *port, char *file_name)
{
   
    int j = 0;
    int start = 0;
    int flag = 0;
    *port = 80;
    char *patterns[] = {HEAD_INFO, HEAD_INFO_S, NULL};
    int i;

    for ( i = 0; patterns[i]; i++)
        if (strncmp(url, patterns[i], strlen(patterns[i])) == 0)
            start = strlen(patterns[i]);


    for ( i = start; url[i] != '/' && url[i] != '\0'; i++, j++)
        domain[j] = url[i];
    domain[j] = '\0';


    char *pos = strstr(domain, ":");
    if (pos)
        sscanf(pos, ":%d", port);



    for ( i = 0; i < (int)strlen(domain); i++)
    {
        if (domain[i] == ':')
        {
            domain[i] = '\0';
            break;
        }
    }

    j = 0;
    for ( i = start; url[i] != '\0'; i++)
    {
//        if (url[i] == '/')
//        {
//            if (i !=  strlen(url) - 1)
//                j = 0;
//            continue;
//        }
//        else
//            file_name[j++] = url[i];
          if (url[i] == '/' && flag == 0)
          {
              j = 0;
              flag = 1;
              continue;
              
          }
          if(flag == 1)
          {
              file_name[j++] = url[i];
          }
    }
    file_name[j] = '\0';
}

struct resp_header get_resp_header(const char *response)
{

    struct resp_header resp;
   char *pos = strstr(response, "HTTP/");
    if (pos)
        sscanf(pos, "%*s %d", &resp.status_code);

    pos = strstr(response, "Content-Type:");
    if (pos)
        sscanf(pos, "%*s %s", resp.content_type);

    pos = strstr(response, "Content-Length:");
    if (pos)
        sscanf(pos, "%*s %ld", &resp.content_length);

    return resp;
}

u32_t get_time_in_secs()
{
    return (u32_t)(aos_now_ms()/1000); // get time in ms, change it in second.
}

struct lwip_gethostbyname_r_helper
{

    ip_addr_t *addr_list[2];

    ip_addr_t addr;

    char *aliases;
} lwip_gethostbyname_r_helper_t;

#define MEM_ALIGN 4

struct hostent *lwip_gethostbyname1(char *hostname)

{
    static struct hostent ret;
    struct hostent *result = &ret;
    int h_errnop;

    char *buf;
    size_t buflen = sizeof(struct lwip_gethostbyname_r_helper) + strlen(hostname) + 1 + (MEM_ALIGN - 1);

    buf = malloc(buflen);
    if(buf == NULL){
        while(1);
    }
    lwip_gethostbyname_r(hostname, &ret,buf/*(char *)(&lwip_gethostbyname_r_helper_t)*/, buflen, &result, &h_errnop);
    return result;
}

//#define FILE_URL "http://forspeed.onlinedown.net/down/MicrosoftOfficeV3.3.2.13.X86.zh_CN_InstallationComponent_00002000.exe"
#define FILE_URL "http://download.skycn.com/hao123-soft-online-bcs/soft/2017_02_22_BaiduPlayer5Setup_5.exe"
#define MTU_SIZE 3000
#define MAX_LEN 1024*1024*1024
#define TEST_TIMES 200
int get_file_with_url( char *path) {
	
	int cfd;
	struct sockaddr_in cadd;
	struct hostent *pURL = NULL;
	struct resp_header resp = {0};//
	char host[36] = {0};
    int port = 0;
	int len = 0;
    int total_rcv_len = 0;
    //int speed=0, speed_sec=0;
    int curr_rcv_len =0;
	//int length = 0;
    //int pos=0;
	char read_buf[MTU_SIZE] = {0};
    uint32_t sec =0;
    //int failed_count = 0;
	if (-1 == (cfd = socket(AF_INET, SOCK_STREAM, 0))) {
		LWIP_DEBUGF(NETIF_DEBUG,("%16.16s %4d socket error\r\n", __func__, __LINE__));
        printf("%16.16s %4d socket error\r\n", __func__, __LINE__);
		return -1;
	}
    parse_url(path, host, &port, resp.file_name);
	pURL = lwip_gethostbyname(host);
	memset(&cadd, 0,sizeof(struct sockaddr_in));
	cadd.sin_family = AF_INET;
	cadd.sin_addr.s_addr = *((unsigned long*) pURL->h_addr_list[0]);
	cadd.sin_port = PP_HTONS(port);
	//
	memset(data_buf, 0, BUFSIZE);
	strcat(data_buf, "GET /");
	strcat(data_buf, resp.file_name);
	strcat(data_buf, " HTTP/1.1\r\n");
	strcat(data_buf, "HOST: ");
	strcat(data_buf, host);
	strcat(data_buf, "\r\n");
	strcat(data_buf, "Cache-Control: no-cache\r\n\r\n");
    LWIP_DEBUGF(NETIF_DEBUG,("CMD-> %s\n",resp.file_name));
	LWIP_DEBUGF(NETIF_DEBUG,("CMD-> %s\n",data_buf));
	int cc;
	if (-1 == (cc = connect(cfd, (struct sockaddr*) &cadd,
		(socklen_t) sizeof(cadd)))) {
        close(cfd);
        LWIP_DEBUGF(NETIF_DEBUG,("\tconnect failed \r\n"));
        printf("\tconnect failed \r\n");
		return -1;
	}
    LWIP_DEBUGF(NETIF_DEBUG,("\t%16.16s %4d connect resp code = %d\r\n", __func__, __LINE__, cc));
    printf("\t%16.16s %4d connect resp code = %d\r\n", __func__, __LINE__, cc);

	int cs;
	if (-1 == (cs = send(cfd, data_buf, strlen(data_buf), 0))) {
		LWIP_DEBUGF(NETIF_DEBUG,("\t%16.16s %4d send err \r\n", __func__, __LINE__));
        close(cfd);
		return -1;
	}
    LWIP_DEBUGF(NETIF_DEBUG,("\t%16.16s %4d send resp code = %d\r\n", __func__, __LINE__, cs));
    printf("\t%16.16s %4d send resp code = %d\r\n", __func__, __LINE__, cs);
    sec = get_time_in_secs();
    //speed_sec = get_time_in_secs();
    curr_rcv_len = 0;
	memset(data_buf,0,sizeof(data_buf));
    //aos_msleep(1000);
    while (1)
    {
        //LWIP_DEBUGF(NETIF_DEBUG, ("\n\n\n\t\t\t ------------ \n\n\n"));
        len = recv(cfd, read_buf, MTU_SIZE,0);

        total_rcv_len += len;
        curr_rcv_len += len;
        LWIP_DEBUGF(NETIF_DEBUG, ("\tAlready received bytes: %d\n", total_rcv_len));
        
        if( len >0 )
        {
            //pos = len>100?100:len;
            //read_buf[pos] = '\0'; // only printf limited bytes of received content.
            LWIP_DEBUGF(NETIF_DEBUG, ("\tCurrent recv Msg len = %d\n", len));
            /*if(curr_rcv_len > 5*1024*1024)
            {
                printf("\tAlready received bytes: %d\n", total_rcv_len);
                curr_rcv_len = 0;
            }*/
            printf("\tCurrent recv Msg len = %d\n", len);
            printf("\tAlready received bytes: %d\n", total_rcv_len);
            //LWIP_DEBUGF(NETIF_DEBUG, ("Current recv Msg data = %s ...\n", read_buf));
            sec = get_time_in_secs();
        }
        else{
            LWIP_DEBUGF(NETIF_DEBUG,("\tReceive failed from remote server!\n"));
            //printf("\tReceive failed from remote server! Retry in next 20 seconds!\n");
            printf("Reach the end of file!\n");
            break;
        }
        
        #if 0
    	if(len > 0)
    	{
    		data_buf[length++] = read_buf[0];
    	}
        
        if(length > sizeof(data_buf))
        {
            LWIP_DEBUGF(NETIF_DEBUG,("find head err"));
            close(cfd);
            return -1;
        }
        int flag = 0;
        for (int i = strlen(data_buf) - 1; data_buf[i] == '\n' || data_buf[i] == '\r'; i--, flag++);
        if (flag == 4)
        {
            LWIP_DEBUGF(NETIF_DEBUG,("time break %s",data_buf));
            break;
        }
        #endif
        
        /*if(len < 2048)
        {
            LWIP_DEBUGF(NETIF_DEBUG,("Reach the end of file!\n"));
            break;
        }*/
        #if 0
        if(curr_rcv_len >= 5*1024*1024)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("\tFinally received file size: %dM %dK %dB \n", (int)(total_rcv_len/1024/1024),(int)(total_rcv_len/1024%1024),(int)(total_rcv_len%1024)));
            printf("\tFinally received file size: %dM %dK %dB \n", (int)(total_rcv_len/1024/1024),(int)(total_rcv_len/1024%1024),(int)(total_rcv_len%1024));
            speed = curr_rcv_len/(get_time_in_secs() -speed_sec);
            printf("\t download speed: %.2dM\n", speed/1024/1024);
            curr_rcv_len = 0;
            speed_sec = get_time_in_secs();
        }
        #endif
        //LWIP_DEBUGF(NETIF_DEBUG, ("\n\n\n\t\t\t ------------ \n\n\n"));
        #if 0
        if(get_time_in_secs() - speed_sec > 10)
        {
            //speed = (int)((total_rcv_len - curr_rcv_len)/(get_time_in_secs() -speed_sec));
            //LWIP_DEBUGF(NETIF_DEBUG,("\t download speed: %.2dM\n", speed/1024/1024));
            //printf("\t download speed: %.2dM\n", speed/1024/1024);
            printf("\t get_time_in_secs() \n", get_time_in_secs());
            printf("\t speed_sec \n", speed_sec);
            printf("\t get_time_in_secs() - speed_sec \n", get_time_in_secs() - speed_sec );
            printf("\t curr_rcv_len \n", curr_rcv_len);
            curr_rcv_len = total_rcv_len; // restart the calculation.
            speed_sec = get_time_in_secs();
        } 
        #endif
        if(get_time_in_secs() - sec > 20) // already no data for 20 seconds
        {
            LWIP_DEBUGF(NETIF_DEBUG,("\t Download time out for about 20 seconds.\n"));
            printf("\t Download time out for about 20 seconds.\n");
            printf("\t Current time: %d\n", get_time_in_secs());
            printf("\t Latest time: %d\n", sec);
            break;//return -1;
        }
    }
    
    #if 0
    memset(resp.file_name,0,sizeof(resp.file_name));
    resp = get_resp_header(data_buf);
	
	strcpy(resp.file_name, resp.file_name);
    
	LWIP_DEBUGF(NETIF_DEBUG,("res file code %d len %d",resp.status_code, resp.content_length));
    #endif
    //return Write_Updata_File_CallBack(cfd,data_buf,sizeof(data_buf),resp.content_length);
    //aos_msleep(5000);
    return 0;
}

void http_get_file(){
    int i = 0;
    printf("entry http_get_file.\n");
    while(i < TEST_TIMES)
    {
        get_file_with_url(FILE_URL);
        i++;
        LWIP_DEBUGF(NETIF_DEBUG, ("\n\n\n\t\t\t ------download round %d------ \n\n\n", i));
        printf("\n\n\n\t\t\t ------download round %d------ \n\n\n", i);
    }
}

//static const struct cli_command http_client_cmds[] = { 
//	{"http_get_file","start to get http file: http_get_file",http_get_file},
////	{"stc","stop tcp client: stc",stop_tcp_client},
////  {"sttc","start tcp client: sttc",start_tcp_client},
//};

//void http_client_debug_init(void){
//    aos_cli_register_commands(&http_client_cmds[0],sizeof(http_client_cmds)/sizeof(http_client_cmds[0]));
//    printf("http_client_debug_init\n");
//}



