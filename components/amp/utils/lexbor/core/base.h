/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_BASE_H
#define LEXBOR_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
//#include <memory.h>
#include <limits.h>
#include <string.h>

#include "def.h"
#include "types.h"
#include "lexbor.h"

#define LEXBOR_VERSION_MAJOR 1
#define LEXBOR_VERSION_MINOR 3
#define LEXBOR_VERSION_PATCH 1

#define LEXBOR_VERSION_STRING LEXBOR_STRINGIZE(LEXBOR_VERSION_MAJOR) "."       \
                              LEXBOR_STRINGIZE(LEXBOR_VERSION_MINOR) "."       \
                              LEXBOR_STRINGIZE(LEXBOR_VERSION_PATCH)

#define lexbor_assert(val)


/*
 * Very important!!!
 *
 * for lexbor 0..00AFFF; LXB_STATUS_OK == 0x000000
 */
typedef enum {
    LXB_STATUS_OK                       = 0x0000,
    LXB_STATUS_ERROR                    = 0x0001,
    LXB_STATUS_ERROR_MEMORY_ALLOCATION,
    LXB_STATUS_ERROR_OBJECT_IS_NULL,
    LXB_STATUS_ERROR_SMALL_BUFFER,
    LXB_STATUS_ERROR_INCOMPLETE_OBJECT,
    LXB_STATUS_ERROR_NO_FREE_SLOT,
    LXB_STATUS_ERROR_TOO_SMALL_SIZE,
    LXB_STATUS_ERROR_NOT_EXISTS,
    LXB_STATUS_ERROR_WRONG_ARGS,
    LXB_STATUS_ERROR_WRONG_STAGE,
    LXB_STATUS_ERROR_UNEXPECTED_RESULT,
    LXB_STATUS_ERROR_UNEXPECTED_DATA,
    LXB_STATUS_CONTINUE,
    LXB_STATUS_SMALL_BUFFER,
    LXB_STATUS_ABORTED,
    LXB_STATUS_STOPPED,
    LXB_STATUS_NEXT,
    LXB_STATUS_STOP,
}
lexbor_status_t;

typedef enum {
    LEXBOR_ACTION_OK    = 0x00,
    LEXBOR_ACTION_STOP  = 0x01,
    LEXBOR_ACTION_NEXT  = 0x02
}
lexbor_action_t;


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_BASE_H */

