/**
 ******************************************************************************
 * @file    tcp_activation_client.c
 * @version V1.1.0
 * @date    18-May-2022
 * @brief   tcp activation client using LwIP RAW API
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#if LWIP_TCP
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint8_t message_count = 0;

extern char g_report_http_data[];
extern char g_response_data[];

/* ECHO protocol states */
enum echoclient_states {
    ES_NOT_CONNECTED = 0,
    ES_CONNECTED,
    ES_RECEIVED,
    ES_CLOSING,
};

int g_net_state = ES_NOT_CONNECTED;

/* structure to be passed as argument to the tcp callbacks */
struct activation_client {
    enum echoclient_states state; /* connection status */
    struct tcp_pcb *pcb;          /* pointer on the current tcp_pcb */
    struct pbuf *p_tx;            /* pointer on pbuf to be transmitted */
};

/* Private function prototypes -----------------------------------------------*/
static err_t tcp_activation_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_activation_client_connection_close(struct tcp_pcb *tpcb, struct activation_client *es);
static err_t tcp_activation_client_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_activation_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_activation_client_send(struct tcp_pcb *tpcb, struct activation_client *es);
static err_t tcp_activation_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Connects to the TCP echo server
 * @param  None
 * @retval None
 */
void tcp_activation_client_connect(const ip_addr_t *dest_ipaddr, u16_t dest_port)
{
    /* create new tcp pcb */
    struct tcp_pcb *pcb_activation_client = tcp_new();
    if (pcb_activation_client != NULL) {
        /* connect to destination address/port */
        tcp_connect(pcb_activation_client, dest_ipaddr, dest_port, tcp_activation_client_connected);
    } else {
        /* deallocate the pcb */
        memp_free(MEMP_TCP_PCB, pcb_activation_client);
    }
}

/**
 * @brief Function called when TCP connection established
 * @param tpcb: pointer on the connection contol block
 * @param err: when connection correctly established err should be ERR_OK
 * @retval err_t: returned error
 */
static err_t tcp_activation_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    struct activation_client *es = NULL;

    if (err == ERR_OK) {
        /* allocate structure es to maintain tcp connection informations */
        es = (struct activation_client *)mem_malloc(sizeof(struct activation_client));
        if (es != NULL) {
            es->state = ES_CONNECTED;
            g_net_state = ES_CONNECTED;
            es->pcb = tpcb;

            /* allocate pbuf */
            es->p_tx = pbuf_alloc(PBUF_TRANSPORT, strlen(g_report_http_data), PBUF_POOL);
            if (es->p_tx) {
                /* copy data to pbuf */
                pbuf_take(es->p_tx, g_report_http_data, strlen(g_report_http_data));

                /* pass newly allocated es structure as argument to tpcb */
                tcp_arg(tpcb, es);

                /* initialize LwIP tcp_recv callback function */
                tcp_recv(tpcb, tcp_activation_client_recv);

                /* initialize LwIP tcp_sent callback function */
                tcp_sent(tpcb, tcp_activation_client_sent);

                /* initialize LwIP tcp_poll callback function */
                tcp_poll(tpcb, tcp_activation_client_poll, 1);

                /* send data */
                tcp_activation_client_send(tpcb, es);

                return ERR_OK;
            }
        } else {
            /* close connection */
            tcp_activation_client_connection_close(tpcb, es);

            /* return memory allocation error */
            return ERR_MEM;
        }
    } else {
        /* close connection */
        tcp_activation_client_connection_close(tpcb, es);
    }
    return err;
}

/**
 * @brief tcp_receiv callback
 * @param arg: argument to be passed to receive callback
 * @param tpcb: tcp connection control block
 * @param err: receive error code
 * @retval err_t: retuned error
 */
