/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 * -*- */

/* coap -- simple implementation of the Constrained Application Protocol (CoAP)
 *         as defined in RFC 7252
 *
 * Copyright (C) 2010--2015 Olaf Bergmann <bergmann@tzi.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see README for terms of
 * use.
 */


/**
 * @file coap-rd.c
 * @brief CoRE resource directory
 *
 * @see https://tools.ietf.org/html/draft-ietf-core-resource-directory
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#ifdef _WIN32
#define strcasecmp _stricmp
#include "getopt.c"
#if !defined(S_ISDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#else
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#endif

#include <coap3/coap.h>

#define COAP_RESOURCE_CHECK_TIME 2

#define RD_ROOT_STR   "rd"
#define RD_ROOT_SIZE  2

static char *cert_file = NULL; /* Combined certificate and private key in PEM */
static char *ca_file = NULL;   /* CA for cert_file - for cert checking in PEM */
static char *root_ca_file = NULL; /* List of trusted Root CAs in PEM */
static int verify_peer_cert = 1; /* PKI granularity - by default set */
#define MAX_KEY   64 /* Maximum length of a pre-shared key in bytes. */
static uint8_t key[MAX_KEY];
static ssize_t key_length = 0;
static int key_defined = 0;
static const char *hint = "CoAP";

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

typedef struct rd_t {
  size_t etag_len;        /**< actual length of @c etag */
  unsigned char etag[8];  /**< ETag for current description */

  coap_string_t data;     /**< points to the resource description  */
} rd_t;

rd_t *resources = NULL;

static ssize_t
cmdline_read_key(char *arg, unsigned char *buf, size_t maxlen) {
  size_t len = strnlen(arg, maxlen);
  if (len) {
    memcpy(buf, arg, len);
    return len;
  }
  return -1;
}

static inline rd_t *
rd_new(void) {
  rd_t *rd;
  rd = (rd_t *)coap_malloc(sizeof(rd_t));
  if (rd)
    memset(rd, 0, sizeof(rd_t));

  return rd;
}

static void
rd_delete(rd_t *rd) {
  if (rd) {
    coap_free(rd->data.s);
    coap_free(rd);
  }
}

static void
resource_rd_delete(void *ptr) {
  rd_delete(ptr);
}

static int quit = 0;

/* SIGINT handler: set quit to 1 for graceful termination */
static void
handle_sigint(int signum COAP_UNUSED) {
  quit = 1;
}

static void
hnd_get_resource(coap_resource_t *resource,
                 coap_session_t *session COAP_UNUSED,
                 const coap_pdu_t *request COAP_UNUSED,
                 const coap_string_t *query COAP_UNUSED,
                 coap_pdu_t *response) {
  rd_t *rd = coap_resource_get_userdata(resource);
  unsigned char buf[3];

  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);

  coap_add_option(response,
                  COAP_OPTION_CONTENT_TYPE,
                  coap_encode_var_safe(buf, sizeof(buf),
                                       COAP_MEDIATYPE_APPLICATION_LINK_FORMAT),
                                       buf);

  if (rd && rd->etag_len)
    coap_add_option(response, COAP_OPTION_ETAG, rd->etag_len, rd->etag);

  if (rd && rd->data.s)
    coap_add_data(response, rd->data.length, rd->data.s);
}

