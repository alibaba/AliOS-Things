/*
 * option.c -- helpers for handling options in CoAP PDUs
 *
 * Copyright (C) 2010-2013 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */


#include "coap_config.h"

#if defined(HAVE_ASSERT_H) && !defined(assert)
# include <assert.h>
#endif

#include <stdio.h>
#include <string.h>

#include "libcoap.h"
#include "option.h"
#include "encode.h"                /* for coap_fls() */
#include "coap_debug.h"
#include "../coap2/mem.h"
#include "utlist.h"

#define ADVANCE_OPT(o,e,step) if ((e) < step) {           \
    coap_log(LOG_DEBUG, "cannot advance opt past end\n"); \
    return 0;                                             \
  } else {                                                \
    (e) -= step;                                          \
    (o) = ((o)) + step;                                   \
  }

/*
 * Used to prevent access to *opt when pointing to after end of buffer
 * after doing a ADVANCE_OPT()
 */
#define ADVANCE_OPT_CHECK(o,e,step) do { \
    ADVANCE_OPT(o,e,step);               \
    if ((e) < 1)                         \
      return 0;                          \
  } while (0)

size_t
coap_opt_parse(const coap_opt_t *opt, size_t length, coap_option_t *result) {

  const coap_opt_t *opt_start = opt; /* store where parsing starts  */

  assert(opt); assert(result);

  if (length < 1)
    return 0;

  result->delta = (*opt & 0xf0) >> 4;
  result->length = *opt & 0x0f;

  switch(result->delta) {
  case 15:
    if (*opt != COAP_PAYLOAD_START) {
      coap_log(LOG_DEBUG, "ignored reserved option delta 15\n");
    }
    return 0;
  case 14:
    /* Handle two-byte value: First, the MSB + 269 is stored as delta value.
     * After that, the option pointer is advanced to the LSB which is handled
     * just like case delta == 13. */
    ADVANCE_OPT_CHECK(opt,length,1);
    result->delta = ((*opt & 0xff) << 8) + 269;
    if (result->delta < 269) {
      coap_log(LOG_DEBUG, "delta too large\n");
      return 0;
    }
    /* fall through */
  case 13:
    ADVANCE_OPT_CHECK(opt,length,1);
    result->delta += *opt & 0xff;
    break;

  default:
    ;
  }

  switch(result->length) {
  case 15:
    coap_log(LOG_DEBUG, "found reserved option length 15\n");
    return 0;
  case 14:
    /* Handle two-byte value: First, the MSB + 269 is stored as delta value.
     * After that, the option pointer is advanced to the LSB which is handled
     * just like case delta == 13. */
    ADVANCE_OPT_CHECK(opt,length,1);
    result->length = ((*opt & 0xff) << 8) + 269;
    /* fall through */
  case 13:
    ADVANCE_OPT_CHECK(opt,length,1);
    result->length += *opt & 0xff;
    break;

  default:
    ;
  }

  /* ADVANCE_OPT() is correct here */
  ADVANCE_OPT(opt,length,1);
  /* opt now points to value, if present */

  result->value = opt;
  if (length < result->length) {
    coap_log(LOG_DEBUG, "invalid option length\n");
    return 0;
  }

#undef ADVANCE_OPT
#undef ADVANCE_OPT_CHECK

  return (opt + result->length) - opt_start;
}

coap_opt_iterator_t *
coap_option_iterator_init(const coap_pdu_t *pdu, coap_opt_iterator_t *oi,
                          const coap_opt_filter_t filter) {
  assert(pdu);
  assert(pdu->token);
  assert(oi);

  memset(oi, 0, sizeof(coap_opt_iterator_t));

  oi->next_option = pdu->token + pdu->token_length;
  if (pdu->token + pdu->used_size <= oi->next_option) {
    oi->bad = 1;
    return NULL;
  }

  oi->length = pdu->used_size - pdu->token_length;

  if (filter) {
    memcpy(oi->filter, filter, sizeof(coap_opt_filter_t));
    oi->filtered = 1;
  }
  return oi;
}

COAP_STATIC_INLINE int
opt_finished(coap_opt_iterator_t *oi) {
  assert(oi);

  if (oi->bad || oi->length == 0 ||
      !oi->next_option || *oi->next_option == COAP_PAYLOAD_START) {
    oi->bad = 1;
  }

  return oi->bad;
}

