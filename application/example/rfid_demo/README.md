# RFID demo

## Contents

```sh
├── aos.mk
├── app_entry.c
├── app_entry.h
├── autobuild.json
├── Config.in
├── k_app_config.h
├── linkkit_event.c
├── maintask.c
├── README.md
├── rfid_app.c
└── rfid_app.h
```

## Introduction

The is a rfid demo to read rfid card number.

### Dependencies

* linkkit

### Supported Boards

- haas100

### Build

```sh
aos make rfid_demo@haas100 -c config

aos make
```

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)
* [gateway](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides)
