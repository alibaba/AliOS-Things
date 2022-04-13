#ifndef __NET_DOWNLOAD_H__
#define __NET_DOWNLOAD_H__


int net_download_work(net_cache_t *nc);
int new_download_abort(net_cache_t *nc);
net_cache_t *net_download_create(void);
int net_download_release(net_cache_t *nc);


#endif
