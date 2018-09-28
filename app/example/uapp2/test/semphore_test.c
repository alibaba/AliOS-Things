#include <k_api.h>
#include <hal/hal.h>

int semphore_test(void)
{
    int ret = 0;

    ksem_t sem;

    krhino_sem_create(&sem, "test_sem", 1);

    krhino_sem_take(&sem, RHINO_WAIT_FOREVER);

    if (sem.count == 0)
        ret++;

    krhino_sem_give(&sem);

    if (sem.count == 1)
        ret++;

    krhino_sem_del(&sem);

    return (ret == 2)? 0: -1;
}
