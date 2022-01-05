#include "coap_config.h"
#include <coap.h>

coap_context_t *main_coap_context;

static coap_time_t clock_offset;
/* changeable clock base (see handle_put_time()) */
static coap_time_t my_clock_base = 0;
static coap_resource_t *time_resource = NULL; /* just for testing */

#ifndef min
# define min(a,b) ((a) < (b) ? (a) : (b))
#endif

void
hnd_get_time(coap_context_t  *ctx, struct coap_resource_t *resource,
             coap_session_t *session,
             coap_pdu_t *request, coap_binary_t *token,
             coap_string_t *query,
             coap_pdu_t *response) {
  unsigned char buf[40];
  size_t len;
  coap_tick_t now;
  coap_tick_t t;

  /* FIXME: return time, e.g. in human-readable by default and ticks
   * when query ?ticks is given. */

  /* if my_clock_base was deleted, we pretend to have no such resource */
  response->code =
    my_clock_base ? COAP_RESPONSE_CODE(205) : COAP_RESPONSE_CODE(404);

  if (coap_find_observer(resource, session, token)) {
    coap_add_option(response, COAP_OPTION_OBSERVE,
                    coap_encode_var_safe(buf, sizeof(buf),
                                         resource->observe),
                    buf);
  }

  if (my_clock_base)
    coap_add_option(response, COAP_OPTION_CONTENT_FORMAT,
                    coap_encode_var_safe(buf, sizeof(buf),
                    COAP_MEDIATYPE_TEXT_PLAIN),
                    buf);

  coap_add_option(response, COAP_OPTION_MAXAGE,
          coap_encode_var_safe(buf, sizeof(buf), 0x01), buf);

  if (my_clock_base) {

    /* calculate current time */
    coap_ticks(&t);
    now = my_clock_base + (t / COAP_TICKS_PER_SECOND);


    if (query != NULL
        && coap_string_equal(query, coap_make_str_const("ticks"))) {
      /* output ticks */
      len = snprintf((char *)buf, sizeof(buf), "%u", (unsigned int)now);
      coap_add_data(response, len, buf);
    }
  }
}

void
init_coap_resources(coap_context_t *ctx) {
  coap_resource_t *r;
#if 0
  r = coap_resource_init(NULL, 0, 0);
  coap_register_handler(r, COAP_REQUEST_GET, hnd_get_index);

  coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("0"), 0);
  coap_add_attr(r, coap_make_str_const("title"), coap_make_str_const("\"General Info\""), 0);
  coap_add_resource(ctx, r);
#endif
  /* store clock base to use in /time */
  my_clock_base = clock_offset;

  r = coap_resource_init(coap_make_str_const("time"), 0);
  if (!r)
    goto error;

  coap_resource_set_get_observable(r, 1);
  time_resource = r;
  coap_register_handler(r, COAP_REQUEST_GET, hnd_get_time);
#if 0
  coap_register_handler(r, COAP_REQUEST_PUT, hnd_put_time);
  coap_register_handler(r, COAP_REQUEST_DELETE, hnd_delete_time);
#endif
  coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("0"), 0);
  /* coap_add_attr(r, coap_make_str_const("title"), coap_make_str_const("\"Internal Clock\""), 0); */
  coap_add_attr(r, coap_make_str_const("rt"), coap_make_str_const("\"ticks\""), 0);
  coap_add_attr(r, coap_make_str_const("if"), coap_make_str_const("\"clock\""), 0);

  coap_add_resource(ctx, r);
#if 0
#ifndef WITHOUT_ASYNC
  r = coap_resource_init(coap_make_str_const("async"), 0);
  coap_register_handler(r, COAP_REQUEST_GET, hnd_get_async);

  coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("0"), 0);
  coap_add_resource(ctx, r);
#endif /* WITHOUT_ASYNC */
#endif

  return;
 error:
  coap_log(LOG_CRIT, "cannot create resource\n");
}

void server_coap_init(void)
{
  coap_address_t listenaddress;

  coap_address_init(&listenaddress);

  /* looks like a server address, but is used as end point for clients too */
  listenaddress.addr = *(IP_ANY_TYPE);
  listenaddress.port = COAP_DEFAULT_PORT;

  coap_set_log_level(LOG_DEBUG);
  main_coap_context = coap_new_context(&listenaddress);

  LWIP_ASSERT("Failed to initialize context", main_coap_context != NULL);
  clock_offset = 1; /* Need a non-zero value */
  init_coap_resources(main_coap_context);
}

void server_coap_poll(void)
{
  static coap_time_t last_time = 0;
  coap_tick_t ticks_now;
  coap_time_t time_now;

  coap_ticks(&ticks_now);
  time_now = coap_ticks_to_rt(ticks_now);

  if (last_time != time_now) {
    /* This takes place once a second */
    last_time = time_now;
    coap_resource_notify_observers(time_resource, NULL);
  }
  coap_check_notify(main_coap_context);
}
