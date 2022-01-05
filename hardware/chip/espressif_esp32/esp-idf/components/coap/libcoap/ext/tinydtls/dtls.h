/*******************************************************************************
 *
 * Copyright (c) 2011, 2012, 2013, 2014, 2015 Olaf Bergmann (TZI) and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v. 1.0 which accompanies this distribution.
 *
 * The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Olaf Bergmann  - initial API and implementation
 *    Hauke Mehrtens - memory optimization, ECC integration
 *    Achim Kraus    - session recovery
 *
 *******************************************************************************/

/**
 * @file dtls.h
 * @brief High level DTLS API and visible structures. 
 */

#ifndef _DTLS_DTLS_H_
#define _DTLS_DTLS_H_

#include <stdint.h>

#include "tinydtls.h"

#include "state.h"
#include "peer.h"

#include "uthash.h"

#include "alert.h"
#include "crypto.h"
#include "hmac.h"

#include "global.h"
#include "dtls_time.h"

#ifndef DTLSv12
#define DTLS_VERSION 0xfeff	/* DTLS v1.1 */
#else
#define DTLS_VERSION 0xfefd	/* DTLS v1.2 */
#endif

typedef enum dtls_credentials_type_t {
  DTLS_PSK_HINT, DTLS_PSK_IDENTITY, DTLS_PSK_KEY
} dtls_credentials_type_t;

typedef struct dtls_ecdsa_key_t {
  dtls_ecdh_curve curve;
  const unsigned char *priv_key;	/** < private key as bytes > */
  const unsigned char *pub_key_x;	/** < x part of the public key for the given private key > */
  const unsigned char *pub_key_y;	/** < y part of the public key for the given private key > */
} dtls_ecdsa_key_t;

/** Length of the secret that is used for generating Hello Verify cookies. */
#define DTLS_COOKIE_SECRET_LENGTH 12

struct dtls_context_t;

/**
 * This structure contains callback functions used by tinydtls to
 * communicate with the application. At least the write function must
 * be provided. It is called by the DTLS state machine to send packets
 * over the network. The read function is invoked to deliver decrypted
 * and verfified application data. The third callback is an event
 * handler function that is called when alert messages are encountered
 * or events generated by the library have occured.
 */ 