coap_opt_t *
coap_option_next(coap_opt_iterator_t *oi) {
  coap_option_t option;
  coap_opt_t *current_opt = NULL;
  size_t optsize;
  int b;                   /* to store result of coap_option_getb() */

  assert(oi);

  if (opt_finished(oi))
    return NULL;

  while (1) {
    /* oi->option always points to the next option to deliver; as
     * opt_finished() filters out any bad conditions, we can assume that
     * oi->option is valid. */
    current_opt = oi->next_option;

    /* Advance internal pointer to next option, skipping any option that
     * is not included in oi->filter. */
    optsize = coap_opt_parse(oi->next_option, oi->length, &option);
    if (optsize) {
      assert(optsize <= oi->length);

      oi->next_option += optsize;
      oi->length -= optsize;

      oi->type += option.delta;
    } else {                        /* current option is malformed */
      oi->bad = 1;
      return NULL;
    }

    /* Exit the while loop when:
     *   - no filtering is done at all
     *   - the filter matches for the current option
     *   - the filter is too small for the current option number
     */
    if (!oi->filtered ||
        (b = coap_option_getb(oi->filter, oi->type)) > 0)
      break;
    else if (b < 0) {                /* filter too small, cannot proceed */
      oi->bad = 1;
      return NULL;
    }
  }

  return current_opt;
}

coap_opt_t *
coap_check_option(coap_pdu_t *pdu, uint16_t type,
                  coap_opt_iterator_t *oi) {
  coap_opt_filter_t f;

  coap_option_filter_clear(f);
  coap_option_setb(f, type);

  coap_option_iterator_init(pdu, oi, f);

  return coap_option_next(oi);
}

uint16_t
coap_opt_delta(const coap_opt_t *opt) {
  uint16_t n;

  n = (*opt++ & 0xf0) >> 4;

  switch (n) {
  case 15: /* error */
    coap_log(LOG_WARNING, "coap_opt_delta: illegal option delta\n");

    /* This case usually should not happen, hence we do not have a
     * proper way to indicate an error. */
    return 0;
  case 14:
    /* Handle two-byte value: First, the MSB + 269 is stored as delta value.
     * After that, the option pointer is advanced to the LSB which is handled
     * just like case delta == 13. */
    n = ((*opt++ & 0xff) << 8) + 269;
    /* fall through */
  case 13:
    n += *opt & 0xff;
    break;
  default: /* n already contains the actual delta value */
    ;
  }

  return n;
}

uint16_t
coap_opt_length(const coap_opt_t *opt) {
  uint16_t length;

  length = *opt & 0x0f;
  switch (*opt & 0xf0) {
  case 0xf0:
    coap_log(LOG_DEBUG, "illegal option delta\n");
    return 0;
  case 0xe0:
    ++opt;
    /* fall through */
    /* to skip another byte */
  case 0xd0:
    ++opt;
    /* fall through */
    /* to skip another byte */
  default:
    ++opt;
  }

  switch (length) {
  case 0x0f:
    coap_log(LOG_DEBUG, "illegal option length\n");
    return 0;
  case 0x0e:
    length = (*opt++ << 8) + 269;
    /* fall through */
  case 0x0d:
    length += *opt++;
    break;
  default:
    ;
  }
  return length;
}

const uint8_t *
coap_opt_value(const coap_opt_t *opt) {
  size_t ofs = 1;

  switch (*opt & 0xf0) {
  case 0xf0:
    coap_log(LOG_DEBUG, "illegal option delta\n");
    return 0;
  case 0xe0:
    ++ofs;
    /* fall through */
  case 0xd0:
    ++ofs;
    break;
  default:
    ;
  }

  switch (*opt & 0x0f) {
  case 0x0f:
    coap_log(LOG_DEBUG, "illegal option length\n");
    return 0;
  case 0x0e:
    ++ofs;
    /* fall through */
  case 0x0d:
    ++ofs;
    break;
  default:
    ;
  }

  return (const uint8_t *)opt + ofs;
}

size_t
coap_opt_size(const coap_opt_t *opt) {
  coap_option_t option;

  /* we must assume that opt is encoded correctly */
  return coap_opt_parse(opt, (size_t)-1, &option);
}

