/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_BASE_H
#define LEXBOR_CSS_SYNTAX_BASE_H

#ifdef __cplusplus
extern "C" {
#endif


#include "core/base.h"


#define LXB_CSS_SYNTAX_VERSION_MAJOR 0
#define LXB_CSS_SYNTAX_VERSION_MINOR 1
#define LXB_CSS_SYNTAX_VERSION_PATCH 2

#define LXB_CSS_SYNTAX_VERSION_STRING                                          \
    LEXBOR_STRINGIZE(LXB_CSS_SYNTAX_VERSION_MAJOR) "."                         \
    LEXBOR_STRINGIZE(LXB_CSS_SYNTAX_VERSION_MINOR) "."                         \
    LEXBOR_STRINGIZE(LXB_CSS_SYNTAX_VERSION_PATCH)


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SYNTAX_BASE_H */
