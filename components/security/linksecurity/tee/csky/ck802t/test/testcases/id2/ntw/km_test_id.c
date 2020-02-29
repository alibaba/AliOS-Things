#include "km.h"
#include "km_test_comm.h"
#include "km_test_dbg.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

uint32_t test_get_id2()
{
    uint32_t ret;
    uint8_t  id_value[25]; //'\0'
    uint32_t id2_len = 0;

    ret = km_get_id2(NULL, &id2_len);
    if (ret != KM_ERR_SHORT_BUFFER || id2_len != 24) {
        KM_TEST_ERR("fail to get ID len 0x%x!\n", ret);
        return ret;
    }

    ret = km_get_id2(id_value, &id2_len);
    if (ret || id2_len != 24) {
        KM_TEST_ERR("fail to get ID!\n");
        return -1;
    }
    id_value[24] = '\0';
    KM_TEST_INF("id2_len is %d, ID2 ID: %s\n", id2_len, id_value);

    return ret;
}