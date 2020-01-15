/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lwip/debug.h"
#include "lwip/sockets.h"
#include "lwip/apps/iperf_task.h"
#include "lwip/apps/iperf_debug.h"
/******************************************************
 *                      Macros
 ******************************************************/
#define IPERF_DEBUG_INTERNAL
#define DBGPRINT_IPERF(FEATURE, ...)  do {          \
            if (g_iperf_debug_feature & FEATURE) {  \
                LWIP_DEBUGF( IPERF_DEBUG, (__VA_ARGS__));             \
            }                                       \
        } while(0)

/******************************************************
 *                    Constants
 ******************************************************/

#define MAX_WIN_SIZE (20*1024)

/* Private macro */
#define IPERF_DEFAULT_PORT  5001 //Port to listen

#define IPERF_HEADER_VERSION1 0x80000000
#define IPERF_DEFAULT_UDP_RATE (1024 * 1024)
#define IPERF_TEST_BUFFER_SIZE (2048)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

// Private typedef -------------------------------------------------------------
typedef struct count_s
{
    unsigned Bytes;
    unsigned KBytes;
    unsigned MBytes;
    unsigned GBytes;
    unsigned times;
} count_t;

// used to reference the 4 byte ID number we place in UDP datagrams
// use int32_t if possible, otherwise a 32 bit bitfield (e.g. on J90)
typedef struct UDP_datagram
{
    int32_t id;
    unsigned int tv_sec;
    unsigned int tv_usec;
} UDP_datagram;

/*
 * The client_hdr structure is sent from clients
 * to servers to alert them of things that need
 * to happen. Order must be perserved in all
 * future releases for backward compatibility.
 * 1.7 has flags, num_threads, port, and buffer_len
 */
typedef struct client_hdr
{
    /*
     * flags is a bitmap for different options
     * the most significant bits are for determining
     * which information is available. So 1.7 uses
     * 0x80000000 and the next time information is added
     * the 1.7 bit will be set and 0x40000000 will be
     * set signifying additional information. If no
     * information bits are set then the header is ignored.
     * The lowest order diferentiates between dualtest and
     * tradeoff modes, wheither the speaker needs to start
     * immediately or after the audience finishes.
     */
    int32_t flags;
    int32_t num_threads;
    int32_t port;
    int32_t buffer_len;
    int32_t win_band;
    int32_t amount;
} client_hdr;

/*
 * The server_hdr structure facilitates the server
 * report of jitter and loss on the client side.
 * It piggy_backs on the existing clear to close
 * packet.
 */
typedef struct server_hdr
{
    /*
     * flags is a bitmap for different options
     * the most significant bits are for determining
     * which information is available. So 1.7 uses
     * 0x80000000 and the next time information is added
     * the 1.7 bit will be set and 0x40000000 will be
     * set signifying additional information. If no
     * information bits are set then the header is ignored.
     */
    int32_t flags;
    int32_t total_len1;
    int32_t total_len2;
    int32_t stop_sec;
    int32_t stop_usec;
    int32_t error_cnt;
    int32_t outorder_cnt;
    int32_t datagrams;
    int32_t jitter1;
    int32_t jitter2;
} server_hdr;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

count_t iperf_calculate_result( int pkt_size, count_t pkt_count, int need_to_convert );
void iperf_display_report( char *report_title, unsigned time, unsigned h_ms_time, count_t pkt_count );
count_t iperf_reset_count( count_t pkt_count );
count_t iperf_copy_count( count_t pkt_count, count_t tmp_count );
count_t iperf_diff_count( count_t pkt_count, count_t tmp_count );
int iperf_format_transform( char *param );

/******************************************************
 *               Variables Definitions
 ******************************************************/

uint32_t g_iperf_debug_feature = 0;
int g_iperf_is_tradeoff_test_client = 0;
int g_iperf_is_tradeoff_test_server = 0;
uint32_t g_iperf_server_addr = 0;

/******************************************************
 *               Function Definitions
 ******************************************************/
