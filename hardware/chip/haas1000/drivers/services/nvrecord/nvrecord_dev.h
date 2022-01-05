/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef NVRECORD_DEV_H
#define NVRECORD_DEV_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    unsigned char *btd_addr;
    unsigned char *ble_addr;
    const char *localname;
	const char *ble_name;
}dev_addr_name;

bool nvrec_dev_data_open(void);
bool nvrec_dev_localname_addr_init(dev_addr_name *dev);
int nvrec_dev_force_get_btaddress(unsigned char *btd_addr);
void nvrec_dev_flash_flush(unsigned char *mempool);
void nvrec_dev_rand_btaddr_gen(uint8_t* bdaddr);
void nvrec_dev_set_xtal_fcap(unsigned int xtal_fcap);
int nvrec_dev_get_xtal_fcap(unsigned int *xtal_fcap);

#ifdef __cplusplus
}
#endif
#endif
