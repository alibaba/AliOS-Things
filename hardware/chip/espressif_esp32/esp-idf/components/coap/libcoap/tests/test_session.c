/* libcoap unit tests
 *
 * Copyright (C) 2013,2015-2018 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap_config.h"
#include "test_session.h"

#include <coap.h>

#include <stdio.h>

/* The error threshold for timeout calculations. The precision of
 * coap_calc_timeout() is assumed to be sufficient if the resulting
 * value deviates at most FP_ERR_THRESHOLD percent from the value that
 * was calculated with double precision floating point arithmetic
 * instead of fixed point integers.
 */
#define FP_ERR_THRESHOLD (1.0)

/* The maximum number of bits for fixed point integer representation.
 * This number must be identical to the definition of MAX_BITS in
 * net.c
 */
#define MAX_BITS 8

static coap_context_t *ctx; /* Holds the coap context for most tests */
static coap_session_t *session; /* Holds a reference-counted session object */

COAP_STATIC_INLINE int
fpeq(const coap_fixed_point_t a, const coap_fixed_point_t b) {
  return (a.integer_part == b.integer_part) &&
    (a.fractional_part == b.fractional_part);
}

static void
t_session1(void) {
  CU_ASSERT(session->ref == 1);

  coap_session_reference(session);
  CU_ASSERT(session->ref == 2);

  coap_session_release(session);
  CU_ASSERT(session->ref == 1);
}

static void
t_session2(void) {
  CU_ASSERT(session->max_retransmit == COAP_DEFAULT_MAX_RETRANSMIT);
  CU_ASSERT(fpeq(session->ack_timeout, COAP_DEFAULT_ACK_TIMEOUT));
  CU_ASSERT(fpeq(session->ack_random_factor, COAP_DEFAULT_ACK_RANDOM_FACTOR));

  CU_ASSERT(coap_session_get_max_transmit(session) == COAP_DEFAULT_MAX_RETRANSMIT);
  CU_ASSERT(fpeq(coap_session_get_ack_timeout(session), COAP_DEFAULT_ACK_TIMEOUT));
  CU_ASSERT(fpeq(coap_session_get_ack_random_factor(session), COAP_DEFAULT_ACK_RANDOM_FACTOR));
}

COAP_STATIC_INLINE double
fp_to_double(const coap_fixed_point_t fp) {
  return fp.integer_part + fp.fractional_part/1000.0;
}

COAP_STATIC_INLINE double
q_to_double(uint8_t q) {
  return (double)(q) / (1 << MAX_BITS);
}

/* Calculates the timeout using the following formula:
 *
 * COAP_TICKS_PER_SECOND * ack_timeout * (1 + (ack_random_factor - 1) * r)
 *
 * where @p r is the randomization value represented as Q0.MAX_BITS.
 * This function returns the result as unsigned int to be
 * checked against the result of coap_calc_timeout().
 */
COAP_STATIC_INLINE unsigned int
timeout(const coap_fixed_point_t ack_timeout,
        const coap_fixed_point_t ack_random_factor,
        uint8_t r) {
  const unsigned int ctps = COAP_TICKS_PER_SECOND;
  double ato = fp_to_double(ack_timeout);
  double rnd = 1 + (fp_to_double(ack_random_factor) - 1.0) * q_to_double(r);

  return (unsigned int)(ctps * ato * rnd);
}

/* Checks if @p v is within FP_ERR_THRESHOLD percent of @p ref.  This
 * function returns 1 if it is within range, 0 otherwise.
 */
COAP_STATIC_INLINE int
good_enough(unsigned int v, unsigned int ref) {
#define delta(a,b) (((a) < (b)) ? ((b) - (a)) : ((a) - (b)))
  return (delta(v,ref) / 100.0) <= FP_ERR_THRESHOLD;
}

static void
t_session3(void) {
  const coap_fixed_point_t ato = COAP_DEFAULT_ACK_TIMEOUT;
  const coap_fixed_point_t arf = COAP_DEFAULT_ACK_RANDOM_FACTOR;

  CU_ASSERT(good_enough(coap_calc_timeout(session, 0), timeout(ato, arf, 0)));
  CU_ASSERT(good_enough(coap_calc_timeout(session, 12), timeout(ato, arf, 12)));
  CU_ASSERT(good_enough(coap_calc_timeout(session, 55), timeout(ato, arf, 55)));
  CU_ASSERT(good_enough(coap_calc_timeout(session, 83), timeout(ato, arf, 83)));
  CU_ASSERT(good_enough(coap_calc_timeout(session, 117), timeout(ato, arf, 117)));
  CU_ASSERT(good_enough(coap_calc_timeout(session, 210), timeout(ato, arf, 210)));
  CU_ASSERT(good_enough(coap_calc_timeout(session, 255), timeout(ato, arf, 255)));
}

