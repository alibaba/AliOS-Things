/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
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