typedef struct {
  /** 
   * Called from dtls_handle_message() to send DTLS packets over the
   * network. The callback function must use the network interface
   * denoted by session->ifindex to send the data.
   *
   * @param ctx  The current DTLS context.
   * @param session The session object, including the address of the
   *              remote peer where the data shall be sent.
   * @param buf  The data to send.
   * @param len  The actual length of @p buf.
   * @return The callback function must return the number of bytes 
   *         that were sent, or a value less than zero to indicate an 
   *         error.
   */
  int (*write)(struct dtls_context_t *ctx, 
	       session_t *session, uint8 *buf, size_t len);

  /** 
   * Called from dtls_handle_message() deliver application data that was 
   * received on the given session. The data is delivered only after
   * decryption and verification have succeeded. 
   *
   * @param ctx  The current DTLS context.
   * @param session The session object, including the address of the
   *              data's origin. 
   * @param buf  The received data packet.
   * @param len  The actual length of @p buf.
   * @return ignored
   */
  int (*read)(struct dtls_context_t *ctx, 
	       session_t *session, uint8 *buf, size_t len);

  /**
   * The event handler is called when a message from the alert
   * protocol is received or the state of the DTLS session changes.
   *
   * @param ctx     The current dtls context.
   * @param session The session object that was affected.
   * @param level   The alert level or @c 0 when an event ocurred that 
   *                is not an alert. 
   * @param code    Values less than @c 256 indicate alerts, while
   *                @c 256 or greater indicate internal DTLS session changes.
   * @return ignored
   */
  int (*event)(struct dtls_context_t *ctx, session_t *session, 
		dtls_alert_level_t level, unsigned short code);

#ifdef DTLS_PSK
  /**
   * Called during handshake to get information related to the
   * psk key exchange. The type of information requested is
   * indicated by @p type which will be one of DTLS_PSK_HINT,
   * DTLS_PSK_IDENTITY, or DTLS_PSK_KEY. The called function
   * must store the requested item in the buffer @p result of
   * size @p result_length. On success, the function must return
   * the actual number of bytes written to @p result, of a
   * value less than zero on error. The parameter @p desc may
   * contain additional request information (e.g. the psk_identity
   * for which a key is requested when @p type == @c DTLS_PSK_KEY.
   *
   * @param ctx     The current dtls context.
   * @param session The session where the key will be used.
   * @param type    The type of the requested information.
   * @param desc    Additional request information
   * @param desc_len The actual length of desc.
   * @param result  Must be filled with the requested information.
   * @param result_length  Maximum size of @p result.
   * @return The number of bytes written to @p result or a value
   *         less than zero on error.
   */
  int (*get_psk_info)(struct dtls_context_t *ctx,
		      const session_t *session,
		      dtls_credentials_type_t type,
		      const unsigned char *desc, size_t desc_len,
		      unsigned char *result, size_t result_length);

#endif /* DTLS_PSK */

#ifdef DTLS_ECC
  /**
   * Called during handshake to get the server's or client's ecdsa
   * key used to authenticate this server or client in this 
   * session. If found, the key must be stored in @p result and 
   * the return value must be @c 0. If not found, @p result is 
   * undefined and the return value must be less than zero.
   *
   * If ECDSA should not be supported, set this pointer to NULL.
   *
   * Implement this if you want to provide your own certificate to 
   * the other peer. This is mandatory for a server providing ECDSA
   * support and optional for a client. A client doing DTLS client
   * authentication has to implementing this callback.
   *
   * @param ctx     The current dtls context.
   * @param session The session where the key will be used.
   * @param result  Must be set to the key object to used for the given
   *                session.
   * @return @c 0 if result is set, or less than zero on error.
   */
  int (*get_ecdsa_key)(struct dtls_context_t *ctx, 
		       const session_t *session,
		       const dtls_ecdsa_key_t **result);

  /**
   * Called during handshake to check the peer's pubic key in this
   * session. If the public key matches the session and should be
   * considerated valid the return value must be @c 0. If not valid,
   * the return value must be less than zero.
   *
   * If ECDSA should not be supported, set this pointer to NULL.
   *
   * Implement this if you want to verify the other peers public key.
   * This is mandatory for a DTLS client doing based ECDSA
   * authentication. A server implementing this will request the
   * client to do DTLS client authentication.
   *
   * @param ctx          The current dtls context.
   * @param session      The session where the key will be used.
   * @param other_pub_x  x component of the public key.
   * @param other_pub_y  y component of the public key.
   * @return @c 0 if public key matches, or less than zero on error.
   * error codes:
   *   return dtls_alert_fatal_create(DTLS_ALERT_BAD_CERTIFICATE);
   *   return dtls_alert_fatal_create(DTLS_ALERT_UNSUPPORTED_CERTIFICATE);
   *   return dtls_alert_fatal_create(DTLS_ALERT_CERTIFICATE_REVOKED);
   *   return dtls_alert_fatal_create(DTLS_ALERT_CERTIFICATE_EXPIRED);
   *   return dtls_alert_fatal_create(DTLS_ALERT_CERTIFICATE_UNKNOWN);
   *   return dtls_alert_fatal_create(DTLS_ALERT_UNKNOWN_CA);
   */
  int (*verify_ecdsa_key)(struct dtls_context_t *ctx, 
			  const session_t *session,
			  const unsigned char *other_pub_x,
			  const unsigned char *other_pub_y,
			  size_t key_size);
#endif /* DTLS_ECC */
} dtls_handler_t;

