# README.md: atparser

## Contents

```shell
├── atparser.c
├── atparser.h
├── atparser.mk
├── hdlc.c
└── hdlc.h
```

## Introduction

`atparser` is an AT component for sending and receiving AT command over uart.

To use this component:
1. Include atparser.h header file;
2. Declare the atparser module in makefile;
3. Configure an AT UART;
4. Initialize atparser (at.init API);
5. atparser APIS: at_send, at_recv, at_send_raw, etc.
6. Please refer to app/example/atapp for example on how to use atparser component.


### Features

- **HDLC**.  `atparser` provides HDLC over uart, including CRC examination, acknowledgement, retransmission.

### Dependencies

None.

## API

See atparser.h

## Reference

None.
