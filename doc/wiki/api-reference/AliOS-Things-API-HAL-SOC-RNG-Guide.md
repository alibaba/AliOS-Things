# API INDEX

  * [1 hal_random_num_read](#1-hal_random_num_read)

------

## 1-hal_random_num_read

```c
int32_t hal_random_num_read(random_dev_t random, void *buf, int32_t bytes)
```

- [x] **Description**

  Fill in a memory buffer with random data

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  random |      the random device   |
  | [out] | inBuffer |    Point to a valid memory buffer, this function will fill      in this memory with random numbers after executed   |
  | [in] |  inByteCount | Length of the memory buffer (bytes) |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step
