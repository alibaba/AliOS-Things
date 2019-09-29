## Contents

## Introduction

The **sk_gui** is a test gui local example for starterkit board, also it will show differnt sensor raw data.

### Features

- GUI disaply
- show sensor raw data

### Directories

```sh
starterkit/sk_gui
	├── AliOS_Things_logo.c
	├── Config.in
	├── k_app_config.h
	├── littlevgl_starterkit.c
	├── lv_conf_starterkit.h
	├── README.md
	└── sk_gui.mk
```

### Supported Boards

- starterkit

### Buid and Flash

```sh
# generate sk.sk_gui@starterkit default config
aos make sk.sk_gui@starterkit -c config

# or customize config manually
aos make menuconfig

# build
aos make

#upload
aos upload
```

## Reference