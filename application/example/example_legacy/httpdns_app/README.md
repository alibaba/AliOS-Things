## Contents

```shell
├── httpdns_app.c
├── Config.in
├── aos.mk
└── README.md
```
```

## Introduction

`httpdns_app` is an demostration for how to use httpdns API to request DNS service through HTTP. 


Compile command example:

```sh
# generate httpdns_app@mk3080 default config
aos make httpdns_app@mk3080 -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

### Features

- **DNS entry cache**.  `httpdns` provides DNS entry cache to fetch entry locally.
- **DNS operation**.  `httpdns` provides asynchronized API to fetch enrty from remote.
- **Configurable**.  `httpdns` provides configuration to set Aliyun HTTPDNS user ID and DNS entry expiration time.

### Dependencies

- network/httpdns

### Supported Boards

- mk3080 

## Reference

None.
