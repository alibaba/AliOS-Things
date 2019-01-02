/**
 * @file    wm_socket.h
 *
 * @brief   socket Module
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
 
#ifndef WM_SOCKET_H
#define WM_SOCKET_H
#include "wm_config.h"
#if TLS_CONFIG_LWIP_VER2_0_3
#include "lwip/sockets.h"

#else
#include "wm_type_def.h"

//socket state defination
#define NETCONN_STATE_NONE         0
#define NETCONN_STATE_WAITING      1
#define NETCONN_STATE_CONNECTED    2
#define NETCONN_STATE_CLOSED       3

//socket event defination
#define NET_EVENT_TCP_JOINED            0
#define NET_EVENT_TCP_DISCONNECT        1
#define NET_EVENT_TCP_CONNECTED         2
#define NET_EVENT_TCP_CONNECT_FAILED    3
#define NET_EVENT_UDP_START             4
#define NET_EVENT_UDP_START_FAILED      5

#define TLS_MAX_SOCKET_NUM       4
#define TLS_MAX_NETCONN_NUM      20

struct pbuf {
  /** next pbuf in singly linked pbuf chain */
  struct pbuf *next;

  /** pointer to the actual data in the buffer */
  void *payload;

  /**
   * total length of this buffer and all next buffers in chain
   * belonging to the same packet.
   *
   * For non-queue packet chains this is the invariant:
   * p->tot_len == p->len + (p->next? p->next->tot_len: 0)
   */
  u16 tot_len;

  /** length of this buffer */
  u16 len;

  /** pbuf_type as u8 instead of enum to save space */
  u8 /*pbuf_type*/ type;

  /** misc flags */
  u8 flags;

  /**
   * the reference count always equals the number of pointers
   * that refer to this pbuf. This can be pointers from an application,
   * the stack itself, or pbuf->next pointers from a chain.
   */
  u16 ref;
};

/**
* @brief This Function prototype for tcp error callback functions. Called when 
*                    receives a RST or is unexpectedly closed for any other reason.
*                   The corresponding socket is already freed when this callback is called!
*
* @param[in] skt_num    Is the socket number that returned by tls_socket_create function.
*
* @param[out] err           Error code to indicate why the socket has been closed
*                          ERR_ABRT: aborted through returning ERR_ABRT from within others
*                                                      callback functions
*                          ERR_RST: the connection was reset by the remote host
*/
typedef void  (*socket_err_fn)(u8 skt_num, s8 err);

/**
* @brief This Function prototype for socket receive callback functions. Called when data has
*                    been received.
*
* @param[in] skt_num   Is the socket number that returned by tls_socket_create function.
*
* @param[in] p              The received data (or NULL when the connection has been closed!)
*
* @param[out] err           An error code if there has been an error receiving, always be ERR_OK 
*                                    when cs mode is udp.
*
* @retval The return value is only valid for tcp receive, for upd it means nothing.
*                   ERR_OK: Return this value after handling the received data.
*                   ERR_ABRT:  Only return ERR_ABRT if you want to abort the socket from within the
*                                      callback function!
*/
typedef s8  (*socket_recv_fn)(u8 skt_num, struct pbuf *p, s8 err);

/**
* @brief This Function prototype for socket srce ip callback functions. Called when data has
*                    been received.
*
* @param[in] skt_num   Is the socket number that returned by tls_socket_create function.
*
* @param[in] datalen           The received data length
*
* @param[in] ipsrc         source ip addr
*
* @param[in] port          source port
*
* @param[out] err           An error code if there has been an error receiving, always be ERR_OK 
*                                    when cs mode is udp.
*
* @retval The return value is only valid for UDP receive, for udp it means nothing.
*                   ERR_OK: Return this value after handling the received data.
*                   ERR_ABRT:  Only return ERR_ABRT if you want to abort the socket from within the
*                                      callback function!
*/
typedef s8  (*socket_recv_ip_rpt_fn)(u8 skt_num, u16 datalen, u8 *ipsrc, u16 port, s8 err);

/**
* @brief This Function prototype for tcp connected callback functions. Called when 
*                   connected to the remote side.
*
* @param[in] skt_num   Is the socket number that returned by tls_socket_create function.
*
* @param[out]  err     An unused error code, always ERR_OK currently.
*
* @retval    ERR_OK: Return this value after handling your logic.
* @retval    ERR_ABRT:  Only return ERR_ABRT if you want to abort the socket from within the
*                                      callback function!
*/
typedef s8 (*socket_connected_fn)(u8 skt_num,  s8 err);

