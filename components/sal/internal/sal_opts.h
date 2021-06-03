/*!
 * @file sal_opts.h
 *
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _SAL_OPTS_H_
#define _SAL_OPTS_H_

/** @addtogroup aos_sal API
 *  The definition of the SAL options.
 *
 *  @{
 */

#define SAL_OPT_ENABLED       1
#define SAL_OPT_DISABLED      0

/**
 * Socket API select support.
 */
#ifndef SAL_SELECT_SUPPORT
#define SAL_SELECT_SUPPORT            SAL_OPT_ENABLED
#endif

/**
 * UDP client support.
 */
#ifndef SAL_UDP_CLIENT_ENABLED
#define SAL_UDP_CLIENT_ENABLED        SAL_OPT_ENABLED
#endif

/**
 * Asynchronized send mode support.
 */
#ifndef SAL_PACKET_SEND_MODE_ASYNC
#define SAL_PACKET_SEND_MODE_ASYNC    SAL_OPT_DISABLED
#endif

/**
 * Debug, assert, error log support.
 */
#ifndef SAL_USE_DEBUG
#define SAL_USE_DEBUG                 SAL_OPT_DISABLED
#endif

/**
 * Input mbox default size.
 */
#ifndef SAL_DEFAULT_INPUTMBOX_SIZE
#define SAL_DEFAULT_INPUTMBOX_SIZE        32
#endif

/**
 * Output mbox default size. Only effective if
 * SAL_PACKET_SEND_MODE_ASYNC enabled.
 */
#ifndef SAL_DEFAULT_OUTPUTMBOX_SIZE
#define SAL_DEFAULT_OUTPUTMBOX_SIZE       8
#endif

/** @} */ /* end of group aos_sal */

#endif
