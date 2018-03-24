src = Split('''
    test_fw.c 
    test_self_entry.c 
    core/buf_queue/buf_queue_del.c 
    core/buf_queue/buf_queue_dyn_create.c 
    core/buf_queue/buf_queue_flush.c 
    core/buf_queue/buf_queue_info_get.c 
    core/buf_queue/buf_queue_recv.c 
    core/buf_queue/buf_queue_test.c 
    core/event/event_break.c 
    core/event/event_opr.c 
    core/event/event_param.c 
    core/event/event_reinit.c 
    core/event/event_test.c 
    core/mm/mm_break.c 
    core/mm/mm_opr.c 
    core/mm/mm_param.c 
    core/mm/mm_test.c 
    core/mm_blk/mm_blk_break.c 
    core/mm_blk/mm_blk_fragment.c 
    core/mm_blk/mm_blk_opr.c 
    core/mm_blk/mm_blk_param.c 
    core/mm_blk/mm_blk_reinit.c 
    core/mm_blk/mm_blk_test.c 
    core/mutex/mutex_opr.c 
    core/mutex/mutex_param.c 
    core/mutex/mutex_reinit.c 
    core/mutex/mutex_test.c 
    core/queue/queue_back_send.c 
    core/queue/queue_del.c 
    core/queue/queue_flush.c 
    core/queue/queue_info_get.c 
    core/queue/queue_is_full.c 
    core/queue/queue_nowait_recv.c 
    core/queue/queue_test.c 
    core/sem/sem_break.c 
    core/sem/sem_count.c 
    core/sem/sem_param.c 
    core/sem/sem_reinit.c 
    core/sem/sem_test.c 
    core/sem/sem_opr.c 
    core/sys/sys_opr.c 
    core/sys/sys_test.c 
    core/task/task_sleep.c 
    core/task/task_del.c  
    core/task/task_suspend_test.c 
    core/task/task_test.c 
    core/task/task_yield_test.c 
    core/task/task_misc_test.c 
    core/task_sem/tasksem_count.c 
    core/task_sem/tasksem_opr.c 
    core/task_sem/tasksem_param.c 
    core/task_sem/tasksem_test.c 
    core/time/time_opr.c 
    core/time/time_test.c 
    core/timer/timer_change.c 
    core/timer/timer_create_del.c 
    core/timer/timer_dyn_create_del.c 
    core/timer/timer_start_stop.c 
    core/timer/timer_test.c 
    core/workqueue/workqueue_test.c 
    core/workqueue/workqueue_interface.c 
    core/ringbuf/ringbuf_break.c 
    core/ringbuf/ringbuf_test.c 
    core/combination/comb_test.c 
    core/combination/sem_event.c 
    core/combination/sem_queue_buf.c 
    core/combination/sem_mutex.c 
''')

component = aos_component('test', src)
component.add_global_includes('.')

if aos_global_config.compiler == 'gcc':
    component_cflags = Split('''
        -Wall 
        -Werror 
        -Wno-unused-variable 
        -Wno-unused-parameter 
        -Wno-implicit-function-declaration
        -Wno-type-limits 
        -Wno-sign-compare 
        -Wno-pointer-sign 
        -Wno-uninitialized
        -Wno-return-type 
        -Wno-unused-function 
        -Wno-unused-but-set-variable
        -Wno-unused-value 
        -Wno-strict-aliasing
    ''')
    for i in component_cflags:
        component.add_cflags(i)


