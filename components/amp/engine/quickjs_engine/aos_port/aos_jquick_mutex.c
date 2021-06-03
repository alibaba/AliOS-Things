
#include <stdlib.h>
#include "aos_jquick_mutex.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"

#define MOD_STR "JQUICK_MUTEX"

JQuick_Mutex jquick_mutex_create()
{
    aos_mutex_t mutex;
    if (0 != aos_mutex_new(&mutex)) {
        return NULL;
    }

    return (JQuick_Mutex)mutex;
}

int jquick_mutex_lock(JQuick_Mutex mutex)
{
    if (!mutex) {
        LOGE(MOD_STR, "JQuick_Mutex: Mutex is NULL\n");
        return -1;
    }

    return aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
}

int jquick_mutex_unlock(JQuick_Mutex mutex)
{
    if (!mutex) {
        LOGE(MOD_STR, "JQuick_Mutex: Mutex is NULL\n");
        return -1;
    }

    return aos_mutex_unlock((aos_mutex_t *)&mutex);
}

int jquick_mutex_destroy(JQuick_Mutex mutex)
{
    if (!mutex) {
        LOGE(MOD_STR, "JQuick_Mutex: Mutex is NULL\n");
        return -1;
    }

    aos_mutex_free((aos_mutex_t *)&mutex);

    return 0;
}
