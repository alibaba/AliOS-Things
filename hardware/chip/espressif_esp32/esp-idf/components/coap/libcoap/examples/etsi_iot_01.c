/* CoAP server for first ETSI CoAP plugtest, March 2012
 *
 * Copyright (C) 2012--2013 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>

#include "uthash.h"
#include "coap.h"

#define COAP_RESOURCE_CHECK_TIME_SEC  1

#ifdef __GNUC__
#define UNUSED_PARAM __attribute__ ((unused))
#else /* not a GCC */
#define UNUSED_PARAM
#endif /* GCC */

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

/* temporary storage for dynamic resource representations */
static int quit = 0;

#define COAP_OPT_BLOCK_SZX_MAX 6 /**< allowed maximum for block szx value */

#define REQUIRE_ETAG 0x01         /* flag for coap_payload_t: require ETag option  */
typedef struct {
  unsigned int flags;             /* some flags to control behavior */
  size_t max_data;                /* maximum size allocated for @p data */
  uint16_t media_type;            /* media type for this object */
  size_t length;                  /* length of data */
  unsigned char data[];           /* the actual contents */
} coap_payload_t;

/* This variable is used to mimic long-running tasks that require
 * asynchronous responses. */
static coap_async_state_t *async = NULL;

/* SIGINT handler: set quit to 1 for graceful termination */
static void
handle_sigint(int signum UNUSED_PARAM) {
  quit = 1;
}

#define INDEX "libcoap server for ETSI CoAP Plugtest, March 2012, Paris\n" \
                 "Copyright (C) 2012 Olaf Bergmann <bergmann@tzi.org>\n\n"

static coap_payload_t *
coap_new_payload(size_t size) {
  coap_payload_t *p;
  p = (coap_payload_t *)coap_malloc(sizeof(coap_payload_t) + size);
  if (p) {
    memset(p, 0, sizeof(coap_payload_t));
    p->max_data = size;
  }

  return p;
}

static inline coap_payload_t *
coap_find_payload(coap_resource_t *resource) {
  coap_payload_t *p = (coap_payload_t *)resource->user_data;
  return p;
}

static inline void
coap_add_payload(coap_resource_t *resource, coap_payload_t *payload){
  assert(payload);

  resource->user_data = payload;
}

static inline void
coap_delete_payload(coap_resource_t *resource) {
  if (resource->user_data) {
    coap_free(resource->user_data);
    resource->user_data = NULL;
  }
}

#if 0
static void
hnd_get_index(coap_context_t *ctx UNUSED_PARAM,
              coap_resource_t *resource UNUSED_PARAM,
              coap_session_t *session UNUSED_PARAM,
              coap_pdu_t *request UNUSED_PARAM,
              coap_binary_t *token UNUSED_PARAM,
              coap_string_t *query UNUSED_PARAM,
              coap_pdu_t *response) {
  unsigned char buf[3];

  response->code = COAP_RESPONSE_CODE(205);

  coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                  coap_encode_var_safe(buf, sizeof(buf),
                                       COAP_MEDIATYPE_TEXT_PLAIN),
                  buf);

  coap_add_option(response, COAP_OPTION_MAXAGE,
          coap_encode_var_safe(buf, sizeof(buf), 0x2ffff), buf);

  coap_add_data(response, strlen(INDEX), (const uint8_t *)INDEX);
}
#endif

static void
hnd_get_resource(coap_context_t *ctx UNUSED_PARAM,
                 coap_resource_t *resource,
                 coap_session_t *session UNUSED_PARAM,
                 coap_pdu_t *request,
                 coap_binary_t *token UNUSED_PARAM,
                 coap_string_t *query UNUSED_PARAM,
                 coap_pdu_t *response) {
  coap_payload_t *test_payload;

  test_payload = coap_find_payload(resource);
  if (!test_payload) {
    response->code = COAP_RESPONSE_CODE(500);

    return;
  }

  response->code = COAP_RESPONSE_CODE(205);

  coap_add_data_blocked_response(resource, session, request, response, token,
                                 test_payload->media_type, -1,
                                 test_payload->length,
                                 test_payload->data);
  return;
}