/* members are in network byte order */
void iperf_udp_run_server( char *parameters[] )
{

    int sockfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    int cli_len;
    struct ip_mreq group;
    int server_port;
    int i;
    count_t pkt_count;
    count_t tmp_count;
    int nbytes = 0; /* the number of read */
    int total_send = 0; /* the total number of send  */
    int mcast_tag = 0; /* the tag of parameter "-B"  */
    int interval_tag = 0; /* the tag of parameter "-i"  */
    char *mcast;
#if defined(IPERF_DEBUG_ENABLE)
    int send_bytes = 0; /* the number of send */
    int tmp = 0;
#endif
    char *buffer = (char*) malloc( IPERF_TEST_BUFFER_SIZE );
    
    uint32_t t1, t2, curr_t, curr_h_ms, t2_h_ms, t1_h_ms, tmp_t, tmp_h_ms, offset_t1, offset_t2, offset_time;
    UDP_datagram *udp_h;
    client_hdr *client_h;
    client_hdr client_h_trans;
    uint32_t timeout;
    timeout = 20 * 1000; //set recvive timeout = 20(sec)
    int is_test_started = 0;
    int udp_h_id = 0;

    memset( buffer, 0, IPERF_TEST_BUFFER_SIZE );
    //Statistics init
    pkt_count = iperf_reset_count( pkt_count );
    tmp_count = iperf_reset_count( tmp_count );
    server_port = 0;
    t1 = 0;
    t2 = 0;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);

    //Handle input parameters
    if ( g_iperf_is_tradeoff_test_client == 0 ) {
        for ( i = 0; i < 13; i++ ) {
            if ( strcmp( (char *) &parameters[i * offset], "-p" ) == 0 ) {
                i++;
                server_port = atoi( (char *) &parameters[i * offset] );
            } else if ( strcmp( (char *) &parameters[i * offset], "-n" ) == 0 ) {
                i++;
                total_send = iperf_format_transform( (char *) &parameters[i * offset] );
                LWIP_DEBUGF( IPERF_DEBUG, ("Set number to transmit = %d Bytes", total_send ));
            } else if ( strcmp( (char *) &parameters[i * offset], "-B" ) == 0 ) {
                i++;
                mcast = (char *) &parameters[i * offset];
                mcast_tag = 1;
                LWIP_DEBUGF( IPERF_DEBUG, ("Join Multicast %s", mcast) );
            } else if ( strcmp( (char *) &parameters[i * offset], "-i" ) == 0 ) {
                interval_tag = 1;
                LWIP_DEBUGF( IPERF_DEBUG, ("Set 10 seconds between periodic bandwidth reports" ));
            }
        }
    }

    // Create a new UDP connection handle
    if ( (sockfd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("[%s:%d] sockfd = %d", __FUNCTION__, __LINE__, sockfd ));
        if ( parameters ) {
            free( parameters );
        }
        aos_task_exit(0);
    }

    socklen_t len = sizeof(timeout);
    if ( setsockopt( sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, len ) < 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("Setsockopt failed - cancel receive timeout" ));
    }

    // Bind to port and any IP address
    memset( &servaddr, 0, sizeof(servaddr) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
    if ( server_port == 0 ) {
        servaddr.sin_port = htons( IPERF_DEFAULT_PORT );
        LWIP_DEBUGF( IPERF_DEBUG, ("Default server port = %d", IPERF_DEFAULT_PORT ));
    } else {
        servaddr.sin_port = htons( server_port );
        LWIP_DEBUGF( IPERF_DEBUG, ("Set server port = %d", server_port ));
    }

    //Multicast settings
    if ( mcast_tag == 1 ) {
        group.imr_multiaddr.s_addr = inet_addr( mcast );
        group.imr_interface.s_addr = htonl( INADDR_ANY );

        if ( setsockopt( sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &group, sizeof(struct ip_mreq) ) < 0 ) {
            LWIP_DEBUGF( IPERF_DEBUG, ("Setsockopt failed - multicast settings" ));
        }
    }

    if ( (bind( sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr) )) < 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("[%s:%d]", __FUNCTION__, __LINE__ ));
        if ( parameters ) {
            free( parameters );
        }
        aos_task_exit(0);
    }

    cli_len = sizeof(cliaddr);

    // Wait and check the request
    do {
        // Handles request
        do {
            iperf_get_current_time( &offset_t1, 0 );
            nbytes = recvfrom( sockfd, buffer, IPERF_TEST_BUFFER_SIZE, 0, (struct sockaddr *) &cliaddr,
                               (socklen_t *) &cli_len );
            iperf_get_current_time( &offset_t2, 0 );

            //if connected to iperf v2.0.1, there is no end package sent from client side
            if ( (offset_t2 > (offset_t1 + 2)) && (nbytes <= 0) && (pkt_count.times >= 1) ) {
                offset_time = offset_t2 - offset_t1;
            } else if ( offset_time != 0 ) {
                offset_time = 0;
            }

            udp_h = (UDP_datagram *) buffer;
            udp_h_id = (int) ntohl( udp_h->id );

#if defined(IPERF_DEBUG_INTERNAL)
            client_h = (client_hdr *)&buffer[12];
            client_h_trans.flags = (int32_t)(ntohl(client_h->flags));
            client_h_trans.num_threads = (int32_t)(ntohl(client_h->num_threads));
            client_h_trans.port = (int32_t)(ntohl(client_h->port));
            client_h_trans.buffer_len = (int32_t)(ntohl(client_h->buffer_len));
            client_h_trans.win_band = (int32_t)(ntohl(client_h->win_band));
            client_h_trans.amount = (int32_t)(ntohl(client_h->amount));

            DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, "UDP server, receive from sockfd \"%d\", id is \"%d\", tv_sec is \"%d\", tv_usec is \"%d\", nbytes is \"%d\"",
                    sockfd, udp_h_id, ntohl(udp_h->tv_sec), ntohl(udp_h->tv_usec), nbytes);
            DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, "UDP server, receive from sin_len = %d, sin_family = %d , port = %d, s_addr = 0x%x", cliaddr.sin_len, cliaddr.sin_family,
                    cliaddr.sin_port, cliaddr.sin_addr.s_addr);
            DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, "[%s:%d] t1 = %d, t2 = %d", __FUNCTION__, __LINE__, t1, t2);

            DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, "[%s:%d], client_h_trans.flag = %d, num_threads = %d, port = %d, buffer_len = %d, win_band = %d, amount = %d"
                    , __FUNCTION__, __LINE__, client_h_trans.flags, client_h_trans.num_threads, client_h_trans.port, client_h_trans.buffer_len, client_h_trans.win_band, client_h_trans.amount);
#endif

#if defined(IPERF_DEBUG_ENABLE)
            if (tmp != nbytes) {
                DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, "[%s:%d] nbytes=%d ", __FUNCTION__, __LINE__, nbytes);
            } else {
                DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, ".");
            }
            tmp = nbytes;
