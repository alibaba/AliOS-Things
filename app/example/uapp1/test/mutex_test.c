#include <k_api.h>
#include <hal/hal.h>


int mutex_test(void)
{
    kmutex_t mutex;

    krhino_mutex_create(&mutex, "test_mutex");

    krhino_mutex_lock(&mutex, 0xff);

    printf("%s: get mutex\r\n", __func__);

    krhino_mutex_unlock(&mutex);

    printf("%s: release mutex\r\n", __func__);

    return 0;

}
