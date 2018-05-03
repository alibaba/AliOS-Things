#ifndef _MAC_CFG_
#define _MAC_CFG_

void *wifi_cfg_init();
void wifi_cfg_get_addr1(const void *handle, char addr[6]);
void wifi_cfg_get_addr2(const void *handle, char addr[6]);
void wifi_cfg_replace_mem_addr1(const void *handle, char *str);
void wifi_cfg_replace_mem_addr2(const void *handle, char *str);
void wifi_cfg_write_cfg(const void *handle);
void wifi_cfg_deinit(void *handle);
#endif /* end of include guard */
