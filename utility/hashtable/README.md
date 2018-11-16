## Contents

* [Introduction](#introduction)
* [APIs](#apis)
    * [ht_init()](#ht_init)
    * [ht_lock()](#ht_lock)
    * [ht_unlock()](#ht_unlock)
    * [ht_find_lockless()](#ht_find_lockless)
    * [ht_find()](#ht_find)
    * [ht_add_lockless()](#ht_add_lockless)
    * [ht_add()](#ht_add)
    * [ht_del_lockless()](#ht_del_lockless)
    * [ht_del()](#ht_del)
    * [ht_iterator_lockless()](#ht_iterator_lockless)
    * [ht_clear_lockless()](#ht_clear_lockless)
    * [ht_clear()](#ht_clear)
    * [ht_destroy()](#ht_destroy)

## Introduction

[hash table](https://en.wikipedia.org/wiki/Hash_table) is a data structure that implements an associative array abstract data type, a structure that can map keys to values.

The **hashtable** component provides friendly [APIs](#apis) to create, add, find hash table.

## APIs

#### ht_init()

```c
void *ht_init(int max_cnt);
```

**arguments**:

| name      | type       |description           |
|:----------|:-----------|----------------------|
| `max_cnt` | `int`      | hashtable max length |
| `length`  | `uint32_t` | input data len       |

**return**:

|return    |description        |
|:---------| ------------------|
| `void *` | hashtable handler |


#### ht_lock()

You may invoke this interface when you find/add/delete hashtable in lockless mode.

```c
void ht_lock(void *ht);
```

**arguments**:

| name | type     |description        |
|:-----|:---------|-------------------|
| `ht` | `void *` | hashtable handler |

#### ht_unlock()

Unlock hashtable

```c
void ht_unlock(void *ht);
```

**arguments**:

| name | type     |description        |
|:-----|:---------|-------------------|
| `ht` | `void *` | hashtable handler |

#### ht_find_lockless()

Find the item with key from hashtable in lockless mode, this maybe more efficient.

```c
void *ht_find_lockless(void *ht, const void  *key, unsigned int key_len,
                       void *val, int *size_val);
```

**arguments**:

| name       | type            |description                           |
|:-----------|:----------------|--------------------------------------|
| `ht`       | `void *`        | hashtable handler                    |
| `key`      | `const void  *` | the key of the item you want to find |
| `key_len`  | `unsigned int`  | key length                           |
| `val`      | `void *`        | found value                          |
| `size_val` | `int *`         | found value size                     |

**return**:

|return    |description                    |
|:---------| ------------------------------|
| `void *` | the pointer to the found item |

#### ht_find()

find the item with key from hashtable.

```c
void *ht_find(void *ht, const void  *key, unsigned int key_len,
                       void *val, int *size_val);
```

**arguments**:

| name       | type            |description                           |
|:-----------|:----------------|--------------------------------------|
| `ht`       | `void *`        | hashtable handler                    |
| `key`      | `const void  *` | the key of the item you want to find |
| `key_len`  | `unsigned int`  | key length                           |
| `val`      | `void *`        | found value                          |
| `size_val` | `int *`         | found value size                     |

**return**:

|return    |description                    |
|:---------| ------------------------------|
| `void *` | the pointer to the found item |

#### ht_add_lockless()

add item into hashtable with the key and the value in lockless mode.

```c
int ht_add_lockless(void *ht, const void *key, unsigned int len_key,
                    const void *val, unsigned int size_val);
```

**arguments**:

| name       | type            |description                           |
|:-----------|:----------------|--------------------------------------|
| `ht`       | `void *`        | hashtable handler                    |
| `key`      | `const void  *` | the key of the item you want to add  |
| `key_len`  | `unsigned int`  | key length                           |
| `val`      | `void *`        | the key of the value you want to add |
| `size_val` | `int *`         | value size                           |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |

#### ht_add()

add item into hashtable with the key and the value.

```c
int ht_add(void *ht, const void *key, unsigned int len_key,
                    const void *val, unsigned int size_val);
```

**arguments**:

| name       | type            |description                           |
|:-----------|:----------------|--------------------------------------|
| `ht`       | `void *`        | hashtable handler                    |
| `key`      | `const void  *` | the key of the item you want to add  |
| `key_len`  | `unsigned int`  | key length                           |
| `val`      | `void *`        | the key of the value you want to add |
| `size_val` | `int *`         | value size                           |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |

#### ht_del_lockless()

delete the item with key from hashtable in lockless mode.

```c
int ht_del_lockless(void *ht, const void *key, unsigned int len_key);
```

**arguments**:

| name       | type            |description                             |
|:-----------|:----------------|----------------------------------------|
| `ht`       | `void *`        | hashtable handler                      |
| `key`      | `const void  *` | the key of the item you want to delete |
| `key_len`  | `unsigned int`  | key length                             |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |

#### ht_del()

delete the item with key from hashtable.

```c
int ht_del(void *ht, const void *key, unsigned int len_key);
```

**arguments**:

| name       | type            |description                             |
|:-----------|:----------------|----------------------------------------|
| `ht`       | `void *`        | hashtable handler                      |
| `key`      | `const void  *` | the key of the item you want to delete |
| `key_len`  | `unsigned int`  | key length                             |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |

#### ht_iterator_lockless()

polling the hashtable and invoke the callback function.

```c
void ht_iterator_lockless(void *ht, iter_func func, void *extra);
```
iter_func struct defined:

```c
typedef void *(*iter_func)(void *key, void *val, void *extra);
```

**arguments**:

| name    | type        |description                                             |
|:--------|:------------|--------------------------------------------------------|
| `ht`    | `void *`    | hashtable handler                                      |
| `func`  | `iter_func` | the function to be invoked while polling the hashtable |
| `extra` | `void *`    | extra information                                      |

#### ht_clear_lockless()

clear all the item in hashtable in lockless mode.

```c
int ht_clear_lockless(void *ht);
```

**arguments**:

| name | type     |description        |
|:-----|:---------|-------------------|
| `ht` | `void *` | hashtable handler |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |

#### ht_clear()

clear all the item in hashtable.

```c
int ht_clear(void *ht);
```

**arguments**:

| name | type     |description        |
|:-----|:---------|-------------------|
| `ht` | `void *` | hashtable handler |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |

#### ht_destroy()

Delete all the items in the hashtable and release memory.

```c
int ht_destroy(void *ht);
```

**arguments**:

| name | type     |description        |
|:-----|:---------|-------------------|
| `ht` | `void *` | hashtable handler |

**return**:

|return |description |
|:------| -----------|
| 0     | sucess     |
| 1     | failure    |

## Reference

* [数据结构之哈希表](http://www.cnblogs.com/s-b-b/p/6208565.html)