static void
hnd_put_resource(coap_resource_t *resource COAP_UNUSED,
                 coap_session_t *session COAP_UNUSED,
                 const coap_pdu_t *request COAP_UNUSED,
                 const coap_string_t *query COAP_UNUSED,
                 coap_pdu_t *response) {
#if 1
  coap_pdu_set_code(response, COAP_RESPONSE_CODE_NOT_IMPLEMENTED);
#else /* FIXME */
  coap_opt_iterator_t opt_iter;
  coap_opt_t *token, *etag;
  coap_pdu_t *response;
  size_t size = sizeof(coap_hdr_t);
  int type = (request->hdr->type == COAP_MESSAGE_CON)
    ? COAP_MESSAGE_ACK : COAP_MESSAGE_NON;
  rd_t *rd = NULL;
  unsigned char code;     /* result code */
  const uint8_t *data;
  coap_string_t tmp;

  HASH_FIND(hh, resources, resource->uri_path.s, resource->uri_path.length, rd);
  if (rd) {
    /* found resource object, now check Etag */
    etag = coap_check_option(request, COAP_OPTION_ETAG, &opt_iter);
    if (!etag || (COAP_OPT_LENGTH(etag) != rd->etag_len)
        || memcmp(COAP_OPT_VALUE(etag), rd->etag, rd->etag_len) != 0) {

      if (coap_get_data(request, &tmp.length, &data)) {

        tmp.s = (unsigned char *)coap_malloc(tmp.length);
        if (!tmp.s) {
          coap_log(LOG_DEBUG,
                   "hnd_put_rd: cannot allocate storage for new rd\n");
          code = COAP_RESPONSE_CODE_SERVICE_UNAVAILABLE;
          goto finish;
        }

        coap_free(rd->data.s);
        rd->data.s = tmp.s;
        rd->data.length = tmp.length;
        memcpy(rd->data.s, data, rd->data.length);
      }
    }

    if (etag) {
      rd->etag_len = min(COAP_OPT_LENGTH(etag), sizeof(rd->etag));
      memcpy(rd->etag, COAP_OPT_VALUE(etag), rd->etag_len);
    }

    code = COAP_RESPONSE_CODE_CHANGED;
    /* FIXME: update lifetime */

    } else {

    code = COAP_RESPONSE_CODE_SERVICE_UNAVAILABLE;
  }

  finish:
  /* FIXME: do not create a new response but use the old one instead */
  response = coap_pdu_init(type, code, request->hdr->id, size);

  if (!response) {
    coap_log(LOG_DEBUG, "cannot create response for mid=0x%x\n",
             request->hdr->id);
    return;
  }

  if (request->hdr->token_length)
    coap_add_token(response, request->hdr->token_length, request->hdr->token);

  if (coap_send(ctx, peer, response) == COAP_INVALID_MID) {
    coap_log(LOG_DEBUG, "hnd_get_rd: cannot send response for mid=0x%x\n",
    request->hdr->id);
  }
#endif
}

static void
hnd_delete_resource(coap_resource_t *resource,
                    coap_session_t *session COAP_UNUSED,
                    const coap_pdu_t *request COAP_UNUSED,
                    const coap_string_t *query COAP_UNUSED,
                    coap_pdu_t *response) {
  rd_t *rd = coap_resource_get_userdata(resource);

  if (rd) {
    rd_delete(rd);
  }
  /* FIXME: link attributes for resource have been created dynamically
   * using coap_malloc() and must be released. */
  coap_delete_resource(coap_session_get_context(session), resource);

  coap_pdu_set_code(response, COAP_RESPONSE_CODE_DELETED);
}

static void
hnd_get_rd(coap_resource_t *resource COAP_UNUSED,
           coap_session_t *session COAP_UNUSED,
           const coap_pdu_t *request COAP_UNUSED,
           const coap_string_t *query COAP_UNUSED,
           coap_pdu_t *response) {
  unsigned char buf[3];

  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);

  coap_add_option(response,
                  COAP_OPTION_CONTENT_TYPE,
                  coap_encode_var_safe(buf, sizeof(buf),
                                       COAP_MEDIATYPE_APPLICATION_LINK_FORMAT),
                                       buf);

  coap_add_option(response,
                  COAP_OPTION_MAXAGE,
                  coap_encode_var_safe(buf, sizeof(buf), 0x2ffff), buf);
}

static int
parse_param(const uint8_t *search,
            size_t search_len,
            unsigned char *data,
            size_t data_len,
            coap_string_t *result) {

  if (result)
    memset(result, 0, sizeof(coap_string_t));

  if (!search_len)
    return 0;

  while (search_len <= data_len) {

    /* handle parameter if found */
    if (memcmp(search, data, search_len) == 0) {
      data += search_len;
      data_len -= search_len;

      /* key is only valid if we are at end of string or delimiter follows */
      if (!data_len || *data == '=' || *data == '&') {
        while (data_len && *data != '=') {
          ++data; --data_len;
        }

        if (data_len > 1 && result) {
          /* value begins after '=' */

          result->s = ++data;
          while (--data_len && *data != '&') {
            ++data; result->length++;
          }
        }

        return 1;
      }
    }

    /* otherwise proceed to next */
    while (--data_len && *data++ != '&')
      ;
  }

  return 0;
}

