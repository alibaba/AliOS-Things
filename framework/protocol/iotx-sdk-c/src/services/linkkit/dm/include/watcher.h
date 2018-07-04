#ifndef __LINKKIT_WATCHER_H__
#define __LINKKIT_WATCHER_H__

typedef struct watcher_s watcher_t;

int watcher_init(void);
int watcher_exit(void);

watcher_t *watcher_new(int id);
int watcher_free(watcher_t *w);

int watcher_wait(watcher_t *w, int *code, char **data, int *data_len, int timeout_ms);
int watcher_signal(int id, int code, void *data, int data_len);

#endif /* __LINKKIT_WATCHER_H__ */