/**
* @brief  This Function prototype for tcp poll callback functions. Called periodically.
*
* @param[in] skt_num   Is the socket number that returned by tls_socket_create function.
*
* @retval    ERR_OK:     Try to do something periodically.
* @retval    ERR_ABRT:  Only return ERR_ABRT if you want to abort the socket from within the
*                                      callback function!
*/
typedef s8 (*socket_poll_fn)(u8 skt_num);

/**
* @brief This Function prototype for tcp accept callback functions. Called when a new
*                   connection can be accepted on a listening tcp.
*
* @param[in] skt_num   Is the socket number that returned by tls_socket_create function.
*
* @param[out]  err            An error code if there has been an error accepting.
*
* @retval    ERR_OK:      Return this value after handling your logic.
* @retval    ERR_ABRT:  Only return ERR_ABRT if you want to abort the socket from within the
*                                      callback function!
*/
typedef s8 (*socket_accept_fn)(u8 skt_num, s8 err);

/**
* @brief This Function prototype for socket state changed callback functions. Called when socket
*                   the sockte's state changed.
*
* @param[in] skt_num   Is the socket number that returned by tls_socket_create function.
*
* @param[in] event       Is the event number, see socket event defination.
*
* @param[in] state        Is the socket state, see socket state defination.
*/
typedef void(*socket_state_changed_fn)(u8 skt_num, u8 event, u8 state);

#define ERR_OK          0    /* No error, everything OK. */
#define ERR_MEM        -1    /* Out of memory error.     */
#define ERR_BUF        -2    /* Buffer error.            */
#define ERR_TIMEOUT    -3    /* Timeout.                 */
#define ERR_RTE        -4    /* Routing problem.         */
#define ERR_INPROGRESS -5    /* Operation in progress    */
#define ERR_VAL        -6    /* Illegal value.           */
#define ERR_WOULDBLOCK -7    /* Operation would block.   */
#define ERR_USE        -8    /* Address in use.          */
#define ERR_ISCONN     -9    /* Already connected.       */

#define ERR_IS_FATAL(e) ((e) < ERR_ISCONN)

#define ERR_ABRT       -10   /* Connection aborted.      */
#define ERR_RST        -11   /* Connection reset.        */
#define ERR_CLSD       -12   /* Connection closed.       */
#define ERR_CONN       -13   /* Not connected.           */

#define ERR_ARG        -14   /* Illegal argument.        */

#define ERR_IF         -15   /* Low-level netif error    */


enum tls_socket_protocol{
    SOCKET_PROTO_TCP,      /* TCP Protocol    */
    SOCKET_PROTO_UDP,     /* UDP Protocol   */
};

enum tls_socket_cs_mode{
    SOCKET_CS_MODE_CLIENT,    /* Client mode    */
    SOCKET_CS_MODE_SERVER,    /* Server mode   */
};

struct tls_socket_desc {
    enum tls_socket_cs_mode cs_mode;              /* Server mode  or Client mode, Only for tcp protocol is valid */
    enum tls_socket_protocol protocol;                /* TCP Protocol or UDP Protocol  */
    u8  ip_addr[4];                                              /* Remote ip address, for tcp client mode is remote server's ip address; for tcp server mode can be any address. 
	                                                                              for udp is remote server's ip address */
    u16 port;                                                       /* port, for tcp client mode is remote server's port; for tcp server mode is local listen port . 
	                                                                              for udp is remote server's port */
    u16 localport;                                               /* local port, for udp and tcp client is local listen port, for tcp server means nothing, tcp server always listen at port */
    char host_name[32];                                     /* remote host name, not support for now  */
    u8  host_len;                                                 /* the length of host name   */
    u32 timeout;                                                  /* poll timeout, not implemented for now   */
    socket_err_fn errf;                                          /* a pointer to socket_err_fn   */
    socket_recv_fn recvf;                                      /* a pointer to socket_recv_fn  */
    socket_connected_fn connf;                             /* a pointer to socket_connected_fn  */
    socket_poll_fn pollf;                                        /* a pointer to socket_poll_fn  */
    socket_accept_fn acceptf;                               /* a pointer to socket_accept_fn   */
    socket_state_changed_fn state_changed;       /* a pointer to socket_state_changed_fn   */
    socket_recv_ip_rpt_fn recvwithipf;           /*recv skt info report*/
};

/**
* @brief This function is called by your application code to create a socket.
*
* @param[in] skd      Is a pointer to an tls_socket_desc.
*
* @retval ERR_OK    If create socket successfully.
*              negative number   If an error was detected.
*/
int tls_socket_create(struct tls_socket_desc * skd);

/**
* @brief This function is called by your application code to send data by the socket.
*
* @param[in] skt_num      Is the socket number that returned by tls_socket_create function.
*
* @param[in] pdata          Is a pointer to the data which need to be send by the socket.
*
* @param[in] len              The data's length.
*
* @retval ERR_OK    If send data successfully.
*              negative number   If an error was detected.
*/
int tls_socket_send(u8 skt_num, void *pdata, u16 len);

/**
* @brief This function is called by your application code to close the socket, and the related resources would be released.
*
* @param[in] skt_num      Is the socket number that returned by tls_socket_create function.
*
* @retval ERR_OK    If close socket successfully.
*              negative number   If an error was detected.
*/
int tls_socket_close(u8 skt_num);

struct tls_skt_status_ext_t {
    u8 socket;
    u8 status;
    enum tls_socket_protocol protocol;
    u8 host_ipaddr[4];
    u16 remote_port;
    u16 local_port;
};

struct tls_skt_status_t {
    u32 socket_cnt;
    struct tls_skt_status_ext_t skts_ext[1];
};
/**
* @brief  This function is called by your application code to get the socket status of specified socket num.
*
* @param[in] skt_num      Is the socket number that returned by tls_socket_create function.
*
* @param[in] buf          Is a pointer to the data contains the socket status, if the socket is server, also contains it's client's status.
*
* @param[in] bufsize      The buf's length. At least, the len should be bigger than sizeof(struct tls_skt_status_t).
*
* @retval ERR_OK    If send data successfully.
*              negative number   If an error was detected.
*/
int tls_socket_get_status(u8 skt_num, u8 *buf, u32 bufsize);
/**
* @brief This function is called by your application code to send data by udp socket.
*
* @param[in] localport         This function will search all created sockets, if there is a socket whose localport equals this value and it's protocol is udp,
*                                          then send the data by this socket, otherwise, nothing to send.
*
*  @param[in] ip_addr          Is the remote ip address.
*
*  @param[in] port               Is the remote port which upd send to.
*
*  @param[in] pdata             Is a pointer to the data which need to be send by the socket.
*
*  @param[in] len              The data's length.
*  @retval ERR_OK    If send data successfully.
*              negative number   If an error was detected.
*/
int tls_socket_udp_sendto(u16 localport, u8  *ip_addr, u16 port, void *pdata, u16 len);

typedef enum {
  PBUF_TRANSPORT,
  PBUF_IP,
  PBUF_LINK,
  PBUF_RAW
} pbuf_layer;

typedef enum {
  PBUF_RAM, /* pbuf data is stored in RAM */
  PBUF_ROM, /* pbuf data is stored in ROM */
  PBUF_REF, /* pbuf comes from the pbuf pool */
  PBUF_POOL /* pbuf payload refers to RAM */
} pbuf_type;


/**
* @brief This Function allocates a pbuf of the given type (possibly a chain for PBUF_POOL type).
*
*                   The actual memory allocated for the pbuf is determined by the
*                   layer at which the pbuf is allocated and the requested size
*                   (from the size parameter).
*
* @param[in] l                   layer flag to define header size
* @param[in] length           size of the pbuf's payload
* @param[in] type             this parameter decides how and where the pbuf
*
* @retval The allocated pbuf. If multiple pbufs where allocated, this
*                     is the first pbuf of a pbuf chain.
*/
struct pbuf *pbuf_alloc(pbuf_layer l, u16 length, pbuf_type type);
/**
* @brief This Function for release the buffer that you receive within the socket_recv_fn callback function.
*                   Attention please: If you return ERR_OK in the socket_recv_fn callback function, you must call this
*                                            function to release the buffer by yourself. Otherwise, the buffer do not need be 
*                                            released by your code.
*
* @param[in] p       The buffer you received in the socket_recv_fn callback function.
*
* @retval The number of de-allocated pbufs
*/
u8 pbuf_free(struct pbuf *p);

/**
* @brief This Function for copy (part of) the contents of a packet buffer to an application supplied buffer.
*
* @param[in] p     the pbuf from which to copy data.
*
* @param[in] dataptr   the application supplied buffer
*
* @param[in] len      length of data to copy (dataptr must be big enough). No more 
*                                than buf->tot_len will be copied, irrespective of len
*
* @param[in] offset   offset into the packet buffer from where to begin copying len bytes
*
* @retval The number of bytes copied, or 0 on failure
*/
u16 pbuf_copy_partial(struct pbuf *p, void *dataptr, u16 len, u16 offset);
#endif

#endif