struct netq_t;

/** Holds global information of the DTLS engine. */
typedef struct dtls_context_t {
  unsigned char cookie_secret[DTLS_COOKIE_SECRET_LENGTH];
  clock_time_t cookie_secret_age; /**< the time the secret has been generated */

  dtls_peer_t *peers;		/**< peer hash map */
#ifdef WITH_CONTIKI
  struct etimer retransmit_timer; /**< fires when the next packet must be sent */
#endif /* WITH_CONTIKI */

  struct netq_t *sendqueue;     /**< the packets to send */

  void *app;			/**< application-specific data */

  dtls_handler_t *h;		/**< callback handlers */

  unsigned char readbuf[DTLS_MAX_BUF];
} dtls_context_t;

/** 
 * This function initializes the tinyDTLS memory management and must
 * be called first.
 */
void dtls_init(void);

/** 
 * Creates a new context object. The storage allocated for the new
 * object must be released with dtls_free_context(). */
dtls_context_t *dtls_new_context(void *app_data);

/** Releases any storage that has been allocated for \p ctx. */
void dtls_free_context(dtls_context_t *ctx);

#define dtls_set_app_data(CTX,DATA) ((CTX)->app = (DATA))
#define dtls_get_app_data(CTX) ((CTX)->app)

/** Sets the callback handler object for @p ctx to @p h. */
static inline void dtls_set_handler(dtls_context_t *ctx, dtls_handler_t *h) {
  ctx->h = h;
}

/**
 * Establishes a DTLS channel with the specified remote peer @p dst.
 * This function returns @c 0 if that channel already exists, a value
 * greater than zero when a new ClientHello message was sent, and
 * a value less than zero on error.
 *
 * @param ctx    The DTLS context to use.
 * @param dst    The remote party to connect to.
 * @return A value less than zero on error, greater or equal otherwise.
 */
int dtls_connect(dtls_context_t *ctx, const session_t *dst);

/**
 * Establishes a DTLS channel with the specified remote peer.
 * This function returns @c 0 if that channel already exists, a value
 * greater than zero when a new ClientHello message was sent, and
 * a value less than zero on error.
 *
 * @param ctx    The DTLS context to use.
 * @param peer   The peer object that describes the session.
 * @return A value less than zero on error, greater or equal otherwise.
 */
int dtls_connect_peer(dtls_context_t *ctx, dtls_peer_t *peer);

/**
 * Closes the DTLS connection associated with @p remote. This function
 * returns zero on success, and a value less than zero on error.
 */
int dtls_close(dtls_context_t *ctx, const session_t *remote);

int dtls_renegotiate(dtls_context_t *ctx, const session_t *dst);

/** 
 * Writes the application data given in @p buf to the peer specified
 * by @p session. 
 * 
 * @param ctx      The DTLS context to use.
 * @param session  The remote transport address and local interface.
 * @param buf      The data to write.
 * @param len      The actual length of @p data.
 * 
 * @return The number of bytes written, @c -1 on error or @c 0
 *         if the peer already exists but is not connected yet.
 */
int dtls_write(struct dtls_context_t *ctx, session_t *session, 
	       uint8 *buf, size_t len);

/**
 * Checks sendqueue of given DTLS context object for any outstanding
 * packets to be transmitted. 
 *
 * @param context The DTLS context object to use.
 * @param next    If not NULL, @p next is filled with the timestamp
 *  of the next scheduled retransmission, or @c 0 when no packets are
 *  waiting.
 */
void dtls_check_retransmit(dtls_context_t *context, clock_time_t *next);

#define DTLS_COOKIE_LENGTH 16

#define DTLS_CT_CHANGE_CIPHER_SPEC 20
#define DTLS_CT_ALERT              21
#define DTLS_CT_HANDSHAKE          22
#define DTLS_CT_APPLICATION_DATA   23

