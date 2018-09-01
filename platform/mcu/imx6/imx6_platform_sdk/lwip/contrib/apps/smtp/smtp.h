#ifndef __SMTP_H__
#define __SMTP_H__

#include "lwip/err.h"

/** The default TCP port used for SMTP */
#define SMTP_DEFAULT_PORT  25

/** Email successfully sent */
#define SMTP_RESULT_OK            0
/** Unknown error */
#define SMTP_RESULT_ERR_UNKNOWN   1
/** Connection to server failed */
#define SMTP_RESULT_ERR_CONNECT   2
/** Failed to resolve server hostname */
#define SMTP_RESULT_ERR_HOSTNAME  3
/** Connection unexpectedly closed by remote server */
#define SMTP_RESULT_ERR_CLOSED    4
/** Connection timed out (server didn't respond in time) */
#define SMTP_RESULT_ERR_TIMEOUT   5
/** Server responded with an unknown response code */
#define SMTP_RESULT_ERR_SVR_RESP  6

/** Prototype of an smtp callback function
 *
 * @param arg argument specified when initiating the email
 * @param smtp_result result of the mail transfer (see defines SMTP_RESULT_*)
 * @param srv_err if aborted by the server, this contains the error code received
 * @param err an error returned by internal lwip functions, can help to specify
 *            the source of the error but must not necessarily be != ERR_OK
 */
typedef void (*smtp_result_fn)(void *arg, u8_t smtp_result, u16_t srv_err, err_t err);

/** This structure is used as argument for smtp_send_mail_int(),
 * which in turn can be used with tcpip_callback() to send mail
 * from interrupt context.
 * For member description, see parameter description of smtp_send_mail().
 * When using with tcpip_callback, this structure has to stay allocated
 * (e.g. using mem_malloc/mem_free) until its 'callback_fn' is called.
 */
struct smtp_send_request {
  const char *from;
  const char* to;
  const char* subject;
  const char* body;
  smtp_result_fn callback_fn;
  void* callback_arg;
  /** If this is != 0, data is *not* copied into an extra buffer
   * but used from the pointers supplied in this struct.
   * This means less memory usage, but data must stay untouched until
   * the callback function is called. */
  u8_t static_data;
};

void smtp_set_server_addr(const char* server);
void smtp_set_server_port(u16_t port);
err_t smtp_set_auth(const char* username, const char* pass);
err_t smtp_send_mail(const char *from, const char* to, const char* subject, const char* body,
                     smtp_result_fn callback_fn, void* callback_arg);
err_t smtp_send_mail_static(const char *from, const char* to, const char* subject, const char* body,
                     smtp_result_fn callback_fn, void* callback_arg);
void smtp_send_mail_int(void *arg);
#if LWIP_DEBUG
const char* smtp_result_str(u8_t smtp_result);
#endif

#endif /* __SMTP_H__ */
