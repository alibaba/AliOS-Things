/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AMP_UTILS_H
#define AMP_UTILS_H

#if defined(__cplusplus)
extern "C" {
#endif

#define AOS_VERSION_LENGTH 32
typedef struct {
    char userjs[AOS_VERSION_LENGTH];
    char app[AOS_VERSION_LENGTH];
    char kernel[AOS_VERSION_LENGTH];
    char system[AOS_VERSION_LENGTH * 2];
    char module_hardware[AOS_VERSION_LENGTH];
    char product_hardware[AOS_VERSION_LENGTH];
    char build_time[AOS_VERSION_LENGTH];
} AOS_SYSTEM_VERSION;

unsigned char hex2num(unsigned char ch);
char itoch(int val);
void num2hex(unsigned char ch, unsigned char *hex);
int end_with(char *str1, char *str2);
void amp_dump(const char *title, const void *buff, const int len);
void aos_userjs_version_set(char *version);
const char *aos_userjs_version_get(void);
const char *aos_app_version_get(void);
const char *aos_system_version_get(void);
const char *aos_system_info_get(void);
const char *aos_kernel_version_get(void);
const char *aos_system_build_time(void);
const char *aos_hardware_version_get(void);

#ifdef SUPPORT_NODE_MODELES
char *getFilePath(char *name);
char *getClearPath(char *path);
char *getNodeModulePath(char *path);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* JSE_UTILS_H */
