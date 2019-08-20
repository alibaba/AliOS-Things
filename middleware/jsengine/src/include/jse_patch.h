/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef JSE_PATCH_H
#define JSE_PATCH_H

/* redefine CLI_CONFIG_INBUF_SIZE to 1024 for cli component */
#ifdef CLI_CONFIG_INBUF_SIZE
#undef CLI_CONFIG_INBUF_SIZE
#define CLI_CONFIG_INBUF_SIZE 1024
#endif

#endif /* JSE_PATCH_H */