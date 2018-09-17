#include <pthread.h>

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    krhino_mutex_dyn_create(&cond->lock, "mutex");
    krhino_sem_dyn_create(&cond->wait_sem, "sem", 0);
    krhino_sem_dyn_create(&cond->wait_done, "sem", 0);
    cond->waiting = cond->signals = 0;

    if (! cond->lock || ! cond->wait_sem || ! cond->wait_done) {
        cond = 0;
    }

    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    if (cond != NULL) {
        if (cond->wait_sem != NULL) {
            krhino_sem_dyn_del(cond->wait_sem);
        }

        if (cond->wait_done != NULL) {
            krhino_sem_dyn_del(cond->wait_done);
        }
    }

    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    int i, num_waiting;
    /* If there are waiting threads not already signalled, then
    signal the condition and wait for the thread to respond.
    */
    krhino_mutex_lock(cond->lock, RHINO_WAIT_FOREVER);

    if (cond->waiting > cond->signals) {
        num_waiting = (cond->waiting - cond->signals);
        cond->signals = cond->waiting;

        for (i = 0; i < num_waiting; ++i) {
            krhino_sem_give(cond->wait_sem);
        }
        /* Now all released threads are blocked here, waiting for us.
        Collect them all (and win fabulous prizes!) :-)
        */
        krhino_mutex_unlock(cond->lock);
        for (i = 0; i < num_waiting; ++i) {
            krhino_sem_take(cond->wait_done, RHINO_WAIT_FOREVER);
        }
    } else {
        krhino_mutex_unlock(cond->lock);
    }

    return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    /* If there are waiting threads not already signalled, then
    signal the condition and wait for the thread to respond.
    */
    krhino_mutex_lock(cond->lock, RHINO_WAIT_FOREVER);
    if ( cond->waiting > cond->signals ) {
        ++cond->signals;
        krhino_sem_give(cond->wait_sem);
        krhino_mutex_unlock(cond->lock);
        krhino_sem_take(cond->wait_done, RHINO_WAIT_FOREVER);
    } else {
        krhino_mutex_unlock(cond->lock);
    }

    return 0;
}

int pthread_cond_timedwait(pthread_cond_t        *cond,
                           pthread_mutex_t       *mutex,
                           const struct timespec *abstime)
{
    kstat_t retval;
    tick_t  ticks;
    struct timeval now;

    gettimeofday(&now, NULL);

    ticks = (abstime->tv_sec - now.tv_sec) * RHINO_CONFIG_TICKS_PER_SECOND +
            ((abstime->tv_nsec - now.tv_usec) / 1000000) / (1000 / RHINO_CONFIG_TICKS_PER_SECOND);

    /* Obtain the protection mutex, and increment the number of waiters.
       This allows the signal mechanism to only perform a signal if there
       are waiting threads.
     */
    krhino_mutex_lock(cond->lock, RHINO_WAIT_FOREVER);
    ++cond->waiting;
    krhino_mutex_unlock(cond->lock);

    /* Unlock the mutex, as is required by condition variable semantics */
    krhino_mutex_unlock(mutex->mutex);

    /* Wait for a signal */
    retval = krhino_sem_take(cond->wait_sem, ticks);

    /* Let the signaler know we have completed the wait, otherwise
           the signaler can race ahead and get the condition semaphore
           if we are stopped between the mutex unlock and semaphore wait,
           giving a deadlock.  See the following URL for details:
        http://www-classic.be.com/aboutbe/benewsletter/volume_III/Issue40.html
    */
    krhino_mutex_lock(cond->lock, RHINO_WAIT_FOREVER);
    if (cond->signals > 0) {
        /* If we timed out, we need to eat a condition signal */
        if (retval == RHINO_BLK_TIMEOUT) {
            krhino_sem_take(cond->wait_sem, RHINO_WAIT_FOREVER);
        }
        /* We always notify the signal thread that we are done */
        krhino_sem_give(cond->wait_done);

        /* Signal handshake complete */
        --cond->signals;
    }
    --cond->waiting;
    krhino_mutex_unlock(cond->lock);

    /* Lock the mutex, as is required by condition variable semantics */
    krhino_mutex_lock(mutex->mutex, RHINO_WAIT_FOREVER);

    return 0;
}


int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{

    kstat_t retval;

    /* Obtain the protection mutex, and increment the number of waiters.
       This allows the signal mechanism to only perform a signal if there
       are waiting threads.
     */
    krhino_mutex_lock(cond->lock, RHINO_WAIT_FOREVER);
    ++cond->waiting;
    krhino_mutex_unlock(cond->lock);

    /* Unlock the mutex, as is required by condition variable semantics */
    krhino_mutex_unlock(mutex->mutex);

    /* Wait for a signal */
    retval = krhino_sem_take(cond->wait_sem, RHINO_WAIT_FOREVER);

    /* Let the signaler know we have completed the wait, otherwise
           the signaler can race ahead and get the condition semaphore
           if we are stopped between the mutex unlock and semaphore wait,
           giving a deadlock.  See the following URL for details:
        http://www-classic.be.com/aboutbe/benewsletter/volume_III/Issue40.html
    */
    krhino_mutex_lock(cond->lock, RHINO_WAIT_FOREVER);
    if (cond->signals > 0) {
        /* If we timed out, we need to eat a condition signal */
        if (retval == RHINO_BLK_TIMEOUT) {
            krhino_sem_take(cond->wait_sem, RHINO_WAIT_FOREVER);
        }
        /* We always notify the signal thread that we are done */
        krhino_sem_give(cond->wait_done);

        /* Signal handshake complete */
        --cond->signals;
    }
    --cond->waiting;
    krhino_mutex_unlock(cond->lock);

    /* Lock the mutex, as is required by condition variable semantics */
    krhino_mutex_lock(mutex->mutex, RHINO_WAIT_FOREVER);

    return 0;
}

int pthread_condattr_init(pthread_condattr_t *attr)
{
    return 0;
}

int pthread_condattr_destroy(pthread_condattr_t *attr)
{
    return 0;
}