/** Generic header structure of the DTLS record layer. */
typedef struct __attribute__((__packed__)) {
  uint8 content_type;		/**< content type of the included message */
  uint16 version;		/**< Protocol version */
  uint16 epoch;		        /**< counter for cipher state changes */
  uint48 sequence_number;       /**< sequence number */
  uint16 length;		/**< length of the following fragment */
  /* fragment */
} dtls_record_header_t;

/* Handshake types */

#define DTLS_HT_HELLO_REQUEST        0
#define DTLS_HT_CLIENT_HELLO         1
#define DTLS_HT_SERVER_HELLO         2
#define DTLS_HT_HELLO_VERIFY_REQUEST 3
#define DTLS_HT_CERTIFICATE         11
#define DTLS_HT_SERVER_KEY_EXCHANGE 12
#define DTLS_HT_CERTIFICATE_REQUEST 13
#define DTLS_HT_SERVER_HELLO_DONE   14
#define DTLS_HT_CERTIFICATE_VERIFY  15
#define DTLS_HT_CLIENT_KEY_EXCHANGE 16
#define DTLS_HT_FINISHED            20

/** Header structure for the DTLS handshake protocol. */
typedef struct __attribute__((__packed__)) {
  uint8 msg_type; /**< Type of handshake message  (one of DTLS_HT_) */
  uint24 length;  /**< length of this message */
  uint16 message_seq; 	/**< Message sequence number */
  uint24 fragment_offset;	/**< Fragment offset. */
  uint24 fragment_length;	/**< Fragment length. */
  /* body */
} dtls_handshake_header_t;

/** Structure of the Client Hello message. */
typedef struct __attribute__((__packed__)) {
  uint16 version;	  /**< Client version */
  uint32 gmt_random;	  /**< GMT time of the random byte creation */
  unsigned char random[28];	/**< Client random bytes */
  /* session id (up to 32 bytes) */
  /* cookie (up to 32 bytes) */
  /* cipher suite (2 to 2^16 -1 bytes) */
  /* compression method */
} dtls_client_hello_t;

/** Structure of the Hello Verify Request. */
typedef struct __attribute__((__packed__)) {
  uint16 version;		/**< Server version */
  uint8 cookie_length;	/**< Length of the included cookie */
  uint8 cookie[];		/**< up to 32 bytes making up the cookie */
} dtls_hello_verify_t;  

#if 0
/** 
 * Checks a received DTLS record for consistency and eventually decrypt,
 * verify, decompress and reassemble the contained fragment for 
 * delivery to high-lever clients. 
 * 
 * \param state The DTLS record state for the current session. 
 * \param 
 */
int dtls_record_read(dtls_state_t *state, uint8 *msg, int msglen);
#endif

/** 
 * Handles incoming data as DTLS message from given peer.
 *
 * @param ctx     The dtls context to use.
 * @param session The current session
 * @param msg     The received data
 * @param msglen  The actual length of @p msg.
 * @return A value less than zero on error, zero on success.
 */
int dtls_handle_message(dtls_context_t *ctx, session_t *session,
			uint8 *msg, int msglen);

/**
 * Check if @p session is associated with a peer object in @p context.
 * This function returns a pointer to the peer if found, NULL otherwise.
 *
 * @param context  The DTLS context to search.
 * @param session  The remote address and local interface
 * @return A pointer to the peer associated with @p session or NULL if
 *  none exists.
 */
dtls_peer_t *dtls_get_peer(const dtls_context_t *context,
			   const session_t *session);

/**
 * Resets all connections with @p peer.
 *
 * @param context  The active DTLS context.
 * @param peer     The peer to reset.
 */
void dtls_reset_peer(dtls_context_t *context, dtls_peer_t *peer);

#endif /* _DTLS_DTLS_H_ */

