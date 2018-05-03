/*
 * iperf, Copyright (c) 2014, The Regents of the University of
 * California, through Lawrence Berkeley National Laboratory (subject
 * to receipt of any required approvals from the U.S. Dept. of
 * Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE.  This software is owned by the U.S. Department of Energy.
 * As such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * This code is distributed under a BSD style license, see the LICENSE
 * file for complete information.
 */
#ifndef __IPERF_H
#define __IPERF_H

#include "iperf_config.h"

//#include <sys/time.h>
//#include <sys/types.h>
//#ifdef HAVE_STDINT_H
//#include <stdint.h>
//#endif
#include "lwip/sockets.h"
#include "lwip/tcp.h"

#include "queue.h"
//#include "cJSON.h"
#include "timer.h"

/* cJSON Types: */
#define Iperf_cJSON_False 0
#define Iperf_cJSON_True 1
#define Iperf_cJSON_NULL 2
#define Iperf_cJSON_Number 3
#define Iperf_cJSON_String 4
#define Iperf_cJSON_Array 5
#define Iperf_cJSON_Object 6
	
#define Iperf_cJSON_IsReference 256

/* The cJSON structure: */
typedef struct Iperf_cJSON {
	struct Iperf_cJSON *next, *prev;	/* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	struct Iperf_cJSON *child;		/* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

	int type;			/* The type of the item, as above. */

	char *valuestring;		/* The item's string, if type==cJSON_String */

#ifdef IPERF_SUPPORT_DOUBLE
    double valuefloat;
#endif
    iperf_ssize_t valueint;   /* The item's number, if type==cJSON_Number */

	char *string;			/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} Iperf_cJSON;

typedef struct Iperf_cJSON_Hooks {
      void *(*malloc_fn)(size_t sz );
      void (*free_fn)( void *ptr );
} Iperf_cJSON_Hooks;
//#define IPERF_DEBUG_MEM
void *iperf_malloc(size_t size);
void iperf_free(void *ptr);

struct iperf_interval_results
{
    iperf_size_t bytes_transferred; /* bytes transfered in this interval */
    struct timeval interval_start_time;
    struct timeval interval_end_time;
    int       end_time;
    u32       interval_duration;

    /* for UDP */
    int       interval_packet_count;
    int       interval_outoforder_packets;
    int       interval_cnt_error;
    u32       packet_count;
    iperf_jit_t  jitter;
    iperf_jit_t  total_jitter;
    int       outoforder_packets;
    int       cnt_error;

    int omitted;

    /* Just placeholders, never accessed. */
    char *tcpInfo;
    int interval_retrans;
    int interval_sacks;
    int snd_cwnd;
    TAILQ_ENTRY(iperf_interval_results) irlistentries;
    void     *custom_data;
    int rtt;
};

struct iperf_stream_result
{
    iperf_size_t bytes_received;
    iperf_size_t bytes_sent;
    iperf_size_t bytes_received_this_interval;
    iperf_size_t bytes_sent_this_interval;
    int stream_prev_total_retrans;
    int stream_retrans;
    int stream_prev_total_sacks;
    int stream_sacks;
    int stream_max_rtt;
    int stream_min_rtt;
    int stream_sum_rtt;
    int stream_count_rtt;
    int stream_max_snd_cwnd;
    struct timeval start_time;
    struct timeval end_time;
    TAILQ_HEAD(irlisthead, iperf_interval_results) interval_results;
    void     *data;
};

#define COOKIE_SIZE 37		/* size of an ascii uuid */
struct iperf_settings
{
    int       domain;               /* AF_INET or AF_INET6 */
    int       socket_bufsize;       /* window size for TCP */
    int       blksize;              /* size of read/writes (-l) */
    u32       rate;                 /* target data rate */
    int       burst;                /* packets per burst */
    int       mss;                  /* for TCP MSS */
    int       ttl;                  /* IP TTL option */
    int       tos;                  /* type of service bit */
    int       flowlabel;            /* IPv6 flow label */
    iperf_size_t bytes;             /* number of bytes to send */
    iperf_size_t blocks;            /* number of blocks (packets) to send */
    char      unit_format;          /* -f */
    char      rsvd[3];
    int       num_ostreams;         /* SCTP initmsg settings */
};

struct iperf_test;

struct iperf_stream
{
    struct iperf_test* test;

    /* configurable members */
    int       local_port;
    int       remote_port;
    int       socket;
    int       id;
	/* XXX: is settings just a pointer to the same struct in iperf_test? if not, 
		should it be? */
    struct iperf_settings *settings;	/* pointer to structure settings */

    /* non configurable members */
    struct iperf_stream_result *result;	/* structure pointer to result */
    Timer     *send_timer;
    int       green_light;
    char      *buffer;		/* data to send, mmapped */

    /*
     * for udp measurements - This can be a structure outside stream, and
     * stream can have a pointer to this
     */
    u32       packet_count;
    int       omitted_packet_count;
    iperf_jit_t   jitter;
    struct timeval  prev_transit;
    struct timeval  prev_receive;
    u32       outoforder_packets;
    u32       cnt_error;
    //struct sockaddr_storage local_addr;
    //struct sockaddr_storage remote_addr;
    struct sockaddr_in local_addr;   
    struct sockaddr_in remote_addr;  
    
    
    int       (*rcv) (struct iperf_stream * stream);
    int       (*snd) (struct iperf_stream * stream);

