## Contents

```shell
.
├── aos.mk
├── aws_lib.h
├── awss_adha.c
├── awss_adha.h
├── awss_aha.c
├── awss_aha.h
├── awss_aplist.c
├── awss_aplist.h
├── awss.c
├── awss_crypt.c
├── awss_crypt.h
├── awss_enrollee.c
├── awss_enrollee.h
├── awss.h
├── awss_ht40.c
├── awss_ht40.h
├── awss_info.c
├── awss_info.h
├── awss_main.c
├── awss_main.h
├── awss_registrar.c
├── awss_smartconfig.c
├── awss_smartconfig.h
├── awss_wifimgr.c
├── awss_wifimgr.h
├── awss_wps.c
├── awss_wps.h
├── CMakeLists.txt
├── Config.in
├── iot.mk
├── README.md
├── zconfig_ieee80211.c
├── zconfig_ieee80211.h
├── zconfig_lib.h
├── zconfig_protocol.c
├── zconfig_protocol.h
├── zconfig_utils.c
├── zconfig_utils.h
└── zconfig_vendor_common.c


```

## Introduction
Alibaba Wireless Setup service.


### Features
 - support smart config
 - support phone asap config
 - support router config
 - support zero config 


### Dependencies
- **hal**. osal and hal to shield different os and hardware
- **infra**. Authentication, net and so on tool set.
- **coap local**. for local communication

## API
None.

## Reference
Please refer to  [awss user manual](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/WiFi_Provision) for describe details.