#endif

            pkt_count = iperf_calculate_result( nbytes, pkt_count, 0 );

            if ( pkt_count.times == 1 ) {
                iperf_get_current_time( &t1, &t1_h_ms );
                t1_h_ms = (t1_h_ms / 100) % 10;
            }

            // Report by second
            if ( (pkt_count.times >= 1 && interval_tag > 0) ) {
                iperf_get_current_time( &curr_t, &curr_h_ms );
                curr_h_ms = (curr_h_ms / 100) % 10;

                if ( offset_time > 0 ) {
                    curr_t -= offset_time;
                }

                if ( curr_h_ms >= t1_h_ms ) {
                    tmp_h_ms = curr_h_ms - t1_h_ms;
                    tmp_t = curr_t - t1;
                } else {
                    tmp_h_ms = curr_h_ms + 10 - t1_h_ms;
                    tmp_t = curr_t - t1 - 1;
                }

                if ( (((int)(curr_t - t1) / 10) == interval_tag) && ((curr_h_ms >= t1_h_ms) || ((curr_t - t1) % 10) >= 1) ) {
                    LWIP_DEBUGF( IPERF_DEBUG, ("Interval: %d.0 - %d.0 sec   ", (int) (curr_t - t1) / 10 * 10 - 10,
                            (int) (curr_t - t1) / 10 * 10 ));
                    iperf_display_report( "UDP Server", 10, 0, iperf_diff_count( pkt_count, tmp_count ) );
                    tmp_count = iperf_copy_count( pkt_count, tmp_count );
                    interval_tag++;
                } else if ( ((udp_h_id < 0) || (nbytes <= 0)) && (((tmp_t) % 10) != 0) && (is_test_started == 1) ) {
                    LWIP_DEBUGF( IPERF_DEBUG, ("Interval: %d.0 - %d.%d sec   ", (int) (curr_t - t1 + 1) / 10 * 10 - 10, 
                            (int) tmp_t, (int) tmp_h_ms ));
                    iperf_display_report( "UDP Server", (tmp_t - ((curr_t - t1 + 1) / 10 * 10 - 10)), tmp_h_ms,
                                          iperf_diff_count( pkt_count, tmp_count ) );
                    tmp_count = iperf_copy_count( pkt_count, tmp_count );
                    interval_tag++;
                }
            }

            if ( (is_test_started == 0) && (udp_h_id > 0) && (nbytes > 0) ) {
                is_test_started = 1;
            } else if ( ((udp_h_id < 0) || (nbytes <= 0)) && (is_test_started == 1) ) { // the last package
                int32_t old_flag = 0;

                // test end, save the current time to "t2"
                if ( pkt_count.times >= 1 ) {
                    /* sync the time if report by second */
                    if ( interval_tag > 0 ) {
                        t2 = curr_t;
                        t2_h_ms = curr_h_ms;
                    } else {
                        iperf_get_current_time( &t2, &t2_h_ms );
                        t2_h_ms = (t2_h_ms / 100) % 10;
                        if ( offset_time > 0 ) {
                            t2 -= offset_time;
                        }
                    }
                }

                // Calculate time: second
                if ( t2_h_ms >= t1_h_ms ) {
                    t2_h_ms = t2_h_ms - t1_h_ms;
                    t2 = t2 - t1;
                } else {
                    t2_h_ms = t2_h_ms + 10 - t1_h_ms;
                    t2 = t2 - t1 - 1;
                }
                // print out result
                iperf_display_report( "[Total]UDP Server", t2, t2_h_ms, pkt_count );

                //TODO: need to send the correct report to client-side, flag = 0 means the report is ignored.
                if ( udp_h_id < 0 ) {
                    old_flag = client_h_trans.flags;
                    client_h_trans.flags = (int32_t) 0;

                    // send the server report to client-side
#if defined(IPERF_DEBUG_ENABLE)
                    send_bytes =
#endif
                    sendto( sockfd, buffer, nbytes, 0, (struct sockaddr *) &cliaddr, cli_len );
                    client_h_trans.flags = old_flag;
                }

#if defined(IPERF_DEBUG_ENABLE)
                DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, "[%s:%d]send_bytes = %d, nbytes = %d,", __FUNCTION__, __LINE__, send_bytes, nbytes);
#endif

                client_h = (client_hdr *) &buffer[12];
                client_h_trans.flags = (int32_t) (ntohl( client_h->flags ));

                // Tradeoff mode
                if ( IPERF_HEADER_VERSION1 & client_h_trans.flags ) {
                    LWIP_DEBUGF( IPERF_DEBUG, ("Tradeoff mode, client-side start." ));

                    g_iperf_is_tradeoff_test_server = 1;
                    g_iperf_server_addr = cliaddr.sin_addr.s_addr;
                    iperf_udp_run_client( NULL );
                    g_iperf_is_tradeoff_test_server = 0;

                }

                LWIP_DEBUGF( IPERF_DEBUG, ("Data transfer is finished." ));
                //TODO: send report to other side
                break;
            }
        } while ( nbytes > 0 );

#if defined(IPERF_DEBUG_ENABLE)
        DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, "[%s:%d] Interval = %d.%d (secs)", __FUNCTION__, __LINE__, t2, t2_h_ms); //sec.
#endif

    } while ( 0 );

    LWIP_DEBUGF( IPERF_DEBUG, (" UDP server close socket!" ));
    close( sockfd );

    LWIP_DEBUGF( IPERF_DEBUG, ("If you want to execute iperf server again, please enter \"iperf -s -u\"." ));

    if ( parameters ) {
        free( parameters );
    }
    free( buffer );
    // For tradeoff mode, task will be deleted in iperf_udp_run_client
    if ( g_iperf_is_tradeoff_test_client == 0 ) {
        aos_task_exit(0);
    }
}