static void
add_source_address(coap_resource_t *resource,
                   const coap_address_t *peer) {
#define BUFSIZE 64
  char *buf;
  size_t n = 1;
  coap_str_const_t attr_val;

  buf = (char *)coap_malloc(BUFSIZE);
  if (!buf)
    return;

  buf[0] = '"';

  switch(peer->addr.sa.sa_family) {

  case AF_INET:
    /* FIXME */
    break;

  case AF_INET6:
    n += snprintf(buf + n, BUFSIZE - n,
      "[%02x%02x:%02x%02x:%02x%02x:%02x%02x" \
      ":%02x%02x:%02x%02x:%02x%02x:%02x%02x]",
      peer->addr.sin6.sin6_addr.s6_addr[0],
      peer->addr.sin6.sin6_addr.s6_addr[1],
      peer->addr.sin6.sin6_addr.s6_addr[2],
      peer->addr.sin6.sin6_addr.s6_addr[3],
      peer->addr.sin6.sin6_addr.s6_addr[4],
      peer->addr.sin6.sin6_addr.s6_addr[5],
      peer->addr.sin6.sin6_addr.s6_addr[6],
      peer->addr.sin6.sin6_addr.s6_addr[7],
      peer->addr.sin6.sin6_addr.s6_addr[8],
      peer->addr.sin6.sin6_addr.s6_addr[9],
      peer->addr.sin6.sin6_addr.s6_addr[10],
      peer->addr.sin6.sin6_addr.s6_addr[11],
      peer->addr.sin6.sin6_addr.s6_addr[12],
      peer->addr.sin6.sin6_addr.s6_addr[13],
      peer->addr.sin6.sin6_addr.s6_addr[14],
      peer->addr.sin6.sin6_addr.s6_addr[15]);

    if (peer->addr.sin6.sin6_port != htons(COAP_DEFAULT_PORT)) {
      n +=
      snprintf(buf + n, BUFSIZE - n, ":%d", peer->addr.sin6.sin6_port);
    }
    break;
    default:
    ;
  }

  if (n < BUFSIZE)
    buf[n++] = '"';

  attr_val.s = (const uint8_t *)buf;
  attr_val.length = n;
  coap_add_attr(resource,
                coap_make_str_const("A"),
                &attr_val,
                0);
  coap_free(buf);
#undef BUFSIZE
}

static rd_t *
make_rd(const coap_pdu_t *pdu) {
  rd_t *rd;
  const uint8_t *data;
  coap_opt_iterator_t opt_iter;
  coap_opt_t *etag;

  rd = rd_new();

  if (!rd) {
    coap_log(LOG_DEBUG, "hnd_get_rd: cannot allocate storage for rd\n");
    return NULL;
  }

  if (coap_get_data(pdu, &rd->data.length, &data)) {
    rd->data.s = (unsigned char *)coap_malloc(rd->data.length);
    if (!rd->data.s) {
      coap_log(LOG_DEBUG, "hnd_get_rd: cannot allocate storage for rd->data\n");
      rd_delete(rd);
      return NULL;
    }
    memcpy(rd->data.s, data, rd->data.length);
  }

  etag = coap_check_option(pdu, COAP_OPTION_ETAG, &opt_iter);
  if (etag) {
    rd->etag_len = min(coap_opt_length(etag), sizeof(rd->etag));
    memcpy(rd->etag, coap_opt_value(etag), rd->etag_len);
  }

  return rd;
}

