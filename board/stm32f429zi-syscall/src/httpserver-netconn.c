/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Netconn_RTOS/Src/httpser-netconn.c 
  * @author  MCD Application Team
  * @brief   Basic http server implementation using LwIP netconn API  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "lwip/apps/fs.h"
#include "string.h"
#include "httpserver-netconn.h"
#include <stdio.h>
#include <k_api.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define WEBSERVER_THREAD_PRIO    ( osPriorityAboveNormal )

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32_t nPageHits = 0;
#define HTTP_SERVER_TASK_STACKSIZE    1024 //256*cpu_stack_t = 1024byte
#define HTTP_SERVER_TASK_PRIORITY     20

extern uint32_t g_stlink_baudrate;
extern void stm32_soc_init(void);
static ktask_t http_srv_task_obj;
cpu_stack_t http_srv_task_buf[HTTP_SERVER_TASK_STACKSIZE];

uint32_t parse_number(uint8_t **string, uint8_t key, uint8_t number_base)
{
    /*temporal pointers*/
    char *start;

    start = (char *)strchr(*string, key);        
    return strtoul((start+1), string, number_base);  
}


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * Send page not found error to HTTP connection
 *
 * @param connection descriptor
 * @return none
 */
inline static void http_transmit_err(struct netconn *conn)
{
    //header
    netconn_write(conn, HTTP_WRONG_HEADER, (size_t)(HTTP_WRONG_HEADER), NETCONN_NOCOPY);
    //data
    netconn_write(conn, HTTP_WRONG_DATA, (size_t)(HTTP_WRONG_DATA), NETCONN_NOCOPY);
}


/**
 * @brief serve tcp connection  
 * @param conn: pointer on connection structure 
 * @retval None
 */
static void http_server_serve(struct netconn *conn) 
{
    struct netbuf *inbuf;
    err_t recv_err;
    char* buf;
    u16_t buflen;
    struct fs_file file;

    recv_err = netconn_recv(conn, &inbuf);

    if (recv_err == ERR_OK)
    {
        if (netconn_err(conn) == ERR_OK) 
        {
            netbuf_data(inbuf, (void**)&buf, &buflen);

            if ((buflen >=5) && (strncmp(buf, HTTP_GET_REQUEST, 5) == 0))
            {
                /* Check if request to get ST.gif */ 
                if ((strncmp((char const *)buf,"GET /login.html",15)==0)||(strncmp(buf, "GET / ", 6) == 0)) 
                {
                    fs_open(&file, "/login.html"); 
                    netconn_write(conn, (const unsigned char*)(file.data), (size_t)file.len, NETCONN_NOCOPY);
                    fs_close(&file);
                }   
                else if(strncmp(buf, "GET /index.html", 15) == 0)
                {
                    /* Load index page */
                    fs_open(&file, "/index.html"); 
                    netconn_write(conn, (const unsigned char*)(file.data), (size_t)file.len, NETCONN_NOCOPY);
                    fs_close(&file);
                }
                else if(strncmp(buf, "GET /configuration.html", 23) == 0) 
                {
                    /* Load index page */
                    fs_open(&file, "/configuration.html"); 
                    netconn_write(conn, (const unsigned char*)(file.data), (size_t)file.len, NETCONN_NOCOPY);
                    fs_close(&file);
                }
                else 
                {
                    /* web page error */
                    http_transmit_err(conn);
                }
            }
            else if ((buflen >=5) && (strncmp(buf, HTTP_POST_REQUEST, 6) == 0))
            {
                /* Check if request to get ST.gif */ 
                if (strncmp((char const *)buf,"POST /uart.cgi",14)==0)
                {
                    uint32_t baud;
                    char* ch;
                    printf(" %s \r\n", buf);
                    if((ch = strstr(buf, "BAUD")) == NULL)
                    {
                        /* web page error */
                        http_transmit_err(conn);
						netbuf_delete(inbuf);
                        return; 								
                    }
                    else
                    {
                        printf(" %s \r\n", ch);
                    }

                    baud = (uint32_t)parse_number((uint8_t **)&ch,'=',10);
                    g_stlink_baudrate = baud;
                    MX_USART3_UART_Init();
                    /* Load index page */
                    fs_open(&file, "/configuration.html"); 
                    netconn_write(conn, (const unsigned char*)(file.data), (size_t)file.len, NETCONN_NOCOPY);
                    fs_close(&file);

                }
            }							
        }
    }
    /* Close the connection (server closes in HTTP) */
    netconn_close(conn);

    /* Delete the buffer (netconn_recv gives us ownership,
       so we have to make sure to deallocate the buffer) */
    netbuf_delete(inbuf);
}


/**
 * @brief  http server thread 
 * @param arg: pointer on argument(not used here) 
 * @retval None
 */
static void http_server_netconn_thread(void *arg)
{ 
    struct netconn *conn, *newconn;
    err_t err, accept_err;

    /* Create a new TCP connection handle */
    conn = netconn_new(NETCONN_TCP);

    if (conn!= NULL)
    {
        /* Bind to port 80 (HTTP) with default IP address */
        err = netconn_bind(conn, NULL, 80);

        if (err == ERR_OK)
        {
            /* Put the connection into LISTEN state */
            netconn_listen(conn);

            while(1) 
            {
                /* accept any icoming connection */
                accept_err = netconn_accept(conn, &newconn);
                if(accept_err == ERR_OK)
                {
                    /* serve connection */
                    http_server_serve(newconn);

                    /* delete connection */
                    netconn_delete(newconn);
                }
            }
        }
    }
}

/**
 * @brief  Initialize the HTTP server (start its thread) 
 * @param  none
 * @retval None
 */
void http_server_netconn_init()
{
    krhino_task_create(&http_srv_task_obj, "http_server_netconn_thread", 0, HTTP_SERVER_TASK_PRIORITY,
            50, http_srv_task_buf, HTTP_SERVER_TASK_STACKSIZE, http_server_netconn_thread, 1);
}

/**
 * @brief  Create and send a dynamic Web Page. This page contains the list of 
 *         running tasks and the number of page hits. 
 * @param  conn pointer on connection structure 
 * @retval None
 */
void DynWebPage(struct netconn *conn)
{
    char PAGE_BODY[512];
    char pagehits[10] = {0};

    /* memset(PAGE_BODY, 0,512); */

}