size_t
coap_opt_setheader(coap_opt_t *opt, size_t maxlen,
                   uint16_t delta, size_t length) {
  size_t skip = 0;

  assert(opt);

  if (maxlen == 0)                /* need at least one byte */
    return 0;

  if (delta < 13) {
    opt[0] = (coap_opt_t)(delta << 4);
  } else if (delta < 269) {
    if (maxlen < 2) {
      coap_log(LOG_DEBUG, "insufficient space to encode option delta %d\n",
               delta);
      return 0;
    }

    opt[0] = 0xd0;
    opt[++skip] = (coap_opt_t)(delta - 13);
  } else {
    if (maxlen < 3) {
      coap_log(LOG_DEBUG, "insufficient space to encode option delta %d\n",
               delta);
      return 0;
    }

    opt[0] = 0xe0;
    opt[++skip] = ((delta - 269) >> 8) & 0xff;
    opt[++skip] = (delta - 269) & 0xff;
  }

  if (length < 13) {
    opt[0] |= length & 0x0f;
  } else if (length < 269) {
    if (maxlen < skip + 2) {
      coap_log(LOG_DEBUG, "insufficient space to encode option length %zu\n",
               length);
      return 0;
    }

    opt[0] |= 0x0d;
    opt[++skip] = (coap_opt_t)(length - 13);
  } else {
    if (maxlen < skip + 3) {
      coap_log(LOG_DEBUG, "insufficient space to encode option delta %d\n",
               delta);
      return 0;
    }

    opt[0] |= 0x0e;
    opt[++skip] = ((length - 269) >> 8) & 0xff;
    opt[++skip] = (length - 269) & 0xff;
  }

  return skip + 1;
}

size_t
coap_opt_encode_size(uint16_t delta, size_t length) {
  size_t n = 1;

  if (delta >= 13) {
    if (delta < 269)
      n += 1;
    else
      n += 2;
  }

  if (length >= 13) {
    if (length < 269)
      n += 1;
    else
      n += 2;
  }

  return n + length;
}

size_t
coap_opt_encode(coap_opt_t *opt, size_t maxlen, uint16_t delta,
                const uint8_t *val, size_t length) {
  size_t l = 1;

  l = coap_opt_setheader(opt, maxlen, delta, length);
  assert(l <= maxlen);

  if (!l) {
    coap_log(LOG_DEBUG, "coap_opt_encode: cannot set option header\n");
    return 0;
  }

  maxlen -= l;
  opt += l;

  if (maxlen < length) {
    coap_log(LOG_DEBUG, "coap_opt_encode: option too large for buffer\n");
    return 0;
  }

  if (val)                        /* better be safe here */
    memcpy(opt, val, length);

  return l + length;
}

/* coap_opt_filter_t has the following internal structure: */
typedef struct {
  uint16_t mask;

#define LONG_MASK ((1 << COAP_OPT_FILTER_LONG) - 1)
#define SHORT_MASK \
  (~LONG_MASK & ((1 << (COAP_OPT_FILTER_LONG + COAP_OPT_FILTER_SHORT)) - 1))

  uint16_t long_opts[COAP_OPT_FILTER_LONG];
  uint8_t short_opts[COAP_OPT_FILTER_SHORT];
} opt_filter;

/** Returns true iff @p type denotes an option type larger than 255. */
COAP_STATIC_INLINE int
is_long_option(uint16_t type) { return type > 255; }

/** Operation specifiers for coap_filter_op(). */
enum filter_op_t { FILTER_SET, FILTER_CLEAR, FILTER_GET };

/**
 * Applies @p op on @p filter with respect to @p type. The following
 * operations are defined:
 *
 * FILTER_SET: Store @p type into an empty slot in @p filter. Returns
 * @c 1 on success, or @c 0 if no spare slot was available.
 *
 * FILTER_CLEAR: Remove @p type from filter if it exists.
 *
 * FILTER_GET: Search for @p type in @p filter. Returns @c 1 if found,
 * or @c 0 if not found.
 *
 * @param filter The filter object.
 * @param type   The option type to set, get or clear in @p filter.
 * @param op     The operation to apply to @p filter and @p type.
 *
 * @return 1 on success, and 0 when FILTER_GET yields no
 * hit or no free slot is available to store @p type with FILTER_SET.
 */