static void
hnd_post_rd(coap_resource_t *resource COAP_UNUSED,
            coap_session_t *session,
            const coap_pdu_t *request,
            const coap_string_t *query COAP_UNUSED,
            coap_pdu_t *response) {
  coap_resource_t *r;
#define LOCSIZE 68
  unsigned char *loc;
  size_t loc_size;
  coap_string_t h = {0, NULL}, ins = {0, NULL}, rt = {0, NULL}, lt = {0, NULL}; /* store query parameters */
  unsigned char *buf;
  coap_str_const_t attr_val;
  coap_str_const_t resource_val;

  loc = (unsigned char *)coap_malloc(LOCSIZE);
  if (!loc) {
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }
  memcpy(loc, RD_ROOT_STR, RD_ROOT_SIZE);

  loc_size = RD_ROOT_SIZE;
  loc[loc_size++] = '/';

  /* store query parameters for later use */
  if (query) {
    parse_param((const uint8_t *)"h", 1, query->s, query->length, &h);
    parse_param((const uint8_t *)"ins", 3, query->s, query->length, &ins);
    parse_param((const uint8_t *)"lt", 2, query->s, query->length, &lt);
    parse_param((const uint8_t *)"rt", 2, query->s, query->length, &rt);
  }

  if (h.length) {   /* client has specified a node name */
    memcpy(loc + loc_size, h.s, min(h.length, LOCSIZE - loc_size - 1));
    loc_size += min(h.length, LOCSIZE - loc_size - 1);

    if (ins.length && loc_size > 1) {
      loc[loc_size++] = '-';
      memcpy((char *)(loc + loc_size),
      ins.s, min(ins.length, LOCSIZE - loc_size - 1));
      loc_size += min(ins.length, LOCSIZE - loc_size - 1);
    }

  } else {      /* generate node identifier */
    loc_size +=
      snprintf((char *)(loc + loc_size), LOCSIZE - loc_size - 1,
               "%x", coap_pdu_get_mid(request));

    if (loc_size > 1) {
      if (ins.length) {
        loc[loc_size++] = '-';
        memcpy((char *)(loc + loc_size),
                ins.s,
                min(ins.length, LOCSIZE - loc_size - 1));
        loc_size += min(ins.length, LOCSIZE - loc_size - 1);
      } else {
        coap_tick_t now;
        coap_ticks(&now);

        loc_size += snprintf((char *)(loc + loc_size),
                             LOCSIZE - loc_size - 1,
                             "-%x",
                             (unsigned int)(now & (unsigned int)-1));
      }
    }
  }

  /* TODO:
   *   - use lt to check expiration
   */

  resource_val.s = loc;
  resource_val.length = loc_size;
  r = coap_resource_init(&resource_val, 0);
  coap_register_handler(r, COAP_REQUEST_GET, hnd_get_resource);
  coap_register_handler(r, COAP_REQUEST_PUT, hnd_put_resource);
  coap_register_handler(r, COAP_REQUEST_DELETE, hnd_delete_resource);

  if (ins.s) {
    buf = (unsigned char *)coap_malloc(ins.length + 2);
    if (buf) {
      /* add missing quotes */
      buf[0] = '"';
      memcpy(buf + 1, ins.s, ins.length);
      buf[ins.length + 1] = '"';
      attr_val.s = buf;
      attr_val.length = ins.length + 2;
      coap_add_attr(r,
                    coap_make_str_const("ins"),
                    &attr_val,
                    0);
      coap_free(buf);
    }
  }

  if (rt.s) {
    buf = (unsigned char *)coap_malloc(rt.length + 2);
    if (buf) {
      /* add missing quotes */
      buf[0] = '"';
      memcpy(buf + 1, rt.s, rt.length);
      buf[rt.length + 1] = '"';
      attr_val.s = buf;
      attr_val.length = rt.length + 2;
      coap_add_attr(r,
                    coap_make_str_const("rt"),
                    &attr_val,
                    0);
      coap_free(buf);
    }
  }

  add_source_address(r, coap_session_get_addr_remote(session));

  {
    rd_t *rd;
    rd = make_rd(request);
    if (rd) {
      coap_resource_set_userdata(r, rd);
    } else {
      /* FIXME: send error response and delete r */
    }
  }

  coap_add_resource(coap_session_get_context(session), r);


  /* create response */

  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CREATED);

  { /* split path into segments and add Location-Path options */
    unsigned char _b[LOCSIZE];
    unsigned char *b = _b;
    size_t buflen = sizeof(_b);
    int nseg;

    nseg = coap_split_path(loc, loc_size, b, &buflen);
    while (nseg--) {
      coap_add_option(response,
                      COAP_OPTION_LOCATION_PATH,
                      coap_opt_length(b),
                      coap_opt_value(b));
      b += coap_opt_size(b);
    }
  }
  coap_free(loc);
}

