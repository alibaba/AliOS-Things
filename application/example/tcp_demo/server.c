#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <netdb.h> 
#include <sys/socket.h> 


extern void communication(int fd);

int main() 
{ 
    int serverfd, clientfd, len; 
    struct sockaddr_in servaddr, clientaddr; 
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&clientaddr, 0, sizeof(clientaddr)); 

    serverfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (serverfd == -1) { 
        printf("socket called failed!\n"); 
        exit(0); 
    } 
    else {
        printf("socket successfully created..\n"); 
    }

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(8080); 

    //绑定server ip地址
    if ((bind(serverfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else {
        printf("Socket successfully binded..\n"); 
    }

    if ((listen(serverfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(clientaddr); 

    //等待设备连接
    clientfd = accept(serverfd, (struct sockaddr *)&clientaddr, &len); 
    if (clientfd < 0) { 
        printf("acccept failed!\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n"); 

    //主循环交流
    communication(clientfd); 
    close(serverfd); 
} 

#define MAX_INPUT_CHAR 100 
void communication(int fd) 
{ 
    int index; 
    char input[MAX_INPUT_CHAR]; 

    while (1) { 
        memset(input, 0, MAX_INPUT_CHAR); 
        index = 0; 

        //等待client发送消息
        read(fd, input, sizeof(input)); 
        printf("Receive from client: %s\n", input); 
        printf("Pluse input sending to client : ");
        memset(input, 0, MAX_INPUT_CHAR); 
        while ((input[index++] = getchar()) != '\n') 
            ; 

        //将input发送给client
        write(fd, input, sizeof(input)); 

        if (strncmp("exit", input, 4) == 0) { 
            printf("Exit communication\n"); 
            break; 
        } 
    } 
} 
