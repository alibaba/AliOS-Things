
#include <string.h>
#include "k_api.h"
#include "aos_obj_syscall_arg.h"
#include "object_api.h"
#include "task_group.h"
#include "aos/kernel.h"

extern obj_class_t         *aos_sem_class_id;
extern obj_class_t         *aos_event_class_id;
extern obj_class_t         *aos_mutex_class_id;
extern obj_class_t         *aos_queue_class_id;
extern obj_class_t         *aos_workqueue_class_id;
extern obj_class_t         *aos_work_class_id;
extern obj_class_t         *aos_timer_class_id;

int sys_aos_handle_delete_stub(aos_obj_delete_syscall_arg_t *arg)
{
    return obj_handle_delete(arg->handle, arg->option);
}

int sys_aos_handle_close_stub(aos_obj_comm_syscall_arg_t *arg)
{
    void *proc_id = get_task_group();
    return obj_handle_close(arg->handle, proc_id);
}
int sys_aos_handle_valid_stub(aos_obj_comm_syscall_arg_t *arg)
{
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return 0;
    }

    (void) obj_core_release(obj_id);
    return 1;
}

/* aos_sem_xxx stub */
int sys_aos_sem_open_stub(aos_sem_open_syscall_arg_t *arg)
{
    int ret;
    obj_handle_t handle = 0;
    aos_hdl_t    k_hdl;
    aos_hdl_t    *user_hdl;
    void *obj_id;
    void *proc_id = get_task_group();

    ret = aos_sem_open(&k_hdl, arg->name, arg->flag, arg->count);

    if (ret != 0) {
        return ret;
    }

    obj_id = k_hdl.hdl;
    handle = obj_handle_alloc(obj_id, proc_id);

    if (handle == 0) {
        obj_close(obj_id, aos_sem_class_id);
        return -EINVAL;
    }
    user_hdl = arg->phandle;
    user_hdl->hdl = (void *)handle;
    return 0;
}

int sys_aos_sem_new_stub(aos_sem_new_syscall_arg_t *arg)
{
    int ret;
    obj_handle_t handle = 0;
    aos_hdl_t    k_hdl;
    aos_hdl_t    *user_hdl;
    void *obj_id;
    void *proc_id = get_task_group();
    ret = aos_sem_new(&k_hdl, arg->count);
    if (ret != 0) {
        return ret;
    }

    obj_id = k_hdl.hdl;
    handle = obj_handle_alloc(obj_id, proc_id);

    if (handle == 0) {
        obj_close(obj_id, aos_sem_class_id);
        return -EINVAL;
    }
    user_hdl = arg->phandle;
    user_hdl->hdl = (void *)handle;
    return 0;
}

int sys_aos_sem_wait_stub(aos_sem_wait_syscall_arg_t *arg)
{
    int ret;
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return -EINVAL;
    }
    aos_hdr.hdl = obj_id;
    ret = aos_sem_wait(&aos_hdr, arg->timeout);
    (void) obj_core_release(obj_id);

    return ret;
}

void sys_aos_sem_signal_stub(aos_obj_comm_syscall_arg_t *arg)
{
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return;
    }
    aos_hdr.hdl = obj_id;
    (void) aos_sem_signal(&aos_hdr);

    (void) obj_core_release(obj_id);
}

void sys_aos_sem_signal_all_stub(aos_obj_comm_syscall_arg_t *arg)
{
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return;
    }
    aos_hdr.hdl = obj_id;
    (void) aos_sem_signal_all(&aos_hdr);

    (void) obj_core_release(obj_id);
}

int sys_aos_sem_getvalue_stub(aos_sem_getvalue_syscall_arg_t *arg)
{
    int ret;
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return -EINVAL;
    }
    aos_hdr.hdl = obj_id;
    ret = aos_sem_getvalue(&aos_hdr, arg->val);

    (void) obj_core_release(obj_id);
    return ret;
}

int sys_aos_sem_unlink_stub(aos_obj_unlink_syscall_arg_t *arg)
{
    return aos_sem_unlink(arg->name);
}

