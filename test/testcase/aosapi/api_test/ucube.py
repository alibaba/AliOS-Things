src = Split('''
    test_kernel.c
    aos_sys_test.c
    aos_task_test.c
    aos_mm_test.c
    aos_mutex_test.c
    aos_queue_test.c
    aos_sem_test.c
    aos_timer_test.c
    aos_workqueue_test.c
    aos_event_test.c
''')

component = aos_component('api_test', src)

component.add_comp_deps('kernel/rhino')

component.add_cflags('-Wall')
component.add_cflags('-Werror')