/**
 * @mainpage 
 *
 * @author Olaf Bergmann, TZI Uni Bremen
 *
 * This library provides a very simple datagram server with DTLS
 * support. It is designed to support session multiplexing in
 * single-threaded applications and thus targets specifically on
 * embedded systems.
 *
 * @section license License
 *
 * This software is under the <a 
 * href="http://www.opensource.org/licenses/mit-license.php">MIT License</a>.
 * 
 * @subsection uthash UTHash
 *
 * This library uses <a href="http://uthash.sourceforge.net/">uthash</a> to manage
 * its peers (not used for Contiki). @b uthash uses the <b>BSD revised license</b>, see
 * <a href="http://uthash.sourceforge.net/license.html">http://uthash.sourceforge.net/license.html</a>.
 *
 * @subsection sha256 Aaron D. Gifford's SHA256 Implementation
 *
 * tinyDTLS provides HMAC-SHA256 with BSD-licensed code from Aaron D. Gifford, 
 * see <a href="http://www.aarongifford.com/">www.aarongifford.com</a>.
 *
 * @subsection aes Rijndael Implementation From OpenBSD
 *
 * The AES implementation is taken from rijndael.{c,h} contained in the crypto 
 * sub-system of the OpenBSD operating system. It is copyright by Vincent Rijmen, *
 * Antoon Bosselaers and Paulo Barreto. See <a 
 * href="http://www.openbsd.org/cgi-bin/cvsweb/src/sys/crypto/rijndael.c">rijndael.c</a> 
 * for License info.
 *
 * @section download Getting the Files
 *
 * You can get the sources either from the <a 
 * href="http://sourceforge.net/projects/tinydtls/files">downloads</a> section or 
 * through git from the <a 
 * href="http://sourceforge.net/projects/tinydtls/develop">project develop page</a>.
 *
 * @section config Configuration
 *
 * Use @c configure to set up everything for a successful build. For Contiki, use the
 * option @c --with-contiki.
 *
 * @section build Building
 *
 * After configuration, just type 
 * @code
make
 * @endcode
 * optionally followed by
 * @code
make install
 * @endcode
 * The Contiki version is integrated with the Contiki build system, hence you do not
 * need to invoke @c make explicitely. Just add @c tinydtls to the variable @c APPS
 * in your @c Makefile.
 *
 * @addtogroup dtls_usage DTLS Usage
 *
 * @section dtls_server_example DTLS Server Example
 *
 * This section shows how to use the DTLS library functions to setup a 
 * simple secure UDP echo server. The application is responsible for the
 * entire network communication and thus will look like a usual UDP
 * server with socket creation and binding and a typical select-loop as
 * shown below. The minimum configuration required for DTLS is the 
 * creation of the dtls_context_t using dtls_new_context(), and a callback
 * for sending data. Received packets are read by the application and
 * passed to dtls_handle_message() as shown in @ref dtls_read_cb. 
 * For any useful communication to happen, read and write call backs 
 * and a key management function should be registered as well. 
 * 
 * @code 
 dtls_context_t *the_context = NULL;
 int fd, result;

 static dtls_handler_t cb = {
   .write = send_to_peer,
   .read  = read_from_peer,
   .event = NULL,
   .get_psk_key = get_psk_key
 };

 fd = socket(...);
 if (fd < 0 || bind(fd, ...) < 0)
   exit(-1);

 the_context = dtls_new_context(&fd);
 dtls_set_handler(the_context, &cb);

 while (1) {
   ...initialize fd_set rfds and timeout ...
   result = select(fd+1, &rfds, NULL, 0, NULL);
    
   if (FD_ISSET(fd, &rfds))
     dtls_handle_read(the_context);
 }

 dtls_free_context(the_context);
 * @endcode
 * 
 * @subsection dtls_read_cb The Read Callback
 *
 * The DTLS library expects received raw data to be passed to
 * dtls_handle_message(). The application is responsible for
 * filling a session_t structure with the address data of the
 * remote peer as illustrated by the following example:
 * 
 * @code
int dtls_handle_read(struct dtls_context_t *ctx) {
  int *fd;
  session_t session;
  static uint8 buf[DTLS_MAX_BUF];
  int len;

  fd = dtls_get_app_data(ctx);

  assert(fd);

  session.size = sizeof(session.addr);
  len = recvfrom(*fd, buf, sizeof(buf), 0, &session.addr.sa, &session.size);
  
  return len < 0 ? len : dtls_handle_message(ctx, &session, buf, len);
}    
 * @endcode 
 * 
 * Once a new DTLS session was established and DTLS ApplicationData has been
 * received, the DTLS server invokes the read callback with the MAC-verified 
 * cleartext data as its argument. A read callback for a simple echo server
 * could look like this:
 * @code
int read_from_peer(struct dtls_context_t *ctx, session_t *session, uint8 *data, size_t len) {
  return dtls_write(ctx, session, data, len);
}
 * @endcode 
 * 
 * @subsection dtls_send_cb The Send Callback
 * 
 * The callback function send_to_peer() is called whenever data must be
 * sent over the network. Here, the sendto() system call is used to
 * transmit data within the given session. The socket descriptor required
 * by sendto() has been registered as application data when the DTLS context
 * was created with dtls_new_context().
 * Note that it is on the application to buffer the data when it cannot be
 * sent at the time this callback is invoked. The following example thus
 * is incomplete as it would have to deal with EAGAIN somehow.
 * @code
int send_to_peer(struct dtls_context_t *ctx, session_t *session, uint8 *data, size_t len) {
  int fd = *(int *)dtls_get_app_data(ctx);
  return sendto(fd, data, len, MSG_DONTWAIT, &session->addr.sa, session->size);
}
 * @endcode
 * 
 * @subsection dtls_get_psk_info The Key Storage
 *
 * When a new DTLS session is created, the library must ask the application
 * for keying material. To do so, it invokes the registered call-back function
 * get_psk_info() with the current context and session information as parameter.
 * When the call-back function is invoked with the parameter @p type set to 
 * @c DTLS_PSK_IDENTITY, the result parameter @p result must be filled with
 * the psk_identity_hint in case of a server, or the actual psk_identity in 
 * case of a client. When @p type is @c DTLS_PSK_KEY, the result parameter
 * must be filled with a key for the given identity @p id. The function must
 * return the number of bytes written to @p result which must not exceed
 * @p result_length.
 * In case of an error, the function must return a negative value that 
 * corresponds to a valid error code defined in alert.h.
 * 
 * @code
int get_psk_info(struct dtls_context_t *ctx UNUSED_PARAM,
	    const session_t *session UNUSED_PARAM,
	    dtls_credentials_type_t type,
	    const unsigned char *id, size_t id_len,
	    unsigned char *result, size_t result_length) {

  switch (type) {
  case DTLS_PSK_IDENTITY:
    if (result_length < psk_id_length) {
      dtls_warn("cannot set psk_identity -- buffer too small\n");
      return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }

    memcpy(result, psk_id, psk_id_length);
    return psk_id_length;
  case DTLS_PSK_KEY:
    if (id_len != psk_id_length || memcmp(psk_id, id, id_len) != 0) {
      dtls_warn("PSK for unknown id requested, exiting\n");
      return dtls_alert_fatal_create(DTLS_ALERT_ILLEGAL_PARAMETER);
    } else if (result_length < psk_key_length) {
      dtls_warn("cannot set psk -- buffer too small\n");
      return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }

    memcpy(result, psk_key, psk_key_length);
    return psk_key_length;
  default:
    dtls_warn("unsupported request type: %d\n", type);
  }

  return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
}
 * @endcode
 * 
 * @subsection dtls_events The Event Notifier
 *
 * Applications that want to be notified whenever the status of a DTLS session
 * has changed can register an event handling function with the field @c event
 * in the dtls_handler_t structure (see \ref dtls_server_example). The call-back
 * function is called for alert messages and internal state changes. For alert
 * messages, the argument @p level will be set to a value greater than zero, and
 * @p code will indicate the notification code. For internal events, @p level
 * is @c 0, and @p code a value greater than @c 255. 
 *
 * Internal events are DTLS_EVENT_CONNECTED, @c DTLS_EVENT_CONNECT, and
 * @c DTLS_EVENT_RENEGOTIATE.
 *
 * @code
int handle_event(struct dtls_context_t *ctx, session_t *session, 
                 dtls_alert_level_t level, unsigned short code) {
  ... do something with event ...
  return 0;
}
 * @endcode
 *
 * @section dtls_client_example DTLS Client Example
 *
 * A DTLS client is constructed like a server but needs to actively setup
 * a new session by calling dtls_connect() at some point. As this function
 * usually returns before the new DTLS channel is established, the application
 * must register an event handler and wait for @c DTLS_EVENT_CONNECT before
 * it can send data over the DTLS channel.
 *
 */