    /* chained send/receive routines for -F mode */
    int       (*rcv2) (struct iperf_stream * stream);
    int       (*snd2) (struct iperf_stream * stream);

//    struct iperf_stream *next;
    SLIST_ENTRY(iperf_stream) streams;

    void     *data;
};

struct protocol {
    int       id;
    char      *name;
    int       (*sock_accept)(struct iperf_test *);
    int       (*sock_listen)(struct iperf_test *);
    int       (*sock_connect)(struct iperf_test *);
    int       (*sock_send)(struct iperf_stream *);
    int       (*sock_recv)(struct iperf_stream *);
    int       (*init)(struct iperf_test *);
    SLIST_ENTRY(protocol) protocols;
};

struct iperf_textline {
    char *line;
    TAILQ_ENTRY(iperf_textline) textlineentries;
};

struct xbind_entry {
    char *name;
    struct addrinfo *ai;
    TAILQ_ENTRY(xbind_entry) link;
};

struct iperf_test
{
    int       sender;                           /* client & !reverse or server & reverse */
    int       sender_has_retransmits;
    struct protocol *protocol;
    char     *server_hostname;                  /* -c option */
    char     *bind_address;                     /* first -B option */
    int       bind_port;                        /* --cport option */
    int       server_port;
    int       omit;                             /* duration of omit period (-O flag) */
    int       duration;                         /* total duration of test (-t flag) */
    char     *title;				/* -T option */
    char     *congestion;			/* -C option */

    int       ctrl_sck;
    int       listener;
    int       prot_listener;

    int       t_errno;
    int       i_errno;

    /* boolean variables for Options */
    int       daemon;                           /* -D option */
    int       no_delay;                         /* -N option */
    int       reverse;                          /* -R option */
    int	      verbose;                          /* -V option - verbose mode */
    int	      zerocopy;                         /* -Z option - use sendfile */
    int       debug;				/* -d option - enable debug */
    int	      get_server_output;		/* --get-server-output */

    int	      multisend;

    /* Select related parameters */
    int       max_fd;
    char      role;                             /* 'c' lient or 's' erver */
    signed char state;
    fd_set    read_set;                         /* set of read sockets */
    fd_set    write_set;                        /* set of write sockets */

    /* Interval related members */ 
    Timer     *timers;
    Timer     *free_timers;
    int       omitting;
    u32       stats_interval;
    u32       reporter_interval;
    void      (*stats_callback) (struct iperf_test *);
    void      (*reporter_callback) (struct iperf_test *);
    Timer     *omit_timer;
    Timer     *timer;
    int        done;
    Timer     *stats_timer;
    Timer     *reporter_timer;

    int       num_streams;                      /* total streams in the test (-P) */
    int       periods;

    iperf_size_t bytes_sent;
    iperf_size_t blocks_sent;
    char      cookie[COOKIE_SIZE];
    char      rsvd[3];
//    struct iperf_stream *streams;               /* pointer to list of struct stream */
    SLIST_HEAD(slisthead, iperf_stream) streams;
    struct iperf_settings *settings;

    SLIST_HEAD(plisthead, protocol) protocols;

    /* callback functions */
    void      (*on_new_stream)(struct iperf_stream *);
    void      (*on_test_start)(struct iperf_test *);
    void      (*on_connect)(struct iperf_test *);
    void      (*on_test_finish)(struct iperf_test *);

    /* cJSON handles for use when in -J mode */
    Iperf_cJSON *json_top;
    Iperf_cJSON *json_start;
    Iperf_cJSON *json_connected;
    Iperf_cJSON *json_intervals;
    Iperf_cJSON *json_end;

    /* Server output (use on client side only) */
    char *server_output_text;
    Iperf_cJSON *json_server_output;

    /* Server output (use on server side only) */
    TAILQ_HEAD(iperf_textlisthead, iperf_textline) server_output_list;

};

/* default settings */
#define PORT 5201  /* default port to listen on (iperf2 default port: 5001, iperf3 5201) */
#define uS_TO_NS 1000
#define SEC_TO_MS 1000
#define SEC_TO_US 1000000L
#define UDP_RATE (1024 * 1024 * 20) /* 1 Mbps */
#define OMIT 0 /* seconds */
#define DURATION 10 /* seconds */

#define SEC_TO_NS 1000000000LL	/* too big for enum/const on some platforms */
#define MAX_RESULT_STRING 4096

/* constants for command line arg sanity checks */
#define MB (1024 * 1024)
#define MAX_TCP_BUFFER (512 * MB)
#define MAX_BLOCKSIZE (16 * 1024) /* MB */
#define MIN_BLOCKSIZE 12
/* Maximum size UDP send is (64K - 1) - IP and UDP header sizes */
/* function pbuf_alloc, udp size must be less than PBUF_POOL_BUFSIZE_ALIGNED - LWIP_MEM_ALIGN_SIZE(offset + SIZEOF_STRUCT_PBUF) */
#define MAX_UDP_BLOCKSIZE (65535 - 8 - 20)/* 1472 */
#define MIN_INTERVAL 0.1
#define MAX_INTERVAL 60.0
#define MAX_TIME (86400*20)
#define MAX_BURST 1000
#define MAX_MSS (9 * 1024)
#define MAX_STREAMS 128
#define MAX_BANDWIDTH (1024*MB)
#define MAX_OMIT 60

#endif /* !__IPERF_H */
