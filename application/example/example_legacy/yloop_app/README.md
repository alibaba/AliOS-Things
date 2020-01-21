## Contents

```shell
├── yloop_app.c
├── Config.in
└── aos.mk
```

## Introduction

`yloop_app` is an demostration for how to use yloop API to perform periodic action without 'while'. Specifcially, this example sends/reads UDP packets to/from loopback address in one loop. 


Compile command example:

```sh
# generate yloop_app@mk3080 default config
aos make yloop_app@mk3080 -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

### Features

- **Delay action**.  `yloop` provides API to schedule a delayed action. In this example, it allows to send data when timer fires.
- **Poll fd**.  `yloop` provides API to register a poll event on fd. In this example, it allows to read data on data arrival. 
- **Loop**. `yloop` provides API to hide 'while' and changes programming scheme. In this example, it allows to keep program running and perform above actions in time.

### Dependencies

- network/yloop

### Supported Boards

- linuxhost, mk3080 

## Reference

None.