/* DELETE handler for dynamic resources created by POST /test */
static void
hnd_delete_resource(coap_context_t *ctx,
                    coap_resource_t *resource,
                    coap_session_t *session UNUSED_PARAM,
                    coap_pdu_t *request UNUSED_PARAM,
                    coap_binary_t *token UNUSED_PARAM,
                    coap_string_t *query UNUSED_PARAM,
                    coap_pdu_t *response) {
  coap_payload_t *payload;

  payload = coap_find_payload(resource);

  if (payload)
    coap_delete_payload(resource);

  coap_delete_resource(ctx, resource);

  response->code = COAP_RESPONSE_CODE(202);
}

static void
hnd_post_test(coap_context_t *ctx,
              coap_resource_t *resource UNUSED_PARAM,
              coap_session_t *session UNUSED_PARAM,
              coap_pdu_t *request,
              coap_binary_t *token UNUSED_PARAM,
              coap_string_t *query UNUSED_PARAM,
              coap_pdu_t *response) {
  coap_opt_iterator_t opt_iter;
  coap_opt_t *option;
  coap_payload_t *test_payload;
  size_t len;
  coap_str_const_t *uri;
  unsigned char *data;

#define BUFSIZE 20
  int res;
  unsigned char _buf[BUFSIZE];
  unsigned char *buf = _buf;
  size_t buflen = BUFSIZE;

  coap_get_data(request, &len, &data);

  /* allocate storage for resource and to hold URI */
  test_payload = coap_new_payload(len);
  snprintf((char *)buf, buflen, "test/%p", (void *)test_payload);
  uri = coap_new_str_const(buf, strlen((char *)buf));
  if (!(test_payload && uri)) {
    coap_log(LOG_CRIT, "cannot allocate new resource under /test");
    response->code = COAP_RESPONSE_CODE(500);
    coap_free(test_payload);
    coap_free(uri);
  } else {
    coap_resource_t *r;

    test_payload->length = len;

    memcpy(test_payload->data, data, len);

    r = coap_resource_init(uri, COAP_RESOURCE_FLAGS_RELEASE_URI);
    coap_register_handler(r, COAP_REQUEST_GET, hnd_get_resource);
    coap_register_handler(r, COAP_REQUEST_DELETE, hnd_delete_resource);

    /* set media_type if available */
    option = coap_check_option(request, COAP_OPTION_CONTENT_TYPE, &opt_iter);
    if (option) {
      test_payload->media_type =
        coap_decode_var_bytes(coap_opt_value(option), coap_opt_length(option));
    }

    coap_add_resource(ctx, r);
    coap_add_payload(r, test_payload);

    /* add Location-Path */
    res = coap_split_path(uri->s, uri->length, buf, &buflen);

    while (res--) {
      coap_add_option(response, COAP_OPTION_LOCATION_PATH,
                      coap_opt_length(buf), coap_opt_value(buf));

      buf += coap_opt_size(buf);
    }

    response->code = COAP_RESPONSE_CODE(201);
  }

}

static void
hnd_put_test(coap_context_t *ctx UNUSED_PARAM,
             coap_resource_t *resource,
             coap_session_t *session UNUSED_PARAM,
             coap_pdu_t *request,
             coap_binary_t *token UNUSED_PARAM,
             coap_string_t *query UNUSED_PARAM,
             coap_pdu_t *response) {
  coap_opt_iterator_t opt_iter;
  coap_opt_t *option;
  coap_payload_t *payload;
  size_t len;
  unsigned char *data;

  response->code = COAP_RESPONSE_CODE(204);

  coap_get_data(request, &len, &data);

  payload = coap_find_payload(resource);
  if (payload && payload->max_data < len) { /* need more storage */
    coap_delete_payload(resource);
    payload = NULL;
    /* bug: when subsequent coap_new_payload() fails, our old contents
       is gone */
  }

  if (!payload) {                /* create new payload */
    payload = coap_new_payload(len);
    if (!payload)
      goto error;

    coap_add_payload(resource, payload);
  }
  payload->length = len;
  memcpy(payload->data, data, len);

  option = coap_check_option(request, COAP_OPTION_CONTENT_TYPE, &opt_iter);
  if (option) {
    /* set media type given in request */
    payload->media_type =
      coap_decode_var_bytes(coap_opt_value(option), coap_opt_length(option));
  } else {
    /* set default value */
    payload->media_type = COAP_MEDIATYPE_TEXT_PLAIN;
  }
  /* FIXME: need to change attribute ct of resource.
     To do so, we need dynamic management of the attribute value
  */

  return;
 error:
  coap_log(LOG_WARNING, "cannot modify resource\n");
  response->code = COAP_RESPONSE_CODE(500);
}

