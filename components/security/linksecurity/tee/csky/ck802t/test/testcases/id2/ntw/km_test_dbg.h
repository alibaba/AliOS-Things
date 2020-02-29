#include <stdio.h>

#define _DEPRESS_UNUSED_WARNING(_x) \
    do {                            \
        (_x) = (_x);                \
    } while (0)

#define KM_TEST_ERR(_f, _a...) \
    printk("KM TEST ERR %s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#define KM_TEST_INF(_f, _a...) printk("KM TEST INF "_f, ##_a)

#define km_malloc malloc
#define km_free free
#define km_memcpy memcpy


#ifdef KM_TEST_DEBUG
void km_test_dump_data(char *name, uint8_t *data, uint32_t len);
#endif /* CONFIG_KM_TEST_DEBUG */