static err_t tcp_activation_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    struct activation_client *es = (struct activation_client *)arg;
    err_t ret_err;

    LWIP_ASSERT("arg != NULL", arg != NULL);

    /* if we receive an empty tcp frame from server => close connection */
    if (p == NULL) {
        /* remote host closed connection */
        es->state = ES_CLOSING;
        g_net_state = ES_CLOSING;
        if (es->p_tx == NULL) {
            /* we're done sending, close connection */
            tcp_activation_client_connection_close(tpcb, es);
        } else {
            /* send remaining data*/
            tcp_activation_client_send(tpcb, es);
        }
        ret_err = ERR_OK;
    }
    /* else : a non empty frame was received from echo server but for some reason err != ERR_OK */
    else if (err != ERR_OK) {
        /* free received pbuf*/
        pbuf_free(p);

        ret_err = err;
    } else if (es->state == ES_CONNECTED) {
        memcpy(g_response_data, p->payload, p->tot_len);

        /* Acknowledge data reception */
        tcp_recved(tpcb, p->tot_len);

        pbuf_free(p);
        tcp_activation_client_connection_close(tpcb, es);
        ret_err = ERR_OK;

        /* increment message count */
        message_count++;
    }

    /* data received when connection already closed */
    else {
        /* Acknowledge data reception */
        tcp_recved(tpcb, p->tot_len);

        /* free pbuf and do nothing */
        pbuf_free(p);
        ret_err = ERR_OK;
    }
    return ret_err;
}

/**
 * @brief function used to send data
 * @param  tpcb: tcp control block
 * @param  es: pointer on structure of type echoclient containing info on data
 *             to be sent
 * @retval None
 */
static void tcp_activation_client_send(struct tcp_pcb *tpcb, struct activation_client *es)
{
    struct pbuf *ptr;
    err_t wr_err = ERR_OK;

    while ((wr_err == ERR_OK) && (es->p_tx != NULL) && (es->p_tx->len <= tcp_sndbuf(tpcb))) {
        /* get pointer on pbuf from es structure */
        ptr = es->p_tx;

        /* enqueue data for transmission */
        wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);

        if (wr_err == ERR_OK) {
            /* continue with next pbuf in chain (if any) */
            es->p_tx = ptr->next;

            if (es->p_tx != NULL) {
                /* increment reference count for es->p */
                pbuf_ref(es->p_tx);
            }

            /* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
            pbuf_free(ptr);
        } else if (wr_err == ERR_MEM) {
            /* we are low on memory, try later, defer to poll */
            es->p_tx = ptr;
        } else {
            /* other problem ?? */
        }
    }
}

/**
 * @brief  This function implements the tcp_poll callback function
 * @param  arg: pointer on argument passed to callback
 * @param  tpcb: tcp connection control block
 * @retval err_t: error code
 */
static err_t tcp_activation_client_poll(void *arg, struct tcp_pcb *tpcb)
{
    err_t ret_err;
    struct activation_client *es = (struct activation_client *)arg;

    if (es != NULL) {
        if (es->p_tx != NULL) {
            /* there is a remaining pbuf (chain) , try to send data */
            tcp_activation_client_send(tpcb, es);
        } else {
            /* no remaining pbuf (chain)  */
            if (es->state == ES_CLOSING) {
                /* close tcp connection */
                tcp_activation_client_connection_close(tpcb, es);
            }
        }
        ret_err = ERR_OK;
    } else {
        /* nothing to be done */
        tcp_abort(tpcb);
        ret_err = ERR_ABRT;
    }
    return ret_err;
}

/**
 * @brief  This function implements the tcp_sent LwIP callback (called when ACK
 *         is received from remote host for sent data)
 * @param  arg: pointer on argument passed to callback
 * @param  tcp_pcb: tcp connection control block
 * @param  len: length of data sent
 * @retval err_t: returned error code
 */
static err_t tcp_activation_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    LWIP_UNUSED_ARG(len);

    struct activation_client *es = (struct activation_client *)arg;

    if (es->p_tx != NULL) {
        /* still got pbufs to send */
        tcp_activation_client_send(tpcb, es);
    }

    return ERR_OK;
}

/**
 * @brief This function is used to close the tcp connection with server
 * @param tpcb: tcp connection control block
 * @param es: pointer on echoclient structure
 * @retval None
 */
static void tcp_activation_client_connection_close(struct tcp_pcb *tpcb, struct activation_client *es)
{
    /* remove callbacks */
    tcp_recv(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_poll(tpcb, NULL, 0);

    if (es != NULL) {
        mem_free(es);
    }

    /* close tcp connection */
    tcp_close(tpcb);
    g_net_state = ES_NOT_CONNECTED;
}

#endif /* LWIP_TCP */