/* aos_event_xxx stub */
int sys_aos_event_open_stub(aos_event_open_syscall_arg_t *arg)
{
    int ret;
    obj_handle_t handle = 0;
    aos_hdl_t    k_hdl;
    aos_hdl_t    *user_hdl;
    void *obj_id;
    void *proc_id = get_task_group();
    ret = aos_event_open(&k_hdl, arg->name, arg->flag, arg->count);

    if (ret != 0) {
        return ret;
    }

    obj_id = k_hdl.hdl;

    handle = obj_handle_alloc(obj_id, proc_id);

    if (handle == 0) {
        obj_close(obj_id, aos_sem_class_id);
        return -EINVAL;
    }
    user_hdl = arg->phandle;
    user_hdl->hdl = (void *)handle;
    return 0;
}


int sys_aos_event_new_stub(aos_event_new_syscall_arg_t *arg)
{
    int ret;
    obj_handle_t handle = 0;
    aos_hdl_t    k_hdl;
    aos_hdl_t    *user_hdl;
    void *obj_id;
    void *proc_id = get_task_group();
    ret = aos_event_new(&k_hdl, arg->count);
    if (ret != 0) {
        return ret;
    }

    obj_id = k_hdl.hdl;
    handle = obj_handle_alloc(obj_id, proc_id);

    if (handle == 0) {
        obj_close(obj_id, aos_sem_class_id);
        return -EINVAL;
    }
    user_hdl = arg->phandle;
    user_hdl->hdl = (void *)handle;
    return 0;
}

int sys_aos_event_get_stub(aos_event_get_syscall_arg_t *arg)
{
    int ret;
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return -EINVAL;
    }
    aos_hdr.hdl = obj_id;
    ret = aos_event_get(&aos_hdr, arg->value, arg->opt, arg->actl_value, arg->timeout);
    (void) obj_core_release(obj_id);
    return ret;
}

int sys_aos_event_set_stub(aos_event_set_syscall_arg_t *arg)
{
    int ret;
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return -EINVAL;
    }
    aos_hdr.hdl = obj_id;
    ret = aos_event_set(&aos_hdr, arg->value, arg->opt);
    (void) obj_core_release(obj_id);
    return ret;
}

int sys_aos_event_unlink_stub(aos_obj_unlink_syscall_arg_t *arg)
{
    return aos_event_unlink(arg->name);
}

/* aos_mutex_xxx stub */
int sys_aos_mutex_new_stub(aos_mutex_new_syscall_arg_t *arg)
{
    int ret;
    obj_handle_t handle = 0;
    aos_hdl_t    k_hdl;
    aos_hdl_t    *user_hdl;
    void *obj_id;
    void *proc_id = get_task_group();
    ret = aos_mutex_new(&k_hdl);
    if (ret != 0) {
        return ret;
    }

    obj_id = k_hdl.hdl;
    handle = obj_handle_alloc(obj_id, proc_id);

    if (handle == 0) {
        obj_close(obj_id, aos_sem_class_id);
        return -EINVAL;
    }
    user_hdl = arg->phandle;
    user_hdl->hdl = (void *)handle;
    return 0;
}

int sys_aos_mutex_lock_stub(aos_mutex_wait_syscall_arg_t *arg)
{
    int ret;
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return -EINVAL;
    }
    aos_hdr.hdl = obj_id;
    ret = aos_mutex_lock(&aos_hdr, arg->timeout);
    (void) obj_core_release(obj_id);

    return ret;
}

int sys_aos_mutex_unlock_stub(aos_mutex_wait_syscall_arg_t *arg)
{
    int ret;
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return -EINVAL;
    }
    aos_hdr.hdl = obj_id;
    ret = aos_mutex_unlock(&aos_hdr);

    (void) obj_core_release(obj_id);
    return ret;
}

/* aos_queue_xxx stub */
int sys_aos_queue_open_stub(aos_queue_open_syscall_arg_t *arg)
{
    int ret;
    obj_handle_t handle = 0;
    aos_hdl_t    k_hdl;
    aos_hdl_t    *user_hdl;
    void *obj_id;
    void *proc_id = get_task_group();
    ret = aos_queue_open(&k_hdl, arg->name, arg->flag, arg->size, arg->max_msg);
    if (ret != 0) {
        return ret;
    }

    obj_id = k_hdl.hdl;
    handle = obj_handle_alloc(obj_id, proc_id);

    if (handle == 0) {
        obj_close(obj_id, aos_sem_class_id);
        return -EINVAL;
    }
    user_hdl = arg->phandle;
    user_hdl->hdl = (void *)handle;
    return 0;
}