static void
init_resources(coap_context_t *ctx) {
  coap_resource_t *r;

  r = coap_resource_init(coap_make_str_const(RD_ROOT_STR), 0);
  coap_register_handler(r, COAP_REQUEST_GET, hnd_get_rd);
  coap_register_handler(r, COAP_REQUEST_POST, hnd_post_rd);

  coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("40"), 0);
  coap_add_attr(r, coap_make_str_const("rt"), coap_make_str_const("\"core.rd\""), 0);
  coap_add_attr(r, coap_make_str_const("ins"), coap_make_str_const("\"default\""), 0);

  coap_add_resource(ctx, r);

  coap_resource_release_userdata_handler(ctx, resource_rd_delete);
}

static void
usage( const char *program, const char *version) {
  const char *p;
  char buffer[72];
  const char *lib_version = coap_package_version();

  p = strrchr( program, '/' );
  if ( p )
    program = ++p;

  fprintf( stderr, "%s v%s -- CoRE Resource Directory implementation\n"
     "(c) 2011-2012,2019-2021 Olaf Bergmann <bergmann@tzi.org> and others\n\n"
     "%s\n"
     "%s\n"
    , program, version, lib_version,
    coap_string_tls_version(buffer, sizeof(buffer)));
  fprintf(stderr, "%s\n", coap_string_tls_support(buffer, sizeof(buffer)));
  fprintf(stderr, "\n"
     "Usage: %s [-g group] [-G group_if] [-p port] [-v num] [-A address]\n"
     "\t       [[-h hint] [-k key]]\n"
     "\t       [[-c certfile] [-C cafile] [-n] [-R trust_casfile]]\n"
     "General Options\n"
     "\t-g group\tJoin the given multicast group.\n"
     "\t       \t\tNote: DTLS over multicast is not currently supported\n"
     "\t-G group_if\tUse this interface for listening for the multicast\n"
     "\t       \t\tgroup. This can be different from the implied interface\n"
     "\t       \t\tif the -A option is used\n"
     "\t-p port\t\tListen on specified port\n"
     "\t-v num \t\tVerbosity level (default 3, maximum is 9). Above 7,\n"
     "\t       \t\tthere is increased verbosity in GnuTLS and OpenSSL logging\n"
     "\t-A address\tInterface address to bind to\n"
     "PSK Options (if supported by underlying (D)TLS library)\n"
     "\t-h hint\t\tIdentity Hint. Default is CoAP. Zero length is no hint\n"
     "\t-k key \t\tPre-Shared Key. This argument requires (D)TLS with PSK\n"
     "\t       \t\tto be available. This cannot be empty if defined.\n"
     "\t       \t\tNote that both -c and -k need to be defined\n"
     "\t       \t\tfor both PSK and PKI to be concurrently supported\n"
     "PKI Options (if supported by underlying (D)TLS library)\n"
     "\t-c certfile\tPEM file containing both CERTIFICATE and PRIVATE KEY\n"
     "\t       \t\tThis argument requires (D)TLS with PKI to be available\n"
     "\t-n     \t\tDisable remote peer certificate checking. This gives\n"
     "\t       \t\tclients the ability to use PKI, but without any defined\n"
     "\t       \t\tcertificates\n"
     "\t-C cafile\tPEM file that contains a list of one or\n"
     "\t       \t\tmore CAs that are to be passed to the client for the\n"
     "\t       \t\tclient to determine what client certificate to use.\n"
     "\t       \t\tNormally, this list of CAs would be the root CA and and\n"
     "\t       \t\tany intermediate CAs. Ideally the server certificate\n"
     "\t       \t\tshould be signed by the same CA so that mutual\n"
     "\t       \t\tauthentication can take place. The contents of cafile\n"
     "\t       \t\tare added to the trusted store of root CAs.\n"
     "\t       \t\tUsing the -C or -R options will will trigger the\n"
     "\t       \t\tvalidation of the client certificate unless overridden\n"
     "\t       \t\tby the -n option\n"
     "\t-R trust_casfile\tPEM file containing the set of trusted root CAs\n"
     "\t       \t\tthat are to be used to validate the client certificate.\n"
     "\t       \t\tAlternatively, this can point to a directory containing\n"
     "\t       \t\ta set of CA PEM files.\n"
     "\t       \t\tUsing '-R trust_casfile' disables common CA mutual\n"
     "\t       \t\tauthentication which can only be done by using\n"
     "\t       \t\t'-C cafile'.\n"
     "\t       \t\tUsing the -C or -R options will will trigger the\n"
     "\t       \t\tvalidation of the client certificate unless overridden\n"
     "\t       \t\tby the -n option\n"
     ,
     program);
}

