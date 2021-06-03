/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CONV_H
#define LEXBOR_CONV_H

#ifdef __cplusplus
extern "C" {
#endif


#include "base.h"


LXB_API size_t
lexbor_conv_float_to_data(double num, lxb_char_t *buf, size_t len);

LXB_API double
lexbor_conv_data_to_double(const lxb_char_t **start, size_t len);

LXB_API unsigned long
lexbor_conv_data_to_ulong(const lxb_char_t **data, size_t length);

LXB_API unsigned
lexbor_conv_data_to_uint(const lxb_char_t **data, size_t length);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CONV_H */
