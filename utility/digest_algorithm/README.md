## Contents

* [Directory](#directory)
* [Introduction](#introduction)
    * [Features](#features)
    * [Dependencies](#dependencies)
* [API](#api)
    * [CheckSumUtils.c](#checksumutilsc)
        * [CRC8_Init()](#crc8_init)
        * [CRC8_Update()](#crc8_update)
        * [CRC8_Final()](#crc8_final)
        * [CRC16_Init()](#crc16_init)
        * [CRC16_Update()](#crc16_update)
        * [CRC16_Final()](#crc16_final)
    * [crc.c](#crcc)
        * [utils_crc16()](#utils_crc16)
        * [utils_crc32()](#utils_crc32)
    * [crc16.c](#crc16c)
        * [checksum_crc16()](#checksum_crc16)
    * [digest_algorithm.c](#digest_algorithmc)
        * [digest_md5_init()](#digest_md5_init)
        * [digest_md5_update()](#digest_md5_update)
        * [digest_md5_final()](#digest_md5_final)
        * [digest_md5()](#digest_md5)
        * [digest_md5_file()](#digest_md5_file)
        * [digest_sha256_init()](#digest_sha256_init)
        * [digest_sha256_update()](#digest_sha256_update)
        * [digest_sha256_final()](#digest_sha256_final)
        * [digest_sha256()](#digest_sha256)
        * [digest_sha384_init()](#digest_sha384_init)
        * [digest_sha384_update()](#digest_sha384_update)
        * [digest_sha384_final()](#digest_sha384_final)
        * [digest_sha384()](#digest_sha384)
        * [digest_sha512_init()](#digest_sha512_init)
        * [digest_sha512_update()](#digest_sha512_update)
        * [digest_sha512_final()](#digest_sha512_final)
        * [digest_sha512()](#digest_sha512)
        * [digest_hmac()](#digest_hmac)
    * [Sample Code](#sample-code)
        * [CRC8 sample code](#crc8-sample-code)
        * [CRC16 sample code](#crc16-sample-code)
        * [md5 sample code](#md5-sample-code)
        * [sha256 sample code](#sha256-sample-code)

## Directory

```sh
digest_algorithm/
├── CheckSumUtils.c         # crc & crc16 checksum algorithm
├── crc.c                   # crc16 & crc32 checksum algorithm
├── crc16.c                 # crc16 checksum algorithm
├── digest_algorithm.c      # digest algorithm, including md5 hmac sha256 sha384 sha512
├── hmac.c                  # hmac digest algorithm
├── md5.c                   # md5 digest algorithm
└── sha2.c                  # sha256 sha384 sha512 digest algorithm
```

## Introduction

**digest_algorithm** provides many checksum and digest algorithm with the friendly APIs.

### Features

* crc algorithm
    * crc
    * crc16
    * crc32
* message-digest algorithm
    * hmac
    * md5
    * sha256
    * sha384
    * sha512

### Dependencies

*(None)*

## APIs

### CheckSumUtils.c

#### CRC8_Init()

```c
void CRC8_Init( CRC8_Context *inContext );
```

CRC8_Context struct:

```c
typedef struct {
    uint8_t crc;
} CRC8_Context;
```

#### CRC8_Update()

Update data into crc context.

```c
void CRC8_Update( CRC8_Context *inContext, const void *inSrc, size_t inLen );
```

**arguments**:

| name        | type            |description     |
|:------------|:----------------|----------------|
| `inContext` | `CRC8_Context *`| crc context, get by [CRC8_Init()](#crc8_Init)    |
| `inSrc`     | `const void *`  | input data     |
| `inLen`     | `size_t`        | input data len |

#### CRC8_Final()

Get the final crc checksum.

```c
void CRC8_Final( CRC8_Context *inContext, uint8_t *outResult );
```

**arguments**:

| name        | type            |description         |
|:------------|:----------------|--------------------|
| `inContext` | `CRC8_Context *`| crc context, get by [CRC8_Init()](#crc8_init)    |
| `outResult` | `uint8_t *`     | final crc checksum |
| `inLen`     | `size_t`        | input data len     |

#### CRC16_Init()

```c
void CRC16_Init( CRC16_Context *inContext );
```

CRC16_Context struct:

```c
typedef struct {
    uint16_t crc;
} CRC16_Context;
```

#### CRC16_Update()

update data into crc context.

```c
void CRC16_Update( CRC16_Context *inContext, const void *inSrc, size_t inLen );
```

**arguments**:

| name        | type            |description     |
|:------------|:----------------|----------------|
| `inContext` | `CRC16_Context *`| crc context, get by [CRC16_Init()](#crc16_init)    |
| `inSrc`     | `const void *`  | input data     |
| `inLen`     | `size_t`        | input data len |

#### CRC16_Final()

get the final crc checksum.

```c
void CRC16_Final( CRC16_Context *inContext, uint16_t *outResult );
```

**arguments**:

| name        | type              |description         |
|:------------|:------------------|--------------------|
| `inContext` | `CRC16_Context *` | crc context, get by [CRC16_Init()](#crc16_init)    |
| `outResult` | `uint16_t *`      | final crc checksum |
| `inLen`     | `size_t`          | input data len     |

### crc.c

#### utils_crc16()

```c
uint16_t utils_crc16(uint8_t *buf, uint32_t length);
```

> The poly is 0x1021

**arguments**:

| name     | type       |description     |
|:---------|:-----------|----------------|
| `buf`    | `uint8_t *`| input data     |
| `length` | `uint32_t` | input data len |

**return**:

|return      |description |
|:-----------| -----------|
| `uint16_t` | checksum   |

#### utils_crc32()

```c
uint32_t utils_crc32(uint8_t *buf, uint32_t length);
```

> The poly is 0x04C11DB7

**arguments**:

| name     | type       |description     |
|:---------|:-----------|----------------|
| `buf`    | `uint8_t *`| input data     |
| `length` | `uint32_t` | input data len |

**return**:

|return      |description |
|:-----------| -----------|
| `uint32_t` | checksum   |


### crc16.c

#### checksum_crc16()

```c
uint16_t checksum_crc16(const uint8_t *data, uint16_t size);
```

**arguments**:

| name   | type             |description     |
|:-------|:-----------------|----------------|
| `data` | `const uint8_t *`| input data     |
| `size` | `uint16_t`       | input data len |

**return**:

|return      |description |
|:-----------| -----------|
| `uint16_t` | checksum   |


### digest_algorithm.c

#### digest_md5_init()

```c
void *digest_md5_init(void);
```

**return**:

|return    |description  |
|:---------| ------------|
| `void *` | md5 handler |

#### digest_md5_update()

```c
int digest_md5_update(void *md5, const void *data, uint32_t length);
```

**arguments**:

| name     | type       |description     |
|:---------|:-----------|----------------|
| `md5`    | `void *`   | md5 handler, get from [digest_md5_init()](#digest_md5_init) |
| `data`   | `void *`   | input data     |
| `length` | `uint32_t` | input data len |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |

#### digest_md5_final()

```c
int digest_md5_final(void *md5, unsigned char *digest);
```

**arguments**:

| name     | type              |description     |
|:---------|:------------------|----------------|
| `md5`    | `void *`          | md5 handler, get from [digest_md5_init()](#digest_md5_init) |
| `digest` | `unsigned char *` | md5 result     |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |


#### digest_md5()

```c
int digest_md5(const void *data, uint32_t length, unsigned char *digest);
```

**arguments**:

| name     | type              |description        |
|:---------|:------------------|-------------------|
| `data`   | `const void *`    | input data        |
| `length` | `uint32_t`        | input data length |
| `digest` | `unsigned char *` | md5 result        |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |

the `digest_md5()` result can be check by linux command: 

```sh
$ echo "AliOS-Things" | md5sum
8e17c9300a66136762f54bf2f0aa9778  -
```

#### digest_md5_file()

```c
int digest_md5_file(const char *path, unsigned char *digest);
```

**arguments**:

| name     | type              |description        |
|:---------|:------------------|-------------------|
| `path`   | `const char *`    | file path         |
| `digest` | `unsigned char *` | md5 result        |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |


the `digest_md5_file()` result can be check by linux command: 
```sh
$ md5sum file
3a5f11431200fd4a4218e4d90e2adad6  file
```

#### digest_sha256_init()

```c
void *digest_sha256_init(void);
```

**return**:

|return    |description     |
|:---------| ---------------|
| `void *` | sha256 handler |

#### digest_sha256_update()

```c
int digest_sha256_update(void *sha256, const void *data, uint32_t length);
```

**arguments**:

| name     | type       |description     |
|:---------|:-----------|----------------|
| `sha256` | `void *`   | sha256 handler, get from [digest_sha256_init()](#digest_sha256_init) |
| `data`   | `void *`   | input data     |
| `length` | `uint32_t` | input data len |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |


#### digest_sha256_final()

```c
int digest_sha256_final(void *sha256, unsigned char *digest);
```

**arguments**:

| name     | type              |description     |
|:---------|:------------------|----------------|
| `sha256` | `void *`          | sha256 handler, get from [digest_sha256_init()](#digest_sha256_init) |
| `digest` | `unsigned char *` | sha256 result     |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |


#### digest_sha256()

```c
int digest_sha256(const void *data, uint32_t length, unsigned char *digest);
```

**arguments**:

| name     | type              |description        |
|:---------|:------------------|-------------------|
| `data`   | `const void *`    | input data        |
| `length` | `uint32_t`        | input data length |
| `digest` | `unsigned char *` | sha256 result     |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |

the `digest_sha256()` result can be check by linux command: 

```sh
$ echo "AliOS-Things" | sha256sum
5213c1b0564da1884bc8ad0c267b447abf41ac9f11c3e46400891c6b3125ec85  -
```




#### digest_sha384_init()

same with [digest_sha256_init()](#digest_sha256_init)

#### digest_sha384_update()

same with [digest_sha256_update()](#digest_sha256_update)

#### digest_sha384_final()

same with [digest_sha256_final()](#digest_sha256_final)

#### digest_sha384()

same with [digest_sha256()](#digest_sha256)

#### digest_sha512_init()

same with [digest_sha256_init()](#digest_sha256_init)

#### digest_sha512_update()

same with [digest_sha256_update()](#digest_sha256_update)

#### digest_sha512_final()

same with [digest_sha256_final()](#digest_sha256_final)

#### digest_sha512()

same with [digest_sha256()](#digest_sha256)

#### digest_hmac()


```c
int digest_hmac(enum digest_type type,
                const unsigned char *msg, uint32_t msg_len,
                const unsigned char *key, uint32_t key_len,
                unsigned char *digest);
```

`enum digest_type` defined:
```c
enum digest_type {
    DIGEST_TYPE_MD5,                /**< MD5 */
    //CHF_SHA1,
    DIGEST_TYPE_SHA256,             /**< SHA-256 */
    DIGEST_TYPE_SHA384,             /**< SHA-384 */
    DIGEST_TYPE_SHA512,             /**< SHA-512 */
};
```

**arguments**:

| name      | type                    |description                 |
|:----------|:------------------------|----------------------------|
| `type`    | `enum digest_type`      | digest_type                |
| `msg`     | `const unsigned char *` | the data to be hashed      |
| `msg_len` | `uint32_t`              | the length of the data     |
| `key`     | `const unsigned char *` | the hmac key               |
| `key_len` | `uint32_t`              | the length of the hmac key |
| `digest`  | `unsigned char *`       | hmac result                |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |

## Sample Code

#### CRC8 sample code

```c
CRC8_Context crc_context = {0};
uint8_t checksum = 0;

uint8_t data1[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t data2[8] = {0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18};

CRC8_Init(&crc_context);
CRC8_Update(&crc_context, data1, sizeof(data1));
CRC8_Update(&crc_context, data2, sizeof(data2));
CRC8_Final(&crc_context, &checksum);

printf("crc8: 0x%x\r\n", checksum);
```

#### CRC16 sample code

```c
CRC16_Context crc_context = {0};
uint16_t checksum = 0;

uint8_t data1[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t data2[8] = {0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18};

CRC16_Init(&crc_context);
CRC16_Update(&crc_context, data1, sizeof(data1));
CRC16_Update(&crc_context, data2, sizeof(data2));
CRC16_Final(&crc_context, &checksum);

printf("crc16: 0x%x\r\n", checksum);
```

#### md5 sample code

```c
void *md5_handler = NULL;
uint8_t md5_result[33] = {0};

uint16_t checksum = 0;

uint8_t data1[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t data2[8] = {0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18};

md5_handler = digest_md5_init();

digest_md5_update(md5_handler, data1, sizeof(data1));
digest_md5_update(md5_handler, data2, sizeof(data2));
digest_md5_final(md5_handler, md5_result);
printf("md5: %s\r\n", md5_result);

digest_md5(data1, sizeof(data1), md5_result);
printf("md5: %s\r\n", md5_result);

digest_md5_file("./file", md5_result);
printf("md5: %s\r\n", md5_result);
```


#### sha256 sample code

```c
void *sha256_handler = NULL;
uint8_t sha256_result[33] = {0};

uint16_t checksum = 0;

uint8_t data1[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t data2[8] = {0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18};

sha256_handler = digest_sha256_init();

digest_sha256_update(sha256_handler, data1, sizeof(data1));
digest_sha256_update(sha256_handler, data2, sizeof(data2));
digest_sha256_final(sha256_handler, sha256_result);
printf("sha256: %s\r\n", sha256_result);

digest_sha256(data1, sizeof(data1), sha256_result);
printf("sha256: %s\r\n", sha256_result);

digest_sha256_file("./file", sha256_result);
printf("sha256: %s\r\n", sha256_result);
```

## Reference

[数字信息摘要常见算法](https://baike.baidu.com/item/%E6%B6%88%E6%81%AF%E6%91%98%E8%A6%81%E7%AE%97%E6%B3%95/3286770?fromtitle=%E6%91%98%E8%A6%81%E7%AE%97%E6%B3%95&fromid=12011257)