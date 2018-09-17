/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef NGHTTP2_HD_HUFFMAN_H
#define NGHTTP2_HD_HUFFMAN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <nghttp2/nghttp2.h>

typedef enum {
  /* FSA accepts this state as the end of huffman encoding
     sequence. */
  NGHTTP2_HUFF_ACCEPTED = 1,
  /* This state emits symbol */
  NGHTTP2_HUFF_SYM = (1 << 1),
  /* If state machine reaches this state, decoding fails. */
  NGHTTP2_HUFF_FAIL = (1 << 2)
} nghttp2_huff_decode_flag;

typedef struct {
  /* huffman decoding state, which is actually the node ID of internal
     huffman tree.  We have 257 leaf nodes, but they are identical to
     root node other than emitting a symbol, so we have 256 internal
     nodes [1..255], inclusive. */
  uint8_t state;
  /* bitwise OR of zero or more of the nghttp2_huff_decode_flag */
  uint8_t flags;
  /* symbol if NGHTTP2_HUFF_SYM flag set */
  uint8_t sym;
} nghttp2_huff_decode;

typedef nghttp2_huff_decode huff_decode_table_type[16];

typedef struct {
  /* Current huffman decoding state. We stripped leaf nodes, so the
     value range is [0..255], inclusive. */
  uint8_t state;
  /* nonzero if we can say that the decoding process succeeds at this
     state */
  uint8_t accept;
} nghttp2_hd_huff_decode_context;

typedef struct {
  /* The number of bits in this code */
  uint32_t nbits;
  /* Huffman code aligned to LSB */
  uint32_t code;
} nghttp2_huff_sym;

extern const nghttp2_huff_sym huff_sym_table[];
extern const nghttp2_huff_decode huff_decode_table[][16];

#endif /* NGHTTP2_HD_HUFFMAN_H */