static void
hnd_delete_test(coap_context_t *ctx UNUSED_PARAM,
                coap_resource_t *resource UNUSED_PARAM,
                coap_session_t *session UNUSED_PARAM,
                coap_pdu_t *request UNUSED_PARAM,
                coap_binary_t *token UNUSED_PARAM,
                coap_string_t *query UNUSED_PARAM,
                coap_pdu_t *response) {
  /* the ETSI validation tool does not like empty resources... */
#if 0
  coap_payload_t *payload;
  payload = coap_find_payload(resource);

  if (payload)
    payload->length = 0;
#endif

  response->code = COAP_RESPONSE_CODE(202);
}

static void
hnd_get_query(coap_context_t *ctx UNUSED_PARAM,
              coap_resource_t *resource UNUSED_PARAM,
              coap_session_t *session UNUSED_PARAM,
              coap_pdu_t *request,
              coap_binary_t *token UNUSED_PARAM,
              coap_string_t *query UNUSED_PARAM,
              coap_pdu_t *response) {
  coap_opt_iterator_t opt_iter;
  coap_opt_filter_t f;
  coap_opt_t *q;
  size_t len, L;
  unsigned char buf[70];

  response->code = COAP_RESPONSE_CODE(205);

  coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                  coap_encode_var_safe(buf, sizeof(buf),
                                       COAP_MEDIATYPE_TEXT_PLAIN),
                  buf);

  coap_option_filter_clear(f);
  coap_option_setb(f, COAP_OPTION_URI_QUERY);

  coap_option_iterator_init(request, &opt_iter, f);

  len = 0;
  while ((len < sizeof(buf)) && (q = coap_option_next(&opt_iter))) {
    L = min(sizeof(buf) - len, 11);
    memcpy(buf + len, "Uri-Query: ", L);
    len += L;

    L = min(sizeof(buf) - len, coap_opt_length(q));
    memcpy(buf + len, coap_opt_value(q), L);
    len += L;

    if (len < sizeof(buf))
      buf[len++] = '\n';
  }

  coap_add_data(response, len, buf);
}

/* handler for TD_COAP_CORE_16 */
static void
hnd_get_separate(coap_context_t *ctx,
                 coap_resource_t *resource UNUSED_PARAM,
                 coap_session_t *session,
                 coap_pdu_t *request,
                 coap_binary_t *token UNUSED_PARAM,
                 coap_string_t *query UNUSED_PARAM,
                 coap_pdu_t *response) {
  coap_opt_iterator_t opt_iter;
  coap_opt_t *option;
  coap_opt_filter_t f;
  unsigned long delay = 5;

  if (async) {
    if (async->id != request->tid) {
      coap_option_filter_clear(f);
      response->code = COAP_RESPONSE_CODE(503);
    }
    return;
  }

  /* search for option delay in query list */
  coap_option_filter_clear(f);
  coap_option_setb(f, COAP_OPTION_URI_QUERY);

  coap_option_iterator_init(request, &opt_iter, f);

  while ((option = coap_option_next(&opt_iter))) {
    if (strncmp("delay=", (const char *)coap_opt_value(option), 6) == 0) {
      int i;
      unsigned long d = 0;

      for (i = 6; i < coap_opt_length(option); ++i)
        d = d * 10 + coap_opt_value(option)[i] - '0';

      /* don't allow delay to be less than COAP_RESOURCE_CHECK_TIME*/
      delay = d < COAP_RESOURCE_CHECK_TIME_SEC
        ? COAP_RESOURCE_CHECK_TIME_SEC
        : d;
      coap_log(LOG_DEBUG, "set delay to %lu\n", delay);
      break;
    }
  }

  async = coap_register_async(ctx, session, request, COAP_ASYNC_SEPARATE,
                              (void *)(COAP_TICKS_PER_SECOND * delay));
}

