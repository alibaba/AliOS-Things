#ifndef __NET_WORKQUEUE_H__
#define __NET_WORKQUEUE_H__

#include "cmsis_os.h"
#include "net_timer.h"
#include "net_mutex.h"

extern osMailQId  workqueue_queue_id;
//check this struct , it's should alloc buffer first while use it
struct workqueue_struct {
	struct list_head head;
	struct net_mutex mutex;
};

struct work_struct;
typedef void (*work_func_t)(struct work_struct *work);

struct work_mail_struct{
	struct work_struct * work_p;
	int    canceled;
};

enum workqueue_status{
	WORKQUEUE_FREE = 0,
	WORKQUEUE_INQUEUE = 1,
	WORKQUEUE_GOING = WORKQUEUE_FREE,
	WORKQUEUE_CANCELED = WORKQUEUE_FREE
};

struct work_struct{
	struct list_head list;
	work_func_t func;
	struct net_mutex lock;
	enum workqueue_status status;
	struct workqueue_struct* wq;
	struct work_mail_struct* mail_work;
};

struct delayed_work{
	struct workqueue_struct* wq;
	struct work_struct work;
	struct timer_list timer;
};


#define work_pending(work)\
				(((work)->status==WORKQUEUE_INQUEUE)?N_TRUE:N_FALSE)


extern void net_workqueue_init(void);
extern void alloc_ordered_workqueue(struct workqueue_struct *wq);
extern void workqueue_timer_callback(unsigned long argument);
extern bool queue_work_def(struct workqueue_struct *wq,
				struct work_struct *work);
extern bool queue_delayed_work(struct workqueue_struct *wq,
				struct delayed_work *dwork,
				unsigned long delay);
extern bool cancel_work_sync(struct work_struct *work);
extern bool cancel_delayed_work(struct delayed_work *dwork);
extern bool cancel_delayed_work_sync(struct delayed_work *dwork);
extern void destroy_workqueue(struct workqueue_struct *wq);
extern void flush_workqueue(struct workqueue_struct *wq);
extern bool flush_delayed_work(struct delayed_work *dwork);
extern void INIT_WORK_DEF( struct work_struct * work, work_func_t func);
extern void INIT_DELAYED_WORK(struct delayed_work * delayed_work, work_func_t func);

#define queue_work(wq, work) ({\
	bool __ret = 0;\
	__ret = queue_work_def(wq, work);\
	__ret;\
})


#define schedule_work(x, y) queue_work(x, y)

#define INIT_WORK(x, y) { \
	INIT_WORK_DEF(x, y);\
}

#endif