void iperf_tcp_run_server( char *parameters[] )
{
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    int server_port;
    int i;
    count_t pkt_count;
    count_t tmp_count;
    int nbytes = 0; /* the number of read */
    int total_rcv = 0; /* the total number of receive  */
    int num_tag = 0; /* the tag of parameter "-n"  */
    int interval_tag = 0; /* the tag of parameter "-i"  */
#if defined(IPERF_DEBUG_ENABLE)
    int tmp = 0;
#endif
    uint32_t t1, t2, curr_t;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);
    char *buffer = (char*) malloc( IPERF_TEST_BUFFER_SIZE );
    uint32_t timeout;
    timeout = 20 * 1000; //set recvive timeout = 20(sec)

    memset( buffer, 0, IPERF_TEST_BUFFER_SIZE );
    //Statistics init
    pkt_count = iperf_reset_count( pkt_count );
    tmp_count = iperf_reset_count( tmp_count );
    server_port = 0;

    //Handle input parameters
    for ( i = 0; i < 9; i++ ) {
        if ( strcmp( (char *) &parameters[i * offset], "-p" ) == 0 ) {
            i++;
            server_port = atoi( (char *) &parameters[i * offset] );
        } else if ( strcmp( (char *) &parameters[i * offset], "-n" ) == 0 ) {
            i++;
            total_rcv = iperf_format_transform( (char *) &parameters[i * offset] );
            num_tag = 1;
            LWIP_DEBUGF( IPERF_DEBUG, ("Set number to receive = %d Bytes", total_rcv ));
        } else {
            if ( strcmp( (char *) &parameters[i * offset], "-i" ) == 0 ) {
            interval_tag = 1;
                LWIP_DEBUGF( IPERF_DEBUG, ( "Set 10 seconds between periodic bandwidth reports" ));
            }
        }
    }

    // Create a new TCP connection handle
    if ( (listenfd = socket( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("[%s:%d] listenfd = %d", __FUNCTION__, __LINE__, listenfd ));
        if ( parameters ) {
            free( parameters );
        }
        aos_task_exit(0);
    }

    socklen_t len = sizeof(timeout);
    if ( setsockopt( listenfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, len ) < 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("Setsockopt failed - cancel receive timeout" ));
    }

    do {
        // Bind to port and any IP address
        memset( &servaddr, 0, sizeof(servaddr) );
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
        if ( server_port == 0 ) {
            servaddr.sin_port = htons( IPERF_DEFAULT_PORT );
            LWIP_DEBUGF( IPERF_DEBUG, ("Default server port = %d", IPERF_DEFAULT_PORT ));
        }
        else {
            servaddr.sin_port = htons( server_port );
            LWIP_DEBUGF( IPERF_DEBUG, ("Set server port = %d", server_port ));
        }

        if ( (bind( listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr) )) < 0 ) {
            LWIP_DEBUGF( IPERF_DEBUG, ("[%s:%d] ", __FUNCTION__, __LINE__ ));
            break;
        }

        // Put the connection into LISTEN state
        if ( (listen( listenfd, 1024 )) < 0 ) {
            LWIP_DEBUGF( IPERF_DEBUG, ("[%s:%d] ", __FUNCTION__, __LINE__ ));
            break;
        }

        int buflen = 64240;
        if ( setsockopt( listenfd, SOL_SOCKET, SO_RCVBUF, (char *) &buflen, sizeof(buflen) ) < 0 ) {
                LWIP_DEBUGF( IPERF_DEBUG, ("Setsockopt failed - cancel receive buf len\n" ));
        }
        do {
            if ( server_port != 0 ) {
                LWIP_DEBUGF( IPERF_DEBUG, ("Listen...(port = %d)", server_port ));
            } else {
                LWIP_DEBUGF( IPERF_DEBUG, ("Listen...(port = %d)", IPERF_DEFAULT_PORT ));
            }
            // Block and wait for an incoming connection
            if ( (connfd = accept( listenfd, (struct sockaddr *) &cliaddr, &clilen )) != -1 ) {
                LWIP_DEBUGF( IPERF_DEBUG, ("[%s:%d] Accept... (sockfd=%d)", __FUNCTION__, __LINE__, connfd ));

                //Connection
                do {
                    nbytes = recv( connfd, buffer, IPERF_TEST_BUFFER_SIZE, 0 );
                    pkt_count = iperf_calculate_result( nbytes, pkt_count, 0 );
                    if ( pkt_count.times == 1 ) {
                        iperf_get_current_time( &t1, 0 );
                    }
#if defined(IPERF_DEBUG_ENABLE)
                    if (tmp != nbytes) {
                        DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, "[%s:%d] nbytes=%d ", __FUNCTION__, __LINE__, nbytes);
                    } else {
                        DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, ".");
                    }
                    tmp = nbytes;
#endif
                    if ( num_tag == 1 ) {
                        total_rcv -= nbytes;
                    }

                    //Reach total receive number "-n"
                    if ( total_rcv < 0 ) {
                        LWIP_DEBUGF( IPERF_DEBUG, ("Finish Receiving" ));
                        break;
                    }
                    if ( pkt_count.times >= 1 && interval_tag > 0 ) {
                        iperf_get_current_time( &curr_t, 0 );
                        if ( ((int)(curr_t - t1) / 10) == interval_tag ) {
                            LWIP_DEBUGF( IPERF_DEBUG, ("Interval: %d - %d sec   ", (int) (curr_t - t1) / 10 * 10 - 10,
                                    (int) (curr_t - t1) / 10 * 10 ));
                            iperf_display_report( "TCP Server", 10, 0, iperf_diff_count( pkt_count, tmp_count ) );
                            tmp_count = iperf_copy_count( pkt_count, tmp_count );
                            interval_tag++;
                        }
                    }
                } while ( nbytes > 0 );

                if ( pkt_count.times >= 1 ) {
                    iperf_get_current_time( &t2, 0 );
                }

                LWIP_DEBUGF( IPERF_DEBUG, ("Close socket!" ));
                //Get report
                iperf_display_report( "[Total]TCP Server", t2 - t1, 0, pkt_count );

                //Statistics init
                pkt_count = iperf_reset_count( pkt_count );
                tmp_count = iperf_reset_count( tmp_count );
                if ( interval_tag > 0 ) {
                    interval_tag = 1;
                } else {
                    interval_tag = 0;
                }
                close( connfd );
            }
        } while ( connfd != -1 && num_tag == 0 );

        close( listenfd );
        if ( num_tag == 0 ) {
            LWIP_DEBUGF( IPERF_DEBUG, ("Close socket!" ));
            iperf_display_report( "[Total]TCP Server ", t2 - t1, 0, pkt_count );
        }
    } while ( 0 ); //Loop just once

    LWIP_DEBUGF( IPERF_DEBUG, ("If you want to execute iperf server again, please enter \"iperf -s\"." ));

    if ( parameters ) {
        free( parameters );
    }

    free( buffer );
        aos_task_exit(0);
}

