src = Split('''
        platform/rhino/espos_misc.c      
        platform/rhino/espos_mutex.c     
        platform/rhino/espos_queue.c     
        platform/rhino/espos_scheduler.c 
        platform/rhino/espos_semaphore.c 
        platform/rhino/espos_spinlock.c  
        platform/rhino/espos_task.c      
        platform/rhino/espos_time.c      
        platform/rhino/espos_timer.c
''')

component = aos_component('espos', src)
component.add_global_includes('include')