static void
fill_keystore(coap_context_t *ctx) {
  if (cert_file == NULL && key_defined == 0) {
    if (coap_dtls_is_supported() || coap_tls_is_supported()) {
      coap_log(LOG_DEBUG,
               "(D)TLS not enabled as neither -k or -c options specified\n");
    }
  }
  if (cert_file) {
    coap_dtls_pki_t dtls_pki;
    memset (&dtls_pki, 0, sizeof(dtls_pki));
    dtls_pki.version = COAP_DTLS_PKI_SETUP_VERSION;
    if (ca_file || root_ca_file) {
      /*
       * Add in additional certificate checking.
       * This list of enabled can be tuned for the specific
       * requirements - see 'man coap_encryption'.
       */
      dtls_pki.verify_peer_cert        = verify_peer_cert;
      dtls_pki.check_common_ca         = !root_ca_file;
      dtls_pki.allow_self_signed       = 1;
      dtls_pki.allow_expired_certs     = 1;
      dtls_pki.cert_chain_validation   = 1;
      dtls_pki.cert_chain_verify_depth = 2;
      dtls_pki.check_cert_revocation   = 1;
      dtls_pki.allow_no_crl            = 1;
      dtls_pki.allow_expired_crl       = 1;
      dtls_pki.validate_cn_call_back   = NULL;
      dtls_pki.cn_call_back_arg        = NULL;
      dtls_pki.validate_sni_call_back  = NULL;
      dtls_pki.sni_call_back_arg       = NULL;
    }
    dtls_pki.pki_key.key_type = COAP_PKI_KEY_PEM;
    dtls_pki.pki_key.key.pem.public_cert = cert_file;
    dtls_pki.pki_key.key.pem.private_key = cert_file;
    dtls_pki.pki_key.key.pem.ca_file = ca_file;
    /* If general root CAs are defined */
    if (root_ca_file) {
      struct stat stbuf;
      if ((stat(root_ca_file, &stbuf) == 0) && S_ISDIR(stbuf.st_mode)) {
        coap_context_set_pki_root_cas(ctx, NULL, root_ca_file);
      } else {
        coap_context_set_pki_root_cas(ctx, root_ca_file, NULL);
      }
    }
    coap_context_set_pki(ctx, &dtls_pki);
  }
  if (key_defined) {
    coap_dtls_spsk_t dtls_psk;
    memset (&dtls_psk, 0, sizeof(dtls_psk));
    dtls_psk.version = COAP_DTLS_SPSK_SETUP_VERSION;
    dtls_psk.validate_id_call_back = NULL;
    dtls_psk.validate_sni_call_back = NULL;
    dtls_psk.psk_info.hint.s = (const uint8_t *)hint;
    dtls_psk.psk_info.hint.length = hint ? strlen(hint) : 0;
    dtls_psk.psk_info.key.s = key;
    dtls_psk.psk_info.key.length = key_length;
    coap_context_set_psk2(ctx, &dtls_psk);
  }
}

