#ifndef MQUEUE_H
#define MQUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <k_api.h>
#include <time.h>

#define DEFAULT_MQUEUE_SIZE (10 * 1024)

typedef kbuf_queue_t *mqd_t;

struct mq_attr {
    long mq_flags;    /* message queue flags */
    long mq_maxmsg;   /* maximum number of messages */
    long mq_msgsize;  /* maximum message size */
    long mq_curmsgs;  /* number of messages currently queued */
};

mqd_t mq_open(const char *name, int oflag, ...);
int mq_close(mqd_t mqdes);

ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio);
int     mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio);
int     mq_setattr(mqd_t                 mqdes,
                   const struct mq_attr *mqstat,
                   struct mq_attr       *omqstat);
int     mq_getattr(mqd_t mqdes, struct mq_attr *mqstat);

ssize_t mq_timedreceive(mqd_t                  mqdes,
                        char                  *msg_ptr,
                        size_t                 msg_len,
                        unsigned              *msg_prio,
                        const struct timespec *abs_timeout);
int     mq_timedsend(mqd_t                  mqdes,
                     const char            *msg_ptr,
                     size_t                 msg_len,
                     unsigned               msg_prio,
                     const struct timespec *abs_timeout);

int     mq_unlink(const char *name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
