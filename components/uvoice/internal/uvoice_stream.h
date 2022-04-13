/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_EXTRACTOR_H__
#define __UVOICE_EXTRACTOR_H__


int file_loader_create(media_loader_t *mloader, char *source);
int file_loader_release(media_loader_t *mloader);
int file_packer_create(media_packer_t *mpacker, char *sink);
int file_packer_release(media_packer_t *mpacker);

int http_loader_create(media_loader_t *mloader, char *source);
int http_loader_release(media_loader_t *mloader);

int partition_loader_create(media_loader_t *mloader, char *source);
int partition_loader_release(media_loader_t *mloader);
int partition_packer_create(media_packer_t *mpacker, char *sink);
int partition_packer_release(media_packer_t *mpacker);


#endif /* __UVOICE_EXTRACTOR_H__ */