/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __SL_WIFI_PROV_H__
#define __SL_WIFI_PROV_H__
#if defined(__cplusplus)
extern "C" {
#endif

/** register smartliving wifi provisioning method, include both smartconfig and device ap
  * @return 0 success, else failed
 */
int wifi_prov_sl_register();

/** report provisioning success result to cloud, do device biding with cellphone
  * @param report 1 for reporting; 0 for not
 */
void sl_report_to_cloud(int report);

void test_report_to_cloud(void);
#if defined(__cplusplus)
}
#endif
#endif  /* __SL_WIFI_PROV_H__ */