static int
coap_option_filter_op(coap_opt_filter_t filter,
                      uint16_t type,
                      enum filter_op_t op) {
  size_t lindex = 0;
  opt_filter *of = (opt_filter *)filter;
  uint16_t nr, mask = 0;

  if (is_long_option(type)) {
    mask = LONG_MASK;

    for (nr = 1; lindex < COAP_OPT_FILTER_LONG; nr <<= 1, lindex++) {

      if (((of->mask & nr) > 0) && (of->long_opts[lindex] == type)) {
        if (op == FILTER_CLEAR) {
          of->mask &= ~nr;
        }

        return 1;
      }
    }
  } else {
    mask = SHORT_MASK;

    for (nr = 1 << COAP_OPT_FILTER_LONG; lindex < COAP_OPT_FILTER_SHORT;
         nr <<= 1, lindex++) {

      if (((of->mask & nr) > 0) && (of->short_opts[lindex] == (type & 0xff))) {
        if (op == FILTER_CLEAR) {
          of->mask &= ~nr;
        }

        return 1;
      }
    }
  }

  /* type was not found, so there is nothing to do if op is CLEAR or GET */
  if ((op == FILTER_CLEAR) || (op == FILTER_GET)) {
    return 0;
  }

  /* handle FILTER_SET: */

  lindex = coap_fls(~of->mask & mask);
  if (!lindex) {
    return 0;
  }

  if (is_long_option(type)) {
    of->long_opts[lindex - 1] = type;
  } else {
    of->short_opts[lindex - COAP_OPT_FILTER_LONG - 1] = (uint8_t)type;
  }

  of->mask |= 1 << (lindex - 1);

  return 1;
}

int
coap_option_filter_set(coap_opt_filter_t filter, uint16_t type) {
  return coap_option_filter_op(filter, type, FILTER_SET);
}

int
coap_option_filter_unset(coap_opt_filter_t filter, uint16_t type) {
  return coap_option_filter_op(filter, type, FILTER_CLEAR);
}

int
coap_option_filter_get(coap_opt_filter_t filter, uint16_t type) {
  /* Ugly cast to make the const go away (FILTER_GET wont change filter
   * but as _set and _unset do, the function does not take a const). */
  return coap_option_filter_op((uint16_t *)filter, type, FILTER_GET);
}

coap_optlist_t *
coap_new_optlist(uint16_t number,
                          size_t length,
                          const uint8_t *data
) {
  coap_optlist_t *node;

  node = coap_malloc_type(COAP_OPTLIST, sizeof(coap_optlist_t) + length);

  if (node) {
    memset(node, 0, (sizeof(coap_optlist_t) + length));
    node->number = number;
    node->length = length;
    node->data = (uint8_t *)&node[1];
    memcpy(node->data, data, length);
  } else {
    coap_log(LOG_WARNING, "coap_new_optlist: malloc failure\n");
  }

  return node;
}

static int
order_opts(void *a, void *b) {
  coap_optlist_t *o1 = (coap_optlist_t *)a;
  coap_optlist_t *o2 = (coap_optlist_t *)b;

  if (!a || !b)
    return a < b ? -1 : 1;

  return (int)(o1->number - o2->number);
}

int
coap_add_optlist_pdu(coap_pdu_t *pdu, coap_optlist_t** options) {
  coap_optlist_t *opt;

  if (options && *options) {
    /* sort options for delta encoding */
    LL_SORT((*options), order_opts);

    LL_FOREACH((*options), opt) {
      coap_add_option(pdu, opt->number, opt->length, opt->data);
    }
    return 1;
  }
  return 0;
}

int
coap_insert_optlist(coap_optlist_t **head, coap_optlist_t *node) {
  if (!node) {
    coap_log(LOG_DEBUG, "optlist not provided\n");
  } else {
    /* must append at the list end to avoid re-ordering of
     * options during sort */
    LL_APPEND((*head), node);
  }

  return node != NULL;
}

static int
coap_internal_delete(coap_optlist_t *node) {
  if (node) {
    coap_free_type(COAP_OPTLIST, node);
  }
  return 1;
}

void
coap_delete_optlist(coap_optlist_t *queue) {
  coap_optlist_t *elt, *tmp;

  if (!queue)
    return;

  LL_FOREACH_SAFE(queue, elt, tmp) {
    coap_internal_delete(elt);
  }
}

