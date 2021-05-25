/**
 * Copyright (c) 2015, Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _OS_TASK_H_
#define _OS_TASK_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup  OS      OSIF
 * \defgroup    Task    Task Management
 *
 * \brief   Define, create, and control task functions.
 * \details The Task Management function group allows to create, delete, and control tasks
 *          in the system.\n
 * Tasks can be in the following states:\n
 * \arg <b>RUNNING:</b> The task that is currently running is in the <b>RUNNING</b> state.
 *                      Only one task at a time can be in this state.
 * \arg <b>READY:</b> Tasks which are ready to run are in the <b>READY</b> state. Once the
 *                    <b>RUNNING</b> task has terminated or is <b>WAITING</b>, the next
 *                    <b>READY</b> task with the highest priority becomes the <b>RUNNING</b> task.
 * \arg <b>WAITING:</b> Tasks that are waiting for an event to occur are in the WAITING state.
 * \arg <b>INACTIVE</b>: Tasks that are not created or terminated are in the <b>INACTIVE</b> state.
 *                       These Tasks typically consume no system resources.
 *
 * \image html OS-task-state-transition.jpg "Task State Transition" width=526px height=526px
 *
 * \ingroup     OS
 */


/**
 * os_task.h
 *
 * \brief Create a new task and add it to the list of tasks that are ready to run.
 *
 * \param[out]  pp_handle  Used to pass back a handle by which the created task
 *                         can be referenced.
 *
 * \param[in]   p_name     A descriptive name for the task.
 *
 * \param[in]   p_routine  Pointer to task routine function that must be implemented
 *                         to never return.
 *
 * \param[in]   p_param    Pointer parameter passed to the task routine function.
 *
 * \param[in]   stack_size The size of the task stack that is specified as the number
 *                         of bytes.
 *
 * \param[in]   priority   The priority at which the task should run. Higher priority
 *                         task has higher priority value.
 *
 * \return           The status of the task creation.
 * \retval true      Task was created successfully and added to task ready list.
 * \retval false     Task was failed to create.
 *
 * <b>Example usage</b>
 * \code{.c}
 * // Task routine implementation.
 * void task_routine(void *p_param)
 * {
 *     for (;;)
 *     {
 *          // Task code goes here.
 *     }
 * }
 *
 * // Task to be created.
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *     uint32_t task_param;
 *
 *     if (os_task_create(&p_handle, "task", task_routine,
 *                        &task_param, STACK_SIZE, TASK_PRIORITY) == true)
 *     {
 *         // Task created successfully.
 *     }
 *     else
 *     {
 *         // Task failed to create.
 *         return -1;
 *     }
 *
 *     // Use the handle to delete the task.
 *     os_task_delete(p_handle);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Task
 */
bool os_task_create(void **pp_handle, const char *p_name, void (*p_routine)(void *),
                    void *p_param, uint16_t stack_size, uint16_t priority);

/**
 * os_task.h
 *
 * \brief Remove a task from RTOS's task management. The task being deleted will be removed
 * from RUNNING, READY or WAITING state.
 *
 * \param[in] p_handle  The handle of the task to be deleted.
 *
 * \return           The status of the task deletion.
 * \retval true      Task was deleted successfully.
 * \retval false     Task was failed to delete.
 *
 * <b>Example usage</b>
 * \code{.c}
 * // Task routine implementation.
 * void task_routine(void *p_param)
 * {
 *     for (;;)
 *     {
 *          // Task code goes here.
 *     }
 * }
 *
 * // Task to be created and deleted.
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *     uint32_t task_param;
 *
 *     if (os_task_create(&p_handle, "task", task_routine,
 *                        &task_param, STACK_SIZE, TASK_PRIORITY) == true)
 *     {
 *         // Task created successfully.
 *     }
 *     else
 *     {
 *         // Task failed to create.
 *         return -1;
 *     }
 *
 *     // Use the handle to delete the task.
 *     os_task_delete(p_handle);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Task
 */
bool os_task_delete(void *p_handle);

