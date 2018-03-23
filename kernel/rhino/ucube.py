src     = Split('''
                    core/k_err.c          
                   core/k_mm.c           
                   core/k_mm_debug.c     
                   core/k_ringbuf.c      
                   core/k_stats.c        
                   core/k_task_sem.c     
                   core/k_timer.c        
                   core/k_buf_queue.c    
                   core/k_event.c        
                   core/k_mm_blk.c       
                   core/k_mutex.c        
                   core/k_pend.c         
                   core/k_sched.c        
                   core/k_sys.c          
                   core/k_tick.c         
                   core/k_workqueue.c    
                   core/k_dyn_mem_proc.c 
                   core/k_idle.c         
                   core/k_obj.c          
                   core/k_queue.c        
                   core/k_sem.c          
                   core/k_task.c         
                   core/k_time.c         
                   common/k_fifo.c       
                   common/k_trace.c
''')
component = aos_component('rhino', src)

component.add_global_includes('core/include')

CONFIG_SYSINFO_KERNEL_VERSION = 'AOS-R-1.2.2'
component.add_global_macro({'SYSINFO_KERNEL_VERSION':'\\"AOS-R-1.2.2\\"'})