static coap_context_t *
get_context(const char *node, const char *port) {
  coap_context_t *ctx = NULL;
  int s;
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  ctx = coap_new_context(NULL);
  if (!ctx) {
    return NULL;
  }
  /* Need PSK set up before we set up (D)TLS endpoints */
  fill_keystore(ctx);

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_DGRAM; /* Coap uses UDP */
  hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;

  s = getaddrinfo(node, port, &hints, &result);
  if ( s != 0 ) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    coap_free_context(ctx);
    return NULL;
  }

  /* iterate through results until success */
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    coap_address_t addr, addrs;
    coap_endpoint_t *ep_udp = NULL, *ep_dtls = NULL, *ep_tcp = NULL, *ep_tls = NULL;

    if (rp->ai_addrlen <= (socklen_t)sizeof(addr.addr)) {
      coap_address_init(&addr);
      addr.size = (socklen_t)rp->ai_addrlen;
      memcpy(&addr.addr, rp->ai_addr, rp->ai_addrlen);
      addrs = addr;
      if (addr.addr.sa.sa_family == AF_INET) {
        uint16_t temp = ntohs(addr.addr.sin.sin_port) + 1;
        addrs.addr.sin.sin_port = htons(temp);
      } else if (addr.addr.sa.sa_family == AF_INET6) {
        uint16_t temp = ntohs(addr.addr.sin6.sin6_port) + 1;
        addrs.addr.sin6.sin6_port = htons(temp);
      } else {
        goto finish;
      }

      ep_udp = coap_new_endpoint(ctx, &addr, COAP_PROTO_UDP);
      if (ep_udp) {
        if (coap_dtls_is_supported() && (key_defined || cert_file)) {
          ep_dtls = coap_new_endpoint(ctx, &addrs, COAP_PROTO_DTLS);
          if (!ep_dtls)
            coap_log(LOG_CRIT, "cannot create DTLS endpoint\n");
        }
      } else {
        coap_log(LOG_CRIT, "cannot create UDP endpoint\n");
        continue;
      }
      ep_tcp = coap_new_endpoint(ctx, &addr, COAP_PROTO_TCP);
      if (ep_tcp) {
        if (coap_tls_is_supported() && (key_defined || cert_file)) {
          ep_tls = coap_new_endpoint(ctx, &addrs, COAP_PROTO_TLS);
          if (!ep_tls)
            coap_log(LOG_CRIT, "cannot create TLS endpoint\n");
        }
      } else {
        coap_log(LOG_CRIT, "cannot create TCP endpoint\n");
      }
      if (ep_udp)
        goto finish;
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
  char addr_str[NI_MAXHOST] = "::";
  char port_str[NI_MAXSERV] = "5683";
  char *group = NULL;
  char *group_if = NULL;
  int opt;
  coap_log_t log_level = LOG_WARNING;
#ifndef _WIN32
  struct sigaction sa;
#endif

  while ((opt = getopt(argc, argv, "A:c:C:g:G:h:k:n:R:p:v:")) != -1) {
    switch (opt) {
    case 'A' :
      strncpy(addr_str, optarg, NI_MAXHOST-1);
      addr_str[NI_MAXHOST - 1] = '\0';
      break;
    case 'c' :
      cert_file = optarg;
      break;
    case 'C' :
      ca_file = optarg;
      break;
    case 'g' :
      group = optarg;
      break;
    case 'G' :
      group_if = optarg;
      break;
    case 'h' :
      if (!optarg[0]) {
        hint = NULL;
        break;
      }
      hint = optarg;
      break;
    case 'k' :
      key_length = cmdline_read_key(optarg, key, MAX_KEY);
      if (key_length < 0) {
        coap_log( LOG_CRIT, "Invalid Pre-Shared Key specified\n" );
        break;
      }
      key_defined = 1;
      break;
    case 'n':
      verify_peer_cert = 0;
      break;
    case 'R' :
      root_ca_file = optarg;
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

  coap_startup();
  coap_dtls_set_log_level(log_level);
  coap_set_log_level(log_level);

  ctx = get_context(addr_str, port_str);
  if (!ctx)
    return -1;

  if (group)
    coap_join_mcast_group_intf(ctx, group, group_if);

  init_resources(ctx);

#ifdef _WIN32
  signal(SIGINT, handle_sigint);
#else
  memset (&sa, 0, sizeof(sa));
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = handle_sigint;
  sa.sa_flags = 0;
  sigaction (SIGINT, &sa, NULL);
  sigaction (SIGTERM, &sa, NULL);
  /* So we do not exit on a SIGPIPE */
  sa.sa_handler = SIG_IGN;
  sigaction (SIGPIPE, &sa, NULL);
#endif

  while ( !quit ) {
    result = coap_io_process( ctx, COAP_RESOURCE_CHECK_TIME * 1000 );
    if ( result >= 0 ) {
      /* coap_check_resource_list( ctx ); */
    }
  }

  coap_free_context( ctx );
  coap_cleanup();

  return 0;
}
