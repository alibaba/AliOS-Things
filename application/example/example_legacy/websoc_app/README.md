## Contents

```shell
├── websoc_app.c
├── Config.in
└── aos.mk
```

## Introduction

`websoc_app` is an demostration for how to use websocket API to interact with remote server. Specifcially, this example set up multiple websocket instances for echo. 


Compile command example:

```sh
# generate websoc_app@mk3080 default config
aos make websoc_app@mk3080 -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

### Features

- **websocket parameter**.  `websocket` provides API to configure connection, such as scheme, host, port.
- **websocket operation**.  `websocket` provides asynchronized API to send text/bin/ping data to remote and use callbacks to inform upperlayer.
- **websocket instance**. `websocket` supports thread-safe multiple instances.

### Dependencies

- network/websocket

### Supported Boards

- mk3080 

## Reference

None.
