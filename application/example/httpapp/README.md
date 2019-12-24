## Contents

```shell
├── http_config.h
├── httpapp.c
└── aos.mk
```

## Introduction

`httpapp` is an example for doing http communication. It provides HTTP POST/GET/PUT/DELETE method to do communication, etc.

Compile command example:

```sh
# generate httpapp@mk3080 default config
aos make httpapp@mk3080 -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

### Features

### Dependencies

- network/http

### Supported Boards

- mk3060

## Reference

None.