int sys_aos_queue_new_stub(aos_queue_new_syscall_arg_t *arg)
{
    int ret;
    obj_handle_t handle = 0;
    aos_hdl_t    k_hdl;
    aos_hdl_t    *user_hdl;
    void *obj_id;
    void *proc_id = get_task_group();
    ret = aos_queue_new(&k_hdl, arg->size, arg->max_msg);
    if (ret != 0) {
        return ret;
    }

    obj_id = k_hdl.hdl;
    handle = obj_handle_alloc(obj_id, proc_id);

    if (handle == 0) {
        obj_close(obj_id, aos_sem_class_id);
        return -EINVAL;
    }

    user_hdl = arg->phandle;
    user_hdl->hdl = (void *)handle;
    return 0;
}

int sys_aos_queue_send_stub(aos_queue_send_syscall_arg_t *arg)
{
    int ret;
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return -EINVAL;
    }
    aos_hdr.hdl = obj_id;
    ret = aos_queue_send(&aos_hdr, arg->msg, arg->size);
    (void) obj_core_release(obj_id);

    return ret;
}

int sys_aos_queue_recv_stub(aos_queue_recv_syscall_arg_t *arg)
{
    int ret;
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return -EINVAL;
    }
    aos_hdr.hdl = obj_id;
    ret = aos_queue_recv(&aos_hdr, arg->ms, arg->msg, arg->size);
    (void) obj_core_release(obj_id);

    return ret;
}

int sys_aos_queue_get_count_stub(aos_obj_comm_syscall_arg_t *arg)
{
    int ret;
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return -EINVAL;
    }

    aos_hdr.hdl = obj_id;
    ret = aos_queue_get_count(&aos_hdr);
    (void) obj_core_release(obj_id);

    return ret;
}

int sys_aos_queue_unlink_stub(aos_obj_unlink_syscall_arg_t *arg)
{
    return aos_queue_unlink(arg->name);
}

/* aos_timer_xxx stub */
#if (AOS_KOBJ_TIMER > 0)
int sys_aos_timer_new_ext_stub(aos_timer_new_syscall_arg_t *arg)
{
    int ret;
    obj_handle_t handle = 0;
    aos_hdl_t    k_hdl;
    aos_hdl_t    *user_hdl;
    void *obj_id;
    void *proc_id = get_task_group();

    ret =  aos_timer_new_ext(&k_hdl, arg->fn, arg->arg,
                             arg->ms, arg->repeat, arg->auto_run);

    if (ret != 0) {
        return ret;
    }

    obj_id = k_hdl.hdl;
    handle = obj_handle_alloc(obj_id, proc_id);

    if (handle == 0) {
        obj_close(obj_id, aos_sem_class_id);
        return -EINVAL;
    }

    user_hdl = arg->phandle;
    user_hdl->hdl = (void *)handle;
    return 0;
}

int sys_aos_timer_start_stub(aos_obj_comm_syscall_arg_t *arg)
{
    int ret;
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return -EINVAL;
    }
    aos_hdr.hdl = obj_id;
    ret = aos_timer_start(&aos_hdr);
    (void) obj_core_release(obj_id);

    return ret;
}

int sys_aos_timer_stop_stub(aos_obj_comm_syscall_arg_t *arg)
{
    int ret;
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return -EINVAL;
    }
    aos_hdr.hdl = obj_id;
    ret = aos_timer_stop(&aos_hdr);
    (void) obj_core_release(obj_id);

    return ret;
}

int sys_aos_timer_change_stub(aos_timer_change_syscall_arg_t *arg)
{
    int ret;
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return -EINVAL;
    }
    aos_hdr.hdl = obj_id;
    ret = aos_timer_change(&aos_hdr, arg->ms);
    (void) obj_core_release(obj_id);

    return ret;
}

int sys_aos_timer_change_once_stub(aos_timer_change_syscall_arg_t *arg)
{
    int ret;
    aos_hdl_t aos_hdr;
    void *proc_id = get_task_group();
    void *obj_id = obj_handle_to_obj_id((obj_handle_t)arg->handle, proc_id, RHINO_TRUE);
    if (obj_id == NULL) {
        return -EINVAL;
    }
    aos_hdr.hdl = obj_id;
    ret = aos_timer_change_once(&aos_hdr, arg->ms);
    (void) obj_core_release(obj_id);

    return ret;
}
#endif
/* ------------------ futex ------------------ */
int sys_aos_futex_stub(void *arg)
{
    aos_futex_syscall_arg_t *_arg = arg;

    return aos_do_futex(_arg->uaddr, _arg->futex_op, _arg->val, _arg->timeout);
}