/**
 * os_task.h
 *
 * \brief Suspend the task. The suspended task will not be scheduled and never get
 * any microcontroller processing time.
 *
 * \param[in] p_handle  The handle of the task to be suspended.
 *
 * \return           The status of the task suspension.
 * \retval true      Task was suspended successfully.
 * \retval false     Task was failed to suspend.
 *
 * <b>Example usage</b>
 * \code{.c}
 * // Task routine implementation.
 * void task_routine(void *p_param)
 * {
 *     for (;;)
 *     {
 *          // Task code goes here.
 *     }
 * }
 *
 * // Task to be created and suspended.
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *     uint32_t task_param;
 *
 *     if (os_task_create(&p_handle, "task", task_routine,
 *                        &task_param, STACK_SIZE, TASK_PRIORITY) == true)
 *     {
 *         // Task created successfully.
 *     }
 *     else
 *     {
 *         // Task failed to create.
 *         return -1;
 *     }
 *
 *     // Use the handle to suspend the created task.
 *     os_task_suspend(p_handle);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Task
 */
bool os_task_suspend(void *p_handle);

/**
 * os_task.h
 *
 * \brief Resume the suspended task.
 *
 * \param[in] p_handle  The handle of the task to be resumed.
 *
 * \return           The status of the task resume.
 * \retval true      Task was resumed successfully.
 * \retval false     Task was failed to resume.
 *
 * <b>Example usage</b>
 * \code{.c}
 * // Task routine implementation.
 * void task_routine(void *p_param)
 * {
 *     for (;;)
 *     {
 *          // Task code goes here.
 *     }
 * }
 *
 * // Task to be suspended and resumed.
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *     uint32_t task_param;
 *
 *     if (os_task_create(&p_handle, "task", task_routine,
 *                        &task_param, STACK_SIZE, TASK_PRIORITY) == true)
 *     {
 *         // Task created successfully.
 *     }
 *     else
 *     {
 *         // Task failed to create.
 *         return -1;
 *     }
 *
 *     // Use the handle to suspend the created task.
 *     os_task_suspend(p_handle);
 *
 *     // Resume the suspended task by ourselves.
 *     os_task_resume(p_handle);
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Task
 */
bool os_task_resume(void *p_handle);

/**
 * os_task.h
 *
 * \brief Force a context swith and pass control to the next task that is in
 * READY state.
 *
 * \param None
 *
 * \return           The status of the task resume.
 * \retval true      Task was yielded successfully.
 * \retval false     Task was failed to yield.
 *
 * <b>Example usage</b>
 * \code{.c}
 * // Task routine implementation.
 * void task_routine(void *p_param)
 * {
 *     for (;;)
 *     {
 *          // Force a context switch
 *          os_task_yield();
 *     }
 * }
 *
 * // Task to be created.
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *     uint32_t task_param;
 *
 *     if (os_task_create(&p_handle, "task", task_routine,
 *                        &task_param, STACK_SIZE, TASK_PRIORITY) == true)
 *     {
 *         // Task created successfully.
 *     }
 *     else
 *     {
 *         // Task failed to create.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Task
 */
bool os_task_yield(void);

/**
 * os_task.h
 *
 * \brief Get the handle of the current running task.
 *
 * \param[out] pp_handle  Used to pass back a handle by which the current task
 *                        can be referenced.
 *
 * \return           The status of getting the current task handle.
 * \retval true      Task handle was got successfully.
 * \retval false     Task handle was failed to get.
 *
 * <b>Example usage</b>
 * \code{.c}
 * // Get current task handle.
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *
 *     os_task_handle_get(&p_handle);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Task
 */
bool os_task_handle_get(void **pp_handle);

/**
 * os_task.h
 *
 * \brief Get the priority of the specified task.
 *
 * \param[in] p_handle    The handle of the task to be queried. Passing a NULL handle
 *                        means querying the priority of the current task.
 *
 * \param[out] p_priority Used to pass back the priority of the task.
 *
 * \return           The status of getting the task priority.
 * \retval true      Task priority was got successfully.
 * \retval false     Task priority was failed to get.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *     uint16_t priority;
 *
 *     if (os_task_create(&p_handle, "task", task_routine,
 *                        NULL, STACK_SIZE, TASK_PRIORITY) == true)
 *     {
 *         // Task created successfully.
 *     }
 *     else
 *     {
 *         // Task failed to create.
 *         return -1;
 *     }
 *
 *     // Get the task priority.
 *     os_task_priority_get(p_handle, &priority);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Task
 */
