# id2 sample
  
It is a sample for using id2 in alios things. 
## Contents

```sh
id2_app
└── id2_client_demo.c        # id2 test demo code
```

## Introduction

This example is an local test code for id2.

### Requirements

If "ID2 OTP" is supported, need to enable "ID2 OTP LOCAL TEST" macro first.

### Features

* ID2 test

### Dependencies

* cli
* alicrypto
* irot

### Supported Boards

- mk3060
- mk3080
- linuxhost

### Result
```
 ...

 ID2=> output secret [length = 0x0020]
 ID2=> 9E 44 17 23 5C EA F7 5E   E6 DB 0C 0B 37 71 0F 37 
 ID2=> EF B0 FB DC F0 B8 39 67   21 8C AA 5F AA FF 91 30 
 ID2=> id2_client_get_secret success
 ID2=> [id2_client_cleanup] enter.
 ID2=> [id2_client_cleanup] exit.
 ID2=> <========================== id2 client test end.

```

