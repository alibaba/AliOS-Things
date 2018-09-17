/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "sal_err.h"

/** Table to quickly map an sal error (err_t) to a socket error
  * by using -err as an index */
static const int err_to_errno_table[] = {
    0,             /* ERR_OK          0      No error, everything OK. */
    ENOMEM,        /* ERR_MEM        -1      Out of memory error.     */
    ENOBUFS,       /* ERR_BUF        -2      Buffer error.            */
    EWOULDBLOCK,   /* ERR_TIMEOUT    -3      Timeout                  */
    EHOSTUNREACH,  /* ERR_RTE        -4      Routing problem.         */
    EINPROGRESS,   /* ERR_INPROGRESS -5      Operation in progress    */
    EINVAL,        /* ERR_VAL        -6      Illegal value.           */
    EWOULDBLOCK,   /* ERR_WOULDBLOCK -7      Operation would block.   */
    EADDRINUSE,    /* ERR_USE        -8      Address in use.          */
    EALREADY,      /* ERR_ALREADY    -9      Already connecting.      */
    EISCONN,       /* ERR_ISCONN     -10     Conn already established.*/
    ENOTCONN,      /* ERR_CONN       -11     Not connected.           */
    -1,            /* ERR_IF         -12     Low-level netif error    */
    ECONNABORTED,  /* ERR_ABRT       -13     Connection aborted.      */
    ECONNRESET,    /* ERR_RST        -14     Connection reset.        */
    ENOTCONN,      /* ERR_CLSD       -15     Connection closed.       */
    EIO            /* ERR_ARG        -16     Illegal argument.        */
};

#ifdef SAL_DEBUG

static const char *err_strerr[] = {
    "Ok.",                    /* ERR_OK          0  */
    "Out of memory error.",   /* ERR_MEM        -1  */
    "Buffer error.",          /* ERR_BUF        -2  */
    "Timeout.",               /* ERR_TIMEOUT    -3  */
    "Routing problem.",       /* ERR_RTE        -4  */
    "Operation in progress.", /* ERR_INPROGRESS -5  */
    "Illegal value.",         /* ERR_VAL        -6  */
    "Operation would block.", /* ERR_WOULDBLOCK -7  */
    "Address in use.",        /* ERR_USE        -8  */
    "Already connecting.",    /* ERR_ALREADY    -9  */
    "Already connected.",     /* ERR_ISCONN     -10 */
    "Not connected.",         /* ERR_CONN       -11 */
    "Low-level netif error.", /* ERR_IF         -12 */
    "Connection aborted.",    /* ERR_ABRT       -13 */
    "Connection reset.",      /* ERR_RST        -14 */
    "Connection closed.",     /* ERR_CLSD       -15 */
    "Illegal argument."       /* ERR_ARG        -16 */
};

/**
 * Convert an lwip internal error to a string representation.
 *
 * @param err an lwip internal err_t
 * @return a string representation for err
 */
const char *
sal_strerr(err_t err)
{
    if ((err > 0) || (-err >= (err_t)sizeof(err_strerr))) {
        return "Unknown error.";
    }
    return err_strerr[-err];
}

#endif

int
err_to_errno(err_t err)
{
    if ((err > 0) || (-err >= (err_t)sizeof(err_to_errno_table))) {
        return EIO;
    }
    return err_to_errno_table[-err];
}