bool os_task_priority_get(void *p_handle, uint16_t *p_priority);

/**
 * os_task.h
 *
 * \brief Set the priority of the specified task.
 *
 * \param[in] p_handle  The handle of the task for which the priority is being set.
 *                      Passing a NULL handle means setting the priority of the
 *                      current task.
 *
 * \param[in] priority  The priority to which the task will be set.
 *
 * \return           The status of setting the task priority.
 * \retval true      Task priority was set successfully.
 * \retval false     Task priority was failed to set.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *     uint16_t priority;
 *
 *     if (os_task_create(&p_handle, "task", task_routine,
 *                        NULL, STACK_SIZE, TASK_PRIORITY) == true)
 *     {
 *         // Task created successfully.
 *     }
 *     else
 *     {
 *         // Task failed to create.
 *         return -1;
 *     }
 *
 *     // Use the handle to raise the created task priority.
 *     os_task_priority_set(p_handle, TASK_PRIORITY + 1);
 *
 *     // Use a NULL handle to raise the current task priority.
 *     os_task_priority_set(NULL, TASK_PRIORITY + 1);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Task
 */
bool os_task_priority_set(void *p_handle, uint16_t priority);

bool os_task_signal_init(void);
void os_task_signal_deinit(void);
/**
 * os_task.h
 *
 * \brief Send a notification signal to the specified task.
 *
 * The notification signal sent to a task will remain pending until it is
 * cleared by the task calling os_task_signal_recv(). If the task was already
 * in the WAITING state to wait for the singal, then the task will be removed
 * from WAITING state and the signal cleared.
 *
 * \param[in] p_handle  The handle of the task to which the signal is sent.
 *
 * \param[in] signal    The signal to be sent.
 *
 * \return          The status of sending the signal.
 * \retval true     Task signal was sent successfully.
 * \retval false    Task signal was failed to send.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *     uint32_t signal;
 *
 *     if (os_task_create(&p_handle, "task", task_routine,
 *                        NULL, STACK_SIZE, TASK_PRIORITY) == true)
 *     {
 *         // Task created successfully.
 *     }
 *     else
 *     {
 *         // Task failed to create.
 *         return -1;
 *     }
 *
 *     // Send signal to the created task.
 *     singal = 1;
 *     os_task_signal_send(p_handle, signal);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Task
 */
bool os_task_signal_send(void *p_handle, uint32_t signal);

/**
 * os_task.h
 *
 * \brief Wait for a notification signal.
 *
 * \param[out] p_signal Used to pass back the received signal.
 *
 * \param[in] wait_ms   The timeout in milliseconds to wait for the signal.
 * \arg \c 0            No blocking and return immediately.
 * \arg \c 0xFFFFFFFF   Block infinitely until the signal received.
 * \arg \c others       The timeout value in milliseconds.
 *
 * \return          The status of receiving the signal.
 * \retval true     Task signal was received successfully.
 * \retval false    Task signal was failed to receive.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     uint32_t signal;
 *
 *     // Block to wait for the signal sent from other tasks.
 *     os_task_signal_recv(&signal, 0xFFFFFFFF);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Task
 */
bool os_task_signal_recv(uint32_t *p_signal, uint32_t wait_ms);

/**
 * os_task.h
 *
 * \brief Clear the signal of the specified task.
 *
 * \param[in] p_handle  The handle of the task to which the signal is clear.
 *
 * \return          The status of clearing the signal.
 * \retval true     Task signal was cleared successfully.
 * \retval false    Task signal was failed to clear.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *     uint32_t signal;
 *
 *     if (os_task_create(&p_handle, "task", task_routine,
 *                        NULL, STACK_SIZE, TASK_PRIORITY) == true)
 *     {
 *         // Task created successfully.
 *     }
 *     else
 *     {
 *         // Task failed to create.
 *         return -1;
 *     }
 *
 *     // Send signal to the created task.
 *     singal = 1;
 *     os_task_signal_send(p_handle, signal);
 *
 *     // Clear signal of the created task.
 *     os_task_signal_clear(p_handle);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Task
 */
bool os_task_signal_clear(void *p_handle);

#ifdef __cplusplus
}
#endif

#endif /* _OS_TASK_H_ */
