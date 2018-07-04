#ifndef __LK_QUEUE_H__
#define __LK_QUEUE_H__

typedef struct lk_queue_s lk_queue_t;

lk_queue_t *lk_queue_new(int max_nodes);
int lk_queue_free(lk_queue_t *q);

void *lk_queue_get(lk_queue_t *q, uint32_t timeout_ms);
int lk_queue_put(lk_queue_t *q, void *data);

#endif /* __LK_QUEUE_H__ */