void iperf_tcp_run_client( char *parameters[] )
{
    int sockfd;
    struct sockaddr_in servaddr;
    char *Server_IP;
    count_t pkt_count;
    count_t tmp_count;
    int nbytes = 0; /* the number of send */
    int total_send = 0; /* the total number of transmit  */
    int num_tag = 0; /* the tag of parameter "-n"  */
    int interval_tag = 0; /* the tag of parameter "-i"  */
    int i;
    int win_size, send_time, server_port, pkt_delay, tos;
    uint32_t t1, t2, curr_t;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);
    char *buffer = (char*) malloc( IPERF_TEST_BUFFER_SIZE );
    pkt_count = iperf_reset_count( pkt_count );
    tmp_count = iperf_reset_count( tmp_count );
    win_size = 0;
    send_time = 0;
    server_port = 0;
    pkt_delay = 0;
    tos = 0;
    memset( buffer, 0, IPERF_TEST_BUFFER_SIZE );
    //Handle input parameters
    Server_IP = (char *) &parameters[0];

    LWIP_DEBUGF( IPERF_DEBUG, ("Servr IP %s ", Server_IP ));
    for ( i = 1; i < 18; i++ ) {
        if ( strcmp( (char *) &parameters[i * offset], "-w" ) == 0 ) {
            i++;
            win_size = iperf_format_transform( (char *) &parameters[i * offset] );
            if ( win_size > MAX_WIN_SIZE ) {
                LWIP_DEBUGF( IPERF_DEBUG, ("Win size too big, set to %d ", MAX_WIN_SIZE ));
                win_size = MAX_WIN_SIZE;
            }
            LWIP_DEBUGF( IPERF_DEBUG, ("Set window size = %d Bytes", win_size ));
        }

        else if ( strcmp( (char *) &parameters[i * offset], "-t" ) == 0 ) {
            i++;
            send_time = atoi( (char *) &parameters[i * offset] );
            LWIP_DEBUGF( IPERF_DEBUG, ("Set send times = %d (secs)", atoi( (char *) &parameters[i * offset] ) ));
        }
        else if ( strcmp( (char *) &parameters[i * offset], "-p" ) == 0 ) {
            i++;
            server_port = atoi( (char *) &parameters[i * offset] );
        }
        else if ( strcmp( (char *) &parameters[i * offset], "-d" ) == 0 ) {
            i++;
            pkt_delay = atoi( (char *) &parameters[i * offset] );
            LWIP_DEBUGF( IPERF_DEBUG, ("Set packet delay = %d (ms)", atoi( (char *) &parameters[i * offset] ) ));
        } else if ( strcmp( (char *) &parameters[i * offset], "-n" ) == 0 ) {
            i++;
            total_send = iperf_format_transform( (char *) &parameters[i * offset] );
            num_tag = 1;
            LWIP_DEBUGF( IPERF_DEBUG, ("Set number to transmit = %d Bytes", total_send ));
        } else if ( strcmp( (char *) &parameters[i * offset], "-S" ) == 0 ) {
            i++;
            tos = atoi( (char *) &parameters[i * offset] );
            LWIP_DEBUGF( IPERF_DEBUG, ("Set TOS = %d", atoi( (char *) &parameters[i * offset] ) ));
        } else if ( strcmp( (char *) &parameters[i * offset], "-i" ) == 0 ) {
            interval_tag = 1;
            LWIP_DEBUGF( IPERF_DEBUG, ("Set 10 seconds between periodic bandwidth reports" ));
        }
    }

    if ( win_size == 0 ) {
        win_size = 1460;
        LWIP_DEBUGF( IPERF_DEBUG, ("Default window size = %d Bytes", win_size ));
    }
    if ( send_time == 0 ) {
        if ( num_tag == 1 ) {
            send_time = 999999;
        }
        else {
            send_time = 10;
            LWIP_DEBUGF( IPERF_DEBUG, ("Default send times = %d (secs)", send_time ));
        }
    }

    // Create a new TCP connection handle
    if ( (sockfd = socket( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("[%s:%d] sockfd = %d", __FUNCTION__, __LINE__, sockfd ));
        if ( parameters ) {
            free( parameters );
        }
        aos_task_exit(0);
    }

    if ( setsockopt( sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos) ) < 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("Set TOS: fail!" ));
    }
    // Bind to port and IP
    memset( &servaddr, 0, sizeof(servaddr) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr( Server_IP );
    if ( server_port == 0 ) {
        servaddr.sin_port = htons( IPERF_DEFAULT_PORT );
        LWIP_DEBUGF( IPERF_DEBUG, ("Default server port = %d", IPERF_DEFAULT_PORT ));
    } else {
        servaddr.sin_port = htons( server_port );
        LWIP_DEBUGF( IPERF_DEBUG, ("Set server port = %d", server_port ));
    }

    if ( (connect( sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr) )) < 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("Connect failed, sockfd is %d, addr is \"%s\"", (int) sockfd,
                ((struct sockaddr *) &servaddr)->sa_data ));
        close( sockfd );
        if ( parameters ) {
            free( parameters );
        }
        aos_task_exit(0);
    }

    iperf_get_current_time( &t1, 0 );

    do {
        nbytes = send( sockfd, buffer, win_size, 0 );
        pkt_count = iperf_calculate_result( nbytes, pkt_count, 0 );
#if defined(IPERF_DEBUG_ENABLE)
        DBGPRINT_IPERF(IPERF_DEBUG_SEND, "[%s:%d]nbytes=%d ", __FUNCTION__, __LINE__, nbytes);
#endif
        aos_msleep( pkt_delay );

        if ( num_tag == 1 ) {
            total_send -= nbytes;
        }
        //Reach total receive number "-n"
        if ( total_send < 0 ) {
            LWIP_DEBUGF( IPERF_DEBUG, ("Finish Sending" ));
            break;
        }

        if ( interval_tag > 0 ) {
            iperf_get_current_time( &curr_t, 0 );

            if ( ((int)(curr_t - t1) / 10) == interval_tag ) {
                LWIP_DEBUGF( IPERF_DEBUG, ("Interval: %d - %d sec   ", (int) (curr_t - t1) / 10 * 10 - 10,
                        (int) (curr_t - t1) / 10 * 10 ));
                iperf_display_report( "TCP Client", 10, 0, iperf_diff_count( pkt_count, tmp_count ) );
                tmp_count = iperf_copy_count( pkt_count, tmp_count );
                interval_tag++;
            }
        }

        iperf_get_current_time( &curr_t, 0 );
    } while ( (int)(curr_t - t1) < send_time );

    iperf_get_current_time( &t2, 0 );

    close( sockfd );
    LWIP_DEBUGF( IPERF_DEBUG, ("Close socket!" ));
    free( buffer );
    iperf_display_report( "[Total]TCP Client", t2 - t1, 0, pkt_count );

    if ( parameters ) {
        free( parameters );
    }
    aos_task_exit(0);
}

