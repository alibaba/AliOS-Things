#ifndef __GENIE_VERSION_H__
#define __GENIE_VERSION_H__

uint32_t genie_version_appver_get(void);

const char *genie_version_sysver_get(void);

int genie_version_appver_string_get(char *version, int len);

#endif
