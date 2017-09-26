/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <lwip/sockets.h> /* 使用BSD socket，需要包含sockets.h头文件 */

#define BUFSZ	1024


/*
void  UDPDataRecv(void *arg, struct udp_pcb *upcb, struct pbuf *p,
                 struct ip_addr *addr, u16_t port)
{
  // char *a;
// uint8_t i=0;
   struct pbuf *q;
  struct ip_addr destAddr = *addr;
  if(p != NULL)                          
  {
      for(q=p; q != NULL; q = p->next)
    {
   //   c = q->payload;
   //  a=q->payload;
    //pbuf_copy_partial(p, UDPData, p->len, 0);
    udp_sendto(upcb,q->payload,&destAddr,5000);          
    }                  
  }
  //pbuf_free(p);
}
*/
/*
void UDPNetInit(void)
{
   struct udp_pcb * UDPpcb;
     struct ip_addr ipaddr1;
     struct ip_addr netmask;
      struct ip_addr gw;
     IP4_ADDR(&ipaddr1,192,168,1,125);
     IP4_ADDR(&netmask, 255, 255, 255, 0);                //子网掩码
     IP4_ADDR(&gw, 192, 168, 1, 1);                //网关
    //
    // Start listening for incoming TFTP requests.
    //
    UDPpcb = udp_new();
//   UDPpcb->so_options |= SOF_BROADCAST;
    udp_bind(UDPpcb,IP_ADDR_ANY,5000);   
    udp_connect(UDPpcb,IP_ADDR_BROADCAST,4000); 
    udp_recv(UDPpcb, UDPDataRecv, NULL);
}
*/



void udpserv(void* paramemter)
{
   int sock;
   int bytes_read;
   char *recv_data;
   uint32_t addr_len;
   struct sockaddr_in server_addr, client_addr;

   /* 分配接收用的数据缓冲 */
   recv_data = malloc(BUFSZ);
   if (recv_data == NULL)
   {
       /* 分配内存失败，返回 */
       printf("No memory\n");
       return;
   }

   /* 创建一个socket，类型是SOCK_DGRAM，UDP类型 */
   if ((sock = lwip_socket(AF_INET, SOCK_DGRAM, 0)) == -1)
   {
       printf("Socket error\n");

       /* 释放接收用的数据缓冲 */
       free(recv_data);
       return;
   }

   /* 初始化服务端地址 */
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = lwip_htons(5000);
   server_addr.sin_addr.s_addr = INADDR_ANY;
   memset(&(server_addr.sin_zero),0, sizeof(server_addr.sin_zero));


   /* 绑定socket到服务端地址 */
   if (lwip_bind(sock,(struct sockaddr *)&server_addr,
            sizeof(struct sockaddr)) == -1)
   {
       /* 绑定地址失败 */
       printf("Bind error\n");

       /* 释放接收用的数据缓冲 */
       free(recv_data);
       return;
   }

   addr_len = sizeof(struct sockaddr);
   printf("UDPServer Waiting for client on port 5000...\n");

   while (1)
   {
       /* 从sock中收取最大BUFSZ - 1字节数据 */
       bytes_read = lwip_recvfrom(sock, recv_data, BUFSZ - 1, 0,
                             (struct sockaddr *)&client_addr, &addr_len);
       /* UDP不同于TCP，它基本不会出现收取的数据失败的情况，除非设置了超时等待 */

       recv_data[bytes_read] = '\0'; /* 把末端清零 */

       /* 输出接收的数据 */
       printf("\n(%s , %d) said : ",inet_ntoa(client_addr.sin_addr),
                  ntohs(client_addr.sin_port));
       printf("%s", recv_data);

       /* 如果接收数据是exit，退出 */
       if (strcmp(recv_data, "exit") == 0)
       {
           lwip_close(sock);

           /* 释放接收用的数据缓冲 */
           free(recv_data);
           break;
       }
   }

   return;
}
