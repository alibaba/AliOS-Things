/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 */

#include "ali_crypto_test.h"

void ali_crypto_print_data(const char *name, uint8_t *data, size_t size)
{
     size_t i;

     if (data == NULL || size == 0) {
          CRYPT_ERR("print_data: no data\n");
          return;
     }

    CRYPT_INF("%s size: %d\n", name, (int)size);
    for (i = 0; i < size - size % 8; i += 8) {
        CRYPT_INF("%s data: %02x%02x %02x%02x %02x%02x %02x%02x\n",
                   name,
                   data[i+0], data[i+1], data[i+2], data[i+3],
                   data[i+4], data[i+5], data[i+6], data[i+7]);
    }
    while(i < size) {
        CRYPT_INF("%s data: %02x\n", name, data[i]);
        i++;
    }

    return;
}
