# README.md: at

## Contents

```shell
├── at.c
├── at/at.h
├── at.mk
├── hdlc.c
└── hdlc.h
```

## Introduction

`at` is an AT component for sending and receiving AT command over uart.

To use this component:
1. Include at/at.h header file;
2. Declare the at module in makefile;
3. Configure an AT UART;
4. Initialize at (at.init API);
5. at APIS: at_send, at_recv, at_send_raw, etc.
6. Please refer to app/example/atapp for example on how to use at component.


### Features

- **HDLC**.  `at` provides HDLC over uart, including CRC examination, acknowledgement, retransmission.

### Dependencies

None.

## API

See at/at.h

## Reference

None.
