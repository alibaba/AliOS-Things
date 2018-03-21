#include <platform_opts.h>
#include "FreeRTOS.h"
#include "task.h"
#include "lwip/netdb.h"
#include "http_client.h"
#include "http_client/example_http_client.h"
#include <platform/platform_stdlib.h>

#define THREAD_STACK_SIZE 1024
#define HOST_NAME "www.google.com.tw"
#define HOST_PORT 80

extern char *http_post_header(char *host, char *resource, char *type, int data_len);
extern char *http_get_header(char *host, char *resource);

void http_client(void)
{
    int port = HOST_PORT;
    char *host = HOST_NAME;
    char *message_fmt = "POST / HTTP/1.0\r\n\r\n"; 
    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes;
    char message[256],*response;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        printf("[ERROR] Create socket failed\n");
    
    server = gethostbyname(host);
    if(server == NULL) 
        printf("[ERROR] Get host ip failed\n");
    
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);
    
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){
        printf("[ERROR] connect failed\n");
        return ;
    }
    
    //send request
    sprintf(message,"%s",http_get_header(HOST_NAME,"/"));
    printf("\nRequest:\n%s\n",message);
    bytes = write(sockfd,message,256);
    if (bytes < 0)
        printf("[ERROR] send packet failed\n");
    
    //receive response
    response = malloc(1500);
    if(response ==NULL)
        printf("[ERROR] malloc failed\n");
    printf("Response:\n");
    do {
        memset(response,0,1500);
        bytes = read(sockfd,response,1500-1);
        if (bytes < 0)
            printf("[ERROR] receive packet failed\n");
        if (bytes == 0)
            break;
        printf("%s",response);
    } while (bytes > 0);

    //close the socket
    close(sockfd);

    return;
}

static void http_client_thread(void *param)
{
    //wait for wifi connected, a rough time
    vTaskDelay(10000);
    
    http_client();
    vTaskDelete(NULL);
}

void example_http_client(void)
{
    if(xTaskCreate(http_client_thread, ((const char*)"http_client_thread"), THREAD_STACK_SIZE, NULL, tskIDLE_PRIORITY , NULL) != pdPASS)
        printf("\n\r%s xTaskCreate(http_client_thread) failed\n", __FUNCTION__);
    return;
}