void iperf_udp_run_client( char *parameters[] )
{
    int sockfd;
    struct sockaddr_in servaddr;
    char *Server_IP = 0;
    count_t pkt_count;
    count_t tmp_count;
    int nbytes = 0; /* the number of send */
    int total_send = 0; /* the total number of transmit  */
    int num_tag = 0; /* the tag of parameter "-n"  */
    int interval_tag = 0; /* the tag of parameter "-i"  */
    int tradeoff_tag = 0; /* the tag of parameter "-r"  */
    int i;
    int data_size, send_time, server_port, pkt_delay, pkt_delay_offset, tos, bw;
    uint32_t t1, t2, curr_t, t1_ms, last_tick, current_tick, last_sleep, current_sleep;
    UDP_datagram *udp_h;
    client_hdr *client_h;
    int udp_h_id = 0;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);
    char *buffer = (char*) malloc( IPERF_TEST_BUFFER_SIZE );
    // test data init
    for ( i = 0; i < IPERF_TEST_BUFFER_SIZE; i++ ) {
        buffer[i] = (i % 10 + '0');
    }
    memset( buffer, 0, IPERF_TEST_BUFFER_SIZE );
    //Statistics init
    pkt_count = iperf_reset_count( pkt_count );
    tmp_count = iperf_reset_count( tmp_count );
    data_size = 0;
    send_time = 0;
    server_port = 0;
    pkt_delay = 0;
    pkt_delay_offset = 0;
    tos = 0;
    bw = 15728640; /* Change from 2621440 to 15728640 */

    //Handle input parameters
    if ( g_iperf_is_tradeoff_test_server == 0 ) {
        Server_IP = (char *) &parameters[0];
        for ( i = 1; i < 18; i++ ) {
            if ( strcmp( (char *) &parameters[i * offset], "-l" ) == 0 ) {
                i++;
                data_size = iperf_format_transform( (char *) &parameters[i * offset] );
                LWIP_DEBUGF( IPERF_DEBUG, ("Set datagram size = %d Bytes", data_size ));
            } else if ( strcmp( (char *) &parameters[i * offset], "-t" ) == 0 ) {
                i++;
                send_time = atoi( (char *) &parameters[i * offset] );
                LWIP_DEBUGF( IPERF_DEBUG, ("Set send times = %d (secs)", atoi( (char *) &parameters[i * offset] ) ));
            } else if ( strcmp( (char *) &parameters[i * offset], "-p" ) == 0 ) {
                i++;
                server_port = atoi( (char *) &parameters[i * offset] );
            } else if ( strcmp( (char *) &parameters[i * offset], "-d" ) == 0 ) {
                i++;
                pkt_delay = atoi( (char *) &parameters[i * offset] );
                LWIP_DEBUGF( IPERF_DEBUG, ("Set packet delay = %d (ms)", atoi( (char *) &parameters[i * offset] ) ));
            } else if ( strcmp( (char *) &parameters[i * offset], "-n" ) == 0 ) {
                i++;
                total_send = iperf_format_transform( (char *) &parameters[i * offset] );
                num_tag = 1;
                LWIP_DEBUGF( IPERF_DEBUG, ("Set number to transmit = %d Bytes", total_send ));
            } else if ( strcmp( (char *) &parameters[i * offset], "-S" ) == 0 ) {
                i++;
                tos = atoi( (char *) &parameters[i * offset] );
                LWIP_DEBUGF( IPERF_DEBUG, ("Set TOS = %d", atoi( (char *) &parameters[i * offset] ) ));
            } else if ( strcmp( (char *) &parameters[i * offset], "-b" ) == 0 ) {
                i++;
                LWIP_DEBUGF( IPERF_DEBUG, ("Set bandwidth = %s", (char *) &parameters[i * offset] ));
                bw = iperf_format_transform( (char *) &parameters[i * offset] );
                if ( bw > 15728640 || bw <= 0 ) {
                    bw = 15728640; /* Change from 2621440 to 15728640 */
                    LWIP_DEBUGF( IPERF_DEBUG, ("Upper limit of bandwith setting = 60Mbits/sec\n" ));
                }
                LWIP_DEBUGF( IPERF_DEBUG, ("bandwidth = %d", bw ));
            } else if ( strcmp( (char *) &parameters[i * offset], "-i" ) == 0 ) {
                interval_tag = 1;
                LWIP_DEBUGF( IPERF_DEBUG, ("Set 10 seconds between periodic bandwidth reports" ));
            } else if ( strcmp( (char *) &parameters[i * offset], "-r" ) == 0 ) {
                tradeoff_tag = 1;
                LWIP_DEBUGF( IPERF_DEBUG, ( "Set to tradeoff mode" ));
            }
        }
    }

    if ( data_size == 0 ) {
        data_size = 1460;
        LWIP_DEBUGF( IPERF_DEBUG, ("Default datagram size = %d Bytes", data_size ));
    }

    if ( bw > 0 ) {
        pkt_delay = (1000 * data_size) / bw;

        // pkt_dalay add 1ms regularly to reduce the offset
        pkt_delay_offset = (((1000 * data_size) % bw) * 10 / bw);
        if ( pkt_delay_offset ) {
            pkt_delay_offset = 10 / pkt_delay_offset;
        }
    }

    if ( send_time == 0 ) {
        if ( num_tag == 1 ) {
            send_time = 999999;
        } else {
            send_time = 10;
            LWIP_DEBUGF( IPERF_DEBUG, ("Default send times = %d (secs)", send_time ));
        }
    }

    // Create a new TCP connection handle
    if ( (sockfd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("[%s:%d] sockfd = %d", __FUNCTION__, __LINE__, sockfd ));
        if ( parameters ) {
            free( parameters );
        }
        aos_task_exit(0);
    }

    if ( setsockopt( sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos) ) < 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("Set TOS: fail!" ));
    }

    // Bind to port and IP
    memset( &servaddr, 0, sizeof(servaddr) );
    servaddr.sin_family = AF_INET;

    if ( g_iperf_is_tradeoff_test_server == 0 ) {
        servaddr.sin_addr.s_addr = inet_addr( Server_IP );
    } else {
        servaddr.sin_addr.s_addr = g_iperf_server_addr;
    }
    LWIP_DEBUGF( IPERF_DEBUG, ("Server address = %x", (unsigned int) servaddr.sin_addr.s_addr ));

    if ( server_port == 0 ) {
        servaddr.sin_port = htons( IPERF_DEFAULT_PORT );
        LWIP_DEBUGF( IPERF_DEBUG, ("Default server port = %d", IPERF_DEFAULT_PORT ));
    } else {
        servaddr.sin_port = htons( server_port );
        LWIP_DEBUGF( IPERF_DEBUG, ("Set server port = %d", server_port ));
    }

    if ( (connect( sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr) )) < 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ( "Connect failed" ));
        close( sockfd );
        if ( parameters ) {
            free( parameters );
        }
        aos_task_exit(0);
    }

    // Init UDP data header
    udp_h = (UDP_datagram *) &buffer[0];
    client_h = (client_hdr *) &buffer[12];
    if ( tradeoff_tag == 1 ) {
        client_h->flags = htonl( IPERF_HEADER_VERSION1 );
    } else {
        client_h->flags = 0;
    }
    client_h->num_threads = htonl( 1 );
    client_h->port = htonl( IPERF_DEFAULT_PORT );
    client_h->buffer_len = 0;
    client_h->win_band = htonl( IPERF_DEFAULT_UDP_RATE );
    if ( num_tag != 1 ) { // time mode
        client_h->amount = htonl( ~(long )(send_time * 100) );
    } else {
        client_h->amount = htonl( (long )(send_time * 100) );
        client_h->amount &= htonl( 0x7FFFFFFF );
    }

    iperf_get_current_time( &t1, &t1_ms );
    last_tick = t1_ms;
    last_sleep = 0;

    do {
        udp_h->id = htonl( udp_h_id );
        udp_h->tv_sec = htonl( (last_tick + last_sleep) / 1000 );
        udp_h->tv_usec = htonl( last_tick + last_sleep );

        udp_h_id++;

        nbytes = send( sockfd, buffer, data_size, 0 );
        pkt_count = iperf_calculate_result( nbytes, pkt_count, 0 );

        iperf_get_current_time( &curr_t, &current_tick );

        if ( (udp_h_id % pkt_delay_offset) == 0 ) {
            current_sleep = pkt_delay - (current_tick - last_tick - last_sleep) + 1;
        } else {
            current_sleep = pkt_delay - (current_tick - last_tick - last_sleep);
        }

        if ( (int) current_sleep > 0 ) {
            aos_msleep( current_sleep );
        } else {
            current_sleep = 0;
        }

        last_tick = current_tick;
        last_sleep = current_sleep;

#if defined(IPERF_DEBUG_INTERNAL)
        // show the debug info per second
        if (((bw == 0) && ((udp_h_id % 5000 == 0))) || (udp_h_id % (bw / nbytes) == 0)) {
            DBGPRINT_IPERF(IPERF_DEBUG_SEND, "[%s:%d] nbytes = %d, udp_h_id = %d, pkt_delay = %d, current_tick = %d, current_sleep = %d",
                    __FUNCTION__, __LINE__, nbytes, udp_h_id, pkt_delay, current_tick, current_sleep);
        }
#endif

        if ( num_tag == 1 ) {
            total_send -= nbytes;
        }

        //Reach total receive number "-n"
        if ( total_send < 0 ) {
            LWIP_DEBUGF( IPERF_DEBUG, ("Finish Sending" ));
            break;
        }

        if ( interval_tag > 0 ) {
            if ( ((int)(current_tick - t1_ms) / 10000) == interval_tag ) {
                LWIP_DEBUGF( IPERF_DEBUG, ("Interval: %d - %d sec   ", (int) (current_tick - t1_ms) / 10000 * 10 - 10,
                        (int) (current_tick - t1_ms) / 10000 * 10 ));
                iperf_display_report( "UDP Client", 10, 0, iperf_diff_count( pkt_count, tmp_count ) );
                tmp_count = iperf_copy_count( pkt_count, tmp_count );
                interval_tag++;
            }
            iperf_get_current_time( &curr_t, &current_tick );
        }
    } while ( (int)(current_tick + (uint32_t)pkt_delay - t1_ms) < send_time * 1000 );

    iperf_get_current_time( &t2, 0 );
    iperf_display_report( "[Total]UDP Client", t2 - t1, 0, pkt_count );

    // send the last datagram
    udp_h_id = (-udp_h_id);
    udp_h->id = htonl( udp_h_id );
    iperf_get_current_time( &curr_t, 0 );
    udp_h->tv_sec = htonl( curr_t );
    udp_h->tv_usec = htonl( curr_t * 1000 );

    nbytes = send( sockfd, buffer, data_size, 0 );

    //TODO: receive the report from server side and print out

    LWIP_DEBUGF( IPERF_DEBUG, ("UDP Client close socket!" ));
    close( sockfd );

    // tradeoff testing
    if ( tradeoff_tag == 1 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("Tradoff test, start server-side." ));
        g_iperf_is_tradeoff_test_client = 1;
        iperf_udp_run_server( NULL );
        g_iperf_is_tradeoff_test_client = 0;
    }

    if ( parameters ) {
        free( parameters );
    }

    free( buffer );
    // For tradeoff mode, task will be deleted in iperf_udp_run_server
    if ( g_iperf_is_tradeoff_test_server == 0 ) {
        aos_task_exit(0);
    }
}

count_t iperf_calculate_result( int pkt_size, count_t pkt_count, int need_to_convert )
{
    if ( pkt_size > 0 ) {
        pkt_count.Bytes += pkt_size;
        pkt_count.times++;
    }

    if ( need_to_convert == 1 ) {
        if ( pkt_count.Bytes >= 1024 ) {
            pkt_count.KBytes += (pkt_count.Bytes / 1024);
            pkt_count.Bytes = pkt_count.Bytes % 1024;
        }

        if ( pkt_count.KBytes >= 1024 ) {
            pkt_count.MBytes += (pkt_count.KBytes / 1024);
            pkt_count.KBytes = pkt_count.KBytes % 1024;
        }

        if ( pkt_count.MBytes >= 1024 ) {
            pkt_count.GBytes += (pkt_count.MBytes / 1024);
            pkt_count.MBytes = pkt_count.MBytes % 1024;
        }
    }

    return pkt_count;
}

void iperf_display_report( char *report_title, unsigned time, unsigned h_ms_time, count_t pkt_count )
{
    unsigned tmp_time = (time * 10) + h_ms_time;

#if defined(IPERF_DEBUG_ENABLE)
    DBGPRINT_IPERF(IPERF_DEBUG_REPORT, "\nTransfer in %d.%d seconds: ", time, h_ms_time);
    if (pkt_count.GBytes != 0) {
        DBGPRINT_IPERF(IPERF_DEBUG_REPORT, "%d GBytes ", pkt_count.GBytes);
    }

    if (pkt_count.MBytes != 0) {
        DBGPRINT_IPERF(IPERF_DEBUG_REPORT, "%d MBytes ", pkt_count.MBytes);
    }

    if (pkt_count.KBytes != 0) {
        DBGPRINT_IPERF(IPERF_DEBUG_REPORT, "%d KBytes ", pkt_count.KBytes);
    }

    DBGPRINT_IPERF(IPERF_DEBUG_REPORT, "[%s:%d], time = %d, h_ms_time = %d, GBytes = %d, MBytes = %d, KBytes= %d, Bytes= %d ", __FUNCTION__, __LINE__, time, h_ms_time, pkt_count.GBytes, pkt_count.MBytes, pkt_count.KBytes, pkt_count.Bytes);
#endif

    LWIP_DEBUGF( IPERF_DEBUG, ("%s Bandwidth: ", report_title ));

    if ( tmp_time != 0 ) {
        //Calculate Bandwidth
        pkt_count.Bytes = (((pkt_count.KBytes * 8 * 10) % tmp_time) * 1024 + pkt_count.Bytes * 8 * 10) / tmp_time;
        pkt_count.KBytes = (((pkt_count.MBytes * 8 * 10) % tmp_time) * 1024 + pkt_count.KBytes * 8 * 10) / tmp_time;
        pkt_count.MBytes = (((pkt_count.GBytes * 8 * 10) % tmp_time) * 1024 + pkt_count.MBytes * 8 * 10) / tmp_time;
        pkt_count.GBytes = pkt_count.GBytes * 8 * 10 / tmp_time;
    } else {
        pkt_count.Bytes = 0;
        pkt_count.KBytes = 0;
        pkt_count.MBytes = 0;
        pkt_count.GBytes = 0;
    }

    pkt_count = iperf_calculate_result( 0, pkt_count, 1 );

    if ( pkt_count.GBytes != 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("%d Gbits ", pkt_count.GBytes ));
    }

    if ( pkt_count.MBytes != 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("%d Mbits ", pkt_count.MBytes ));
    }

    if ( pkt_count.KBytes != 0 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("%d Kbits ", pkt_count.KBytes ));
    }
    LWIP_DEBUGF( IPERF_DEBUG, ("%d bits/sec", pkt_count.Bytes ));

