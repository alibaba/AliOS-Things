# API INDEX

  * [1 aos_kv_set](#1-aos_kv_set)
  * [2 aos_kv_get](#2-aos_kv_get)
  * [3 aos_kv_del](#3-aos_kv_del)

------

## 1-aos_kv_set

```c
int aos_kv_set(const char *key, const void *value, int len, int sync)
```

- [x] **Description**

  Add a new KV pair.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | key |   the key of the KV pair.   |
  | [in] | value | the value of the KV pair.   |
  | [in] | len |   the length of the value.   |
  | [in] | sync |  save the KV pair to flash right now (should always be 1). |

- [x] **Returns**

  0 on success, negative error on failure.

## 2-aos_kv_get

```c
int aos_kv_get(const char *key, void *buffer, int *buffer_len)
```

- [x] **Description**

  Get the KV pair's value stored in buffer by its key.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | key      |  the key of the KV pair to get.   |
  | [out] |  buffer   |  the memory to store the value.   |
  | [in-out] | buffer_len | in: the length of the input buffer.        out: the real length of the value. |

- [x] **Returns**

  0 on success, negative error on failure.

## 3-aos_kv_del

```c
int aos_kv_del(const char *key)
```

- [x] **Description**

  Delete the KV pair by its key.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | key | the key of the KV pair to delete. |

- [x] **Returns**

  0 on success, negative error on failure.
