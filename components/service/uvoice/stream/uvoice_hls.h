/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef __HLS_H__
#define __HLS_H__


static int hls_index_get(char *desc);
int uvoice_hls_index_update(http_loader_t *loader, int index);
int uvoice_hls_file_next(http_loader_t *loader);
int uvoice_hls_build(http_loader_t *loader);
int uvoice_hls_release(http_loader_t *loader);


#endif /* __HLS_H__ */
