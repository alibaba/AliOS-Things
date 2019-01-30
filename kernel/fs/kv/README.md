## Contents

## Introduction
**kv** is a lightweight persistent stroage module which is based on key-value style. It provides convenient interface for MCU (Micro Control Unit). The developers can achieve more efficient and common application development based on flash memory. The storage module support wear-leveling and power fail safeguard, also have a low footprint.

### Features
- key-value pair set
- key-value pait get
- key-value pair delete
- key-value pair delete for group (same prefix)

### Directories

```sh
kv
├── include
│   ├── kv_adapt.h # kv OS adapt layer declaration
│   ├── kv_api.h   # kv module interface declaration
│   ├── kv_conf.h  # kv configuration
│   └── kv_err.h   # kv errno defination
├── kv_adapt.c     # kv os adapt layer implementation
├── kv.c           # kv core logic
├── kv_cli.c       # kv CLI command
├── kv_types.h     # kv internal data structure defination
├── kv_secure.c    # kv encrypt/decrypt implementation
```

### Dependencies

### Notice
If users want to enable the encrypt/decrypt feature (mbedtls version) for kv. Please implement the **kv_secure_get_key** and **kv_secure_get_iv** function. (declare in `kv_adapt.h`)

Here is a example:
```C

uint8_t aes_key[32] = {
        0x86, 0xf6, 0xd2, 0xbe, 0x45, 0xb5, 0xab, 0x9c,
        0xc7, 0xd5, 0x96, 0xf7, 0xaf, 0x45, 0xfa, 0xf7,
        0xbe, 0x6a, 0x5d, 0xb0, 0x04, 0xc4, 0xde, 0xb5,
        0xf5, 0x0c, 0x4f, 0xc3, 0x71, 0x19, 0x3e, 0xe8
    };

uint8_t aes_iv[16]  = {
        0xef, 0x80, 0x18, 0xdc, 0xa3, 0x72, 0x72, 0x31,
        0x99, 0x2e, 0x3a, 0xba, 0x60, 0xf5, 0x0b, 0xd4
    };

uint8_t* kv_secure_get_key(uint32_t len)
{
    if ((len <= 0) || (len > sizeof(aes_key))) {
        return NULL;
    }

    return aes_key;
}

uint8_t* kv_secure_get_iv(uint32_t len)
{
    if ((len <= 0) || (len > sizeof(aes_iv))) {
        return NULL;
    }

    return aes_iv;
}

```

## Reference
