/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _SAL_OPTS_H_
#define _SAL_OPTS_H_

#define SAL_OPT_ENABLED       1
#define SAL_OPT_DISABLED      0

/* Socket API select support. */
#define SAL_SELECT_SUPPORT            SAL_OPT_ENABLED

/* UDP client support. */
#define SAL_UDP_CLIENT_ENABLED        SAL_OPT_ENABLED

/* asynchronized send mode support. */
#define SAL_PACKET_SEND_MODE_ASYNC    SAL_OPT_DISABLED

/* debug, assert, error log support */
#define SAL_USE_DEBUG                 SAL_OPT_DISABLED

/* input mbox default size */
#define SAL_DEFAULT_INPUTMBOX_SIZE        32

/* output mbox default size. Only effective if SAL_PACKET_SEND_MODE_ASYNC enabled */
#define SAL_DEFAULT_OUTPUTMBOX_SIZE       8

#endif
