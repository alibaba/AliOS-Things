## Contents

```shell
.
├── aos.mk
├── awss_bind.c
├── awss_cmp_coap.c
├── awss_cmp.h
├── awss_cmp_mqtt.c
├── awss_event.c
├── awss_event.h
├── awss_log.h
├── awss_notify.c
├── awss_notify.h
├── awss_packet.c
├── awss_packet.h
├── awss_report.c
├── awss_timer.c
├── awss_timer.h
├── awss_utils.h
├── CMakeLists.txt
├── Config.in
├── iot.mk
├── os
│   ├── os.h
│   ├── os_misc.c
│   ├── platform.h
│   └── product
│       └── product.h
├── passwd.c
├── passwd.h
├── README.md

```

## Introduction
device bind service,provides the ability to bind user accounts.


### Features



### Dependencies
- **hal**. osal and hal to shield different os and hardware
- **infra**. Authentication, net and so on tool set.
- **mqtt**. get/request bind message with server

## API
None.

## Reference
Please refer to  [awss user manual](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/WiFi_Provision) for describe details.