/**
 * @addtogroup contiki Contiki
 *
 * To use tinyDTLS as Contiki application, place the source code in the directory 
 * @c apps/tinydtls in the Contiki source tree and invoke configure with the option
 * @c --with-contiki. This will define WITH_CONTIKI in tinydtls.h and include 
 * @c Makefile.contiki in the main Makefile. To cross-compile for another platform
 * you will need to set your host and build system accordingly. For example,
 * when configuring for ARM, you would invoke
 * @code
./configure --with-contiki --build=x86_64-linux-gnu --host=arm-none-eabi 
 * @endcode
 * on an x86_64 linux host.
 *
 * Then, create a Contiki project with @c APPS += tinydtls in its Makefile. A sample
 * server could look like this (with read_from_peer() and get_psk_key() as shown above).
 *
 * @code
#include "contiki.h"

#include "tinydtls.h"
#include "dtls.h"

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[UIP_LLIPH_LEN])

int send_to_peer(struct dtls_context_t *, session_t *, uint8 *, size_t);

static struct uip_udp_conn *server_conn;
static dtls_context_t *dtls_context;

static dtls_handler_t cb = {
  .write = send_to_peer,
  .read  = read_from_peer,
  .event = NULL,
  .get_psk_key = get_psk_key
};

PROCESS(server_process, "DTLS server process");
AUTOSTART_PROCESSES(&server_process);

PROCESS_THREAD(server_process, ev, data)
{
  PROCESS_BEGIN();

  dtls_init();

  server_conn = udp_new(NULL, 0, NULL);
  udp_bind(server_conn, UIP_HTONS(5684));

  dtls_context = dtls_new_context(server_conn);
  if (!dtls_context) {
    dtls_emerg("cannot create context\n");
    PROCESS_EXIT();
  }

  dtls_set_handler(dtls_context, &cb);

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == tcpip_event && uip_newdata()) {
      session_t session;

      uip_ipaddr_copy(&session.addr, &UIP_IP_BUF->srcipaddr);
      session.port = UIP_UDP_BUF->srcport;
      session.size = sizeof(session.addr) + sizeof(session.port);
    
      dtls_handle_message(ctx, &session, uip_appdata, uip_datalen());
    }
  }

  PROCESS_END();
}

int send_to_peer(struct dtls_context_t *ctx, session_t *session, uint8 *data, size_t len) {
  struct uip_udp_conn *conn = (struct uip_udp_conn *)dtls_get_app_data(ctx);

  uip_ipaddr_copy(&conn->ripaddr, &session->addr);
  conn->rport = session->port;

  uip_udp_packet_send(conn, data, len);

  memset(&conn->ripaddr, 0, sizeof(server_conn->ripaddr));
  memset(&conn->rport, 0, sizeof(conn->rport));

  return len;
}
 * @endcode
 */
