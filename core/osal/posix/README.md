## Contents

## Introduction
**POSIX** The Portable Operating System Interface (POSIX) is a family of standards specified by the IEEE Computer Society for maintaining compatibility between operating systems. POSIX defines the application programming interface (API), along with command line shells and utility interfaces, for software compatibility with variants of Unix and other operating systems.

### Features
- pthread
- semaphore
- mutex
- cond
- timer
- mutex
- device IO
- file system

### Directories

```sh
posix
├── aos.mk                       # makefile of posix
├── dirent.c                     # implement of device IO and file system
├── enviro.c                     # implement of environ variable
├── posix_init.c                 # implement of posix init
├── mqueue.c                     # implement of queue
├── prctl.h                      # implement of prctl
├── semaphore.c                  # implement of semaphore
├── timer.c                      # implement of timer
├── pthread.c                    # implement of pthread
├── pthread_attr.h               # implement of pthread attribute
├── pthread_cond.c               # implement of cond
├── pthread_mutex.c              # implement of mutex
├── pthread_sched.c              # implement of pthread sched
├── pthread_tsd.h                # implement of thread specific data
```
### How to use POSIX
#### step1. Add POSIX component to the project
    For example, if you want to use POSIX in board developerkit, you can add the following code to "board/developerkit/aos.mk"
```
$(NAME)_COMPONENTS  += osal_posix
```
#### step2. Config POSIX
    open "include/posix/posix_config.h" to enable the features you need, all of features are enabled by default.
    note: 1.If you want to use function "tmpfile", ramfs must be added to the project.
          2.If you want to use "pathconf/fpathconf", you must open "kernel/fs/vfs/include/vfs_conf.h" and set "VFS_CONFIG_CURRENT_DIRECTORY_ENABLE" to 1.
          3.If you want to use Pthread, you must add the following code to "hook_impl.c" or "k_config.c".
          For example, if you want to use Pthread in board developerkit, you must open "platform/mcu/stm32l4xx_cube/aos/hook_impl.c" and add the following code.

```C
void krhino_task_del_hook(ktask_t *task, res_free_t *arg)
{
    _pthread_tcb_t *ptcb;
    _pthread_cleanup_t *cleanup;
    pthread_key_list_t *pthread_key_list_s_c = NULL;

    g_sched_lock[cpu_cur_get()]++;

    ptcb = _pthread_get_tcb(task);

    if (ptcb == NULL) {
        g_sched_lock[cpu_cur_get()]--;
        return;
    }

    /* excute all cleanup function if existed */
    do {
        cleanup = ptcb->cleanup;
        if (cleanup != NULL ) {
            ptcb->cleanup = cleanup->prev;
            cleanup->cleanup_routine(cleanup->para);
            krhino_mm_free(cleanup);
        }
    } while(ptcb->cleanup != NULL);

    /* call the destructor function of TSD */
    pthread_key_list_s_c = &pthread_key_list_head;
    while (pthread_key_list_s_c != NULL) {
        if (pthread_key_list_s_c->head.fun != NULL){
            pthread_key_list_s_c->head.fun(NULL);
        }

        pthread_key_list_s_c = pthread_key_list_s_c->next;
    }

    if (ptcb->attr.detachstate == PTHREAD_CREATE_JOINABLE) {
        /* give join sem if is joinable */
        if (ptcb->join_sem != NULL) {
            krhino_sem_give(ptcb->join_sem);
        }
    }

    if (arg == NULL) {
        g_sched_lock[cpu_cur_get()]--;
        return;
    }

    if (ptcb->attr.detachstate == PTHREAD_CREATE_DETACHED) {
        if (ptcb->join_sem != NULL) {
            krhino_sem_dyn_del(ptcb->join_sem);
        }

        klist_insert(&g_res_list, &arg->res_list);
        arg->res[0] = task->task_stack_base;
        arg->res[1] = task;
        arg->res[2] = ptcb;
        arg->cnt += 3;
        krhino_sem_give(&g_res_sem);
    }

    g_sched_lock[cpu_cur_get()]--;
}
```
#### step3. Init POSIX
    Call function "posix_init" to initialize POSIX.

You can use POSIX after completing these steps.

### Dependencies

## Reference
- http://pubs.opengroup.org/