static void
check_async(coap_context_t *ctx,
            coap_tick_t now) {
  coap_pdu_t *response;
  coap_async_state_t *tmp;

  size_t size = 8;

  if (!async || now < async->created + (unsigned long)async->appdata)
    return;

  response = coap_pdu_init(async->flags & COAP_ASYNC_CONFIRM
                           ? COAP_MESSAGE_CON
                           : COAP_MESSAGE_NON,
                           COAP_RESPONSE_CODE(205), 0, size);
  if (!response) {
    coap_log(LOG_DEBUG, "check_async: insufficient memory, we'll try later\n");
    async->appdata =
      (void *)((unsigned long)async->appdata + 15 * COAP_TICKS_PER_SECOND);
    return;
  }

  response->tid = coap_new_message_id(async->session);

  if (async->tokenlen)
    coap_add_token(response, async->tokenlen, async->token);

  coap_add_data(response, 4, (const uint8_t *)"done");

  if (coap_send(async->session, response) == COAP_INVALID_TID) {
    coap_log(LOG_DEBUG, "check_async: cannot send response for message %d\n",
          response->tid);
  }
  coap_remove_async(ctx, async->session, async->id, &tmp);
  coap_free_async(async);
  async = NULL;
}

static coap_payload_t *
make_large(const char *filename) {
  coap_payload_t *payload;
  FILE *inputfile = NULL;
  struct stat statbuf;

  if (!filename)
    return NULL;

  /* read from specified input file */
  if (stat(filename, &statbuf) < 0) {
    coap_log(LOG_WARNING, "cannot stat file %s\n", filename);
    return NULL;
  }

  payload = coap_new_payload(statbuf.st_size);
  if (!payload)
    return NULL;

  inputfile = fopen(filename, "r");
  if ( !inputfile ) {
    coap_log(LOG_WARNING, "cannot read file %s\n", filename);
    coap_free(payload);
    return NULL;
  }

  payload->length = fread(payload->data, 1, statbuf.st_size, inputfile);
  payload->media_type = 41;

  fclose(inputfile);

  return payload;
}

static void
init_resources(coap_context_t *ctx) {
  coap_resource_t *r;
  coap_payload_t *test_payload;

  test_payload = coap_new_payload(200);
  if (!test_payload)
    coap_log(LOG_CRIT, "cannot allocate resource /test");
  else {
    test_payload->length = 13;
    memcpy(test_payload->data, "put data here", test_payload->length);
    /* test_payload->media_type is 0 anyway */

    r = coap_resource_init(coap_make_str_const("test"), 0);
    coap_register_handler(r, COAP_REQUEST_GET, hnd_get_resource);
    coap_register_handler(r, COAP_REQUEST_POST, hnd_post_test);
    coap_register_handler(r, COAP_REQUEST_PUT, hnd_put_test);
    coap_register_handler(r, COAP_REQUEST_DELETE, hnd_delete_test);

    coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("0"), 0);
    coap_add_attr(r, coap_make_str_const("rt"), coap_make_str_const("test"), 0);
    coap_add_attr(r, coap_make_str_const("if"), coap_make_str_const("core#b"), 0);
#if 0
    coap_add_attr(r, coap_make_str_const("obs"), NULL, 0);
#endif
    coap_add_resource(ctx, r);
    coap_add_payload(r, test_payload);
  }

  /* TD_COAP_BLOCK_01
   * TD_COAP_BLOCK_02 */
  test_payload = make_large("etsi_iot_01_largedata.txt");
  if (!test_payload)
    coap_log(LOG_CRIT, "cannot allocate resource /large\n");
  else {
    r = coap_resource_init(coap_make_str_const("large"), 0);
    coap_register_handler(r, COAP_REQUEST_GET, hnd_get_resource);

    coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("41"), 0);
    coap_add_attr(r, coap_make_str_const("rt"), coap_make_str_const("large"), 0);
    coap_add_resource(ctx, r);

    test_payload->flags |= REQUIRE_ETAG;

    coap_add_payload(r, test_payload);
  }

  /* For TD_COAP_CORE_12 */
  test_payload = coap_new_payload(20);
  if (!test_payload)
    coap_log(LOG_CRIT, "cannot allocate resource /seg1/seg2/seg3\n");
  else {
    test_payload->length = 10;
    memcpy(test_payload->data, "segsegseg!", test_payload->length);
    /* test_payload->media_type is 0 anyway */

    r = coap_resource_init(coap_make_str_const("seg1/seg2/seg3"), 0);
    coap_register_handler(r, COAP_REQUEST_GET, hnd_get_resource);

    coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("0"), 0);
    coap_add_resource(ctx, r);

    coap_add_payload(r, test_payload);
  }

  /* For TD_COAP_CORE_13 */
  r = coap_resource_init(coap_make_str_const("query"), 0);
  coap_register_handler(r, COAP_REQUEST_GET, hnd_get_query);

  coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("0"), 0);
  coap_add_resource(ctx, r);

  /* For TD_COAP_CORE_16 */
  r = coap_resource_init(coap_make_str_const("separate"), 0);
  coap_register_handler(r, COAP_REQUEST_GET, hnd_get_separate);

  coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("0"), 0);
  coap_add_attr(r, coap_make_str_const("rt"), coap_make_str_const("seperate"), 0);
  coap_add_resource(ctx, r);
}

static void
usage( const char *program, const char *version) {
  const char *p;

  p = strrchr( program, '/' );
  if ( p )
    program = ++p;

  fprintf( stderr, "%s v%s -- ETSI CoAP plugtest server\n"
           "(c) 2012 Olaf Bergmann <bergmann@tzi.org>\n\n"
           "usage: %s [-A address] [-p port]\n\n"
           "\t-A address\tinterface address to bind to\n"
           "\t-p port\t\tlisten on specified port\n"
           "\t-v num\t\tverbosity level (default: 3)\n",
           program, version, program );
}

static coap_context_t *
get_context(const char *node, const char *port) {
  coap_context_t *ctx = NULL;
  int s;
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_DGRAM; /* Coap uses UDP */
  hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;

  s = getaddrinfo(node, port, &hints, &result);
  if ( s != 0 ) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    return NULL;
  }

  /* iterate through results until success */
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    coap_address_t addr;

    if (rp->ai_addrlen <= sizeof(addr.addr)) {
      coap_address_init(&addr);
      addr.size = rp->ai_addrlen;
      memcpy(&addr.addr, rp->ai_addr, rp->ai_addrlen);

      ctx = coap_new_context(&addr);
      if (ctx) {
        /* TODO: output address:port for successful binding */
        goto finish;
      }
    }
  }

  fprintf(stderr, "no context available for interface '%s'\n", node);

 finish:
  freeaddrinfo(result);
  return ctx;
}

int
main(int argc, char **argv) {
  coap_context_t  *ctx;
  int result;
  coap_tick_t now;
  char addr_str[NI_MAXHOST] = "::";
  char port_str[NI_MAXSERV] = "5683";
  int opt;
  coap_log_t log_level = LOG_WARNING;
  struct sigaction sa;

  while ((opt = getopt(argc, argv, "A:p:v:")) != -1) {
    switch (opt) {
    case 'A' :
      strncpy(addr_str, optarg, NI_MAXHOST-1);
      addr_str[NI_MAXHOST - 1] = '\0';
      break;
    case 'p' :
      strncpy(port_str, optarg, NI_MAXSERV-1);
      port_str[NI_MAXSERV - 1] = '\0';
      break;
    case 'v' :
      log_level = strtol(optarg, NULL, 10);
      break;
    default:
      usage( argv[0], LIBCOAP_PACKAGE_VERSION );
      exit( 1 );
    }
  }

  coap_set_log_level(log_level);

  ctx = get_context(addr_str, port_str);
  if (!ctx)
    return -1;

  coap_register_option(ctx, COAP_OPTION_BLOCK2);

  init_resources(ctx);

  memset (&sa, 0, sizeof(sa));
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = handle_sigint;
  sa.sa_flags = 0;
  sigaction (SIGINT, &sa, NULL);
  sigaction (SIGTERM, &sa, NULL);

  while ( !quit ) {
    result = coap_run_once( ctx, COAP_RESOURCE_CHECK_TIME * 1000 );
    if ( result >= 0 ) {
      /* coap_check_resource_list( ctx ); */
    }
    /* check if we have to send asynchronous responses */
    coap_ticks(&now);
    check_async(ctx, now);
  }

  RESOURCES_ITER(ctx->resources, r) {
    coap_delete_payload(r);
  }
  coap_free_context( ctx );
  coap_cleanup();

  return 0;
}