#if defined(IPERF_DEBUG_ENABLE)
    DBGPRINT_IPERF(IPERF_DEBUG_REPORT, "Receive times: %d", pkt_count.times);
#endif

}

count_t iperf_reset_count( count_t pkt_count )
{
    pkt_count.Bytes = 0;
    pkt_count.KBytes = 0;
    pkt_count.MBytes = 0;
    pkt_count.GBytes = 0;
    pkt_count.times = 0;

    return pkt_count;
}

count_t iperf_copy_count( count_t pkt_count, count_t tmp_count )
{

    tmp_count.Bytes = pkt_count.Bytes;
    tmp_count.KBytes = pkt_count.KBytes;
    tmp_count.MBytes = pkt_count.MBytes;
    tmp_count.GBytes = pkt_count.GBytes;
    tmp_count.times = pkt_count.times;

    return tmp_count;
}

count_t iperf_diff_count( count_t pkt_count, count_t tmp_count )
{
    /* pkt_count > tmp_count */
    tmp_count.times = pkt_count.times - tmp_count.times;

    if ( pkt_count.Bytes >= tmp_count.Bytes ) {
        tmp_count.Bytes = pkt_count.Bytes - tmp_count.Bytes;
    } else {
        tmp_count.Bytes = pkt_count.Bytes + 1024 - tmp_count.Bytes;
        if ( pkt_count.KBytes > 0 ) {
            pkt_count.KBytes--;
        } else if ( pkt_count.MBytes > 0 ) {
            pkt_count.MBytes--;
            pkt_count.KBytes = 1023;
        } else if ( pkt_count.GBytes > 0 ) {
            pkt_count.GBytes--;
            pkt_count.MBytes = 1023;
            pkt_count.KBytes = 1023;
        } else {
            LWIP_DEBUGF( IPERF_DEBUG, ("Warning: Diff data is wrong." ));
        }
    }

    if ( pkt_count.KBytes >= tmp_count.KBytes ) {
        tmp_count.KBytes = pkt_count.KBytes - tmp_count.KBytes;
    } else {
        tmp_count.KBytes = pkt_count.KBytes + 1024 - tmp_count.KBytes;
        if ( pkt_count.MBytes > 0 ) {
            pkt_count.MBytes--;
        } else if ( pkt_count.GBytes > 0 ) {
            pkt_count.GBytes--;
            pkt_count.MBytes = 1023;
        } else {
            LWIP_DEBUGF( IPERF_DEBUG, ("Warning: Diff data is wrong." ));
        }
    }

    if ( pkt_count.MBytes >= tmp_count.MBytes ) {
        tmp_count.MBytes = pkt_count.MBytes - tmp_count.MBytes;
    } else {
        tmp_count.MBytes = pkt_count.MBytes + 1024 - tmp_count.MBytes;
        if ( pkt_count.GBytes > 0 ) {
            pkt_count.GBytes--;
        } else {
            LWIP_DEBUGF( IPERF_DEBUG, ("Warning: Diff data is wrong." ));
        }
    }

#if defined(IPERF_DEBUG_INTERNAL)
    DBGPRINT_IPERF(IPERF_DEBUG_REPORT, "\niperf_diff_count: ret.times = %d, ret.GBytes = %d, ret.MBytes = %d, ret.KBytes = %d, ret.Bytes = %d",
            tmp_count.times, tmp_count.GBytes, tmp_count.MBytes, tmp_count.KBytes, tmp_count.Bytes);
#endif

    return tmp_count;
}

void iperf_get_current_time( uint32_t *s, uint32_t *ms )
{
    uint64_t now = aos_now_ms();
    if ( s ) {
        *s = (uint32_t)(now/1000);
    }

    if ( ms ) {
        *ms = (uint32_t)(now);
    }
}

void iperf_set_debug_mode( uint32_t debug )
{
    g_iperf_debug_feature = debug;
}

int iperf_format_transform( char *param )
{
    char *temp;
    int win_size = 0;
    int i;

    temp = param;

    for ( i = 0; temp[i] != '\0'; i++ ) {
        if ( temp[i] == 'k' ) {
            temp[i] = '\0';
            win_size = (1000 * atoi( temp ));
        } else if ( temp[i] == 'm' ) {
            temp[i] = '\0';
            win_size = (1000 * 1000 * atoi( temp ));
        } else if ( temp[i] == 'K' ) {
            temp[i] = '\0';
            win_size = 1024 * atoi( temp );
        } else if ( temp[i] == 'M' ) {
            temp[i] = '\0';
            win_size = 1024 * 1024 * atoi( temp );
        } else {
            win_size = atoi( param );
        }
    }

    return win_size;
}

