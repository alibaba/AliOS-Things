## Introduction

**chip_code** provides app or other component an api([get_chip_code()](#get_chip_code)) to get chip_code by chip name.

### Features

chip_code data struct including `vendor`, `id`, `mcu_family_name`, and defined as this:

```c
typedef struct __chip_code {
    UINT16  vendor;
    UINT16  id;
    const char  *mcu_family_name;
} chip_code_st;
```

* `vendor` list can be found by enum **CHIP_VENDOR** in `chip_code.h`.
* `id` list can be found by enum **CHIP_ID** in `chip_code.h`.
* `mcu_family_name` must be same with `HOST_MCU_FAMILY`(defined by the build system).

all the already defined chip_code can be found by the variable **g_chip_codes** in `chip_code.c`.

### Dependencies

*(None)*

## APIs

### get_chip_code()

```c
chip_code_st *get_chip_code( char *name);
```

##### Arguments

| name        | type    |description    |
|:------------|:--------|---------------|
| `name`      | `char *`| the chip name |

##### Return

|return            |description            |
|:-----------------| ----------------------|
| `chip_code_st *` | address of found chip |
| `NULL`           | find nothing          |