static void
t_session4(void) {
  const coap_fixed_point_t ato = {8,300};
  const coap_fixed_point_t arf = {2,011};

  coap_session_set_ack_timeout(session, ato);
  coap_session_set_ack_random_factor(session, arf);

  CU_ASSERT(fpeq(coap_session_get_ack_timeout(session), ato));
  CU_ASSERT(fpeq(coap_session_get_ack_random_factor(session), arf));

  CU_ASSERT(good_enough(coap_calc_timeout(session, 0), timeout(ato, arf, 0)));
  CU_ASSERT(good_enough(coap_calc_timeout(session, 12), timeout(ato, arf, 12)));
  CU_ASSERT(good_enough(coap_calc_timeout(session, 55), timeout(ato, arf, 55)));
  CU_ASSERT(good_enough(coap_calc_timeout(session, 83), timeout(ato, arf, 83)));
  CU_ASSERT(good_enough(coap_calc_timeout(session, 117), timeout(ato, arf, 117)));
  CU_ASSERT(good_enough(coap_calc_timeout(session, 210), timeout(ato, arf, 210)));
  CU_ASSERT(good_enough(coap_calc_timeout(session, 255), timeout(ato, arf, 255)));
}

static void
t_session5(void) {
  coap_session_release(session);

  CU_ASSERT_PTR_NULL(ctx->sessions);
  session = NULL;
}

/* Test 6 creates a new session after the original session has been
 * released by test 5 */
static void
t_session6(void) {
  coap_address_t laddr, saddr;
  coap_address_init(&laddr);
  coap_address_init(&saddr);

  CU_ASSERT_PTR_NULL(session);

  laddr.size = sizeof(struct sockaddr_in6);
  laddr.addr.sin6.sin6_family = AF_INET6;
  laddr.addr.sin6.sin6_addr = in6addr_any;
  laddr.addr.sin6.sin6_port = htons(COAP_DEFAULT_PORT);

  coap_address_copy(&saddr, &laddr);
  saddr.addr.sin6.sin6_addr = in6addr_loopback;
  saddr.addr.sin6.sin6_port = htons(20000);

  session = coap_new_client_session(ctx, &saddr, &laddr, COAP_PROTO_UDP);
  CU_ASSERT_PTR_NOT_NULL(session);
  CU_ASSERT_PTR_NOT_NULL(ctx->sessions);
  CU_ASSERT(session->state == COAP_SESSION_STATE_ESTABLISHED);
  coap_session_release(session);
}

/* This function creates a set of nodes for testing. These nodes
 * will exist for all tests and are modified by coap_insert_node()
 * and coap_remove_from_queue().
 */
static int
t_session_tests_create(void) {
  coap_address_t addr;
  coap_address_init(&addr);

  addr.size = sizeof(struct sockaddr_in6);
  addr.addr.sin6.sin6_family = AF_INET6;
  addr.addr.sin6.sin6_addr = in6addr_any;
  addr.addr.sin6.sin6_port = htons(COAP_DEFAULT_PORT);

  ctx = coap_new_context(&addr);

  if (ctx != NULL) {
    addr.addr.sin6.sin6_addr = in6addr_loopback;
    session = coap_new_client_session(ctx, NULL, &addr, COAP_PROTO_UDP);
  }

  return (ctx == NULL) || (session == NULL);
}

static int
t_session_tests_remove(void) {
  coap_free_context(ctx);
  return 0;
}

CU_pSuite
t_init_session_tests(void) {
  CU_pSuite suite;

  suite = CU_add_suite("session",
                       t_session_tests_create, t_session_tests_remove);
  if (!suite) {                        /* signal error */
    fprintf(stderr, "W: cannot add session test suite (%s)\n",
            CU_get_error_msg());

    return NULL;
  }

#define SESSION_TEST(s,t)                                              \
  if (!CU_ADD_TEST(s,t)) {                                              \
    fprintf(stderr, "W: cannot add session test (%s)\n",              \
            CU_get_error_msg());                                      \
  }

  SESSION_TEST(suite, t_session1);
  SESSION_TEST(suite, t_session2);
  SESSION_TEST(suite, t_session3);
  SESSION_TEST(suite, t_session4);
  SESSION_TEST(suite, t_session5);
  SESSION_TEST(suite, t_session6);

  return suite;
}
