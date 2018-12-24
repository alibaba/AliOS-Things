## Introduction

**Base64** is a group of similar binary-to-text encoding schemes that represent binary data in an ASCII string format by translating it into a radix-64 representation.

**Base64** encoding schemes are commonly used when there is a need to encode binary data that needs to be stored and transferred over media that are designed to deal with textual data.

### Features

The **Base64** component provides 2 APIs to encode & decode data:
* [base64_encode()](#base64_encode)
* [base64_decode()](#base64_decode)

### Dependencies

*(none)*

## APIs

### base64_encode()

`base64_encode()` used to encode data to base64 format.

```c
unsigned char *base64_encode(const unsigned char *input, int input_len,
                             unsigned char *output, int *output_len);
```

##### Arguments

| name        | type                   |description              |
|:------------|:-----------------------|-------------------------|
| `input`     | `const unsigned char *`| the data need to encode |
| `input_len` | `int`                  | input data len          |
| `output`    | `unsigned char *`      | already encoded data    |
| `input_len` | `int *`                | encoded data len        |


##### Return

|return             |description             |
|:------------------| ---------------------- |
| `unsigned char *` | address of output data |
| `NULL`            | something error        |

##### sample code

```c
unsigned char input[] = "AliOS-Things";
unsigned char *output;
int output_len;

base64_encode(input, strlen(input), output, &output_len);
```

You can use linux command: `base64` to test the `base64_encode()` result:

```sh
$ echo "AliOS-Things" | base64
QWxpT1MtVGhpbmdzCg==
```

### base64_decode()

`base64_decode()` use to decode base64 format data to normal data.

```c
unsigned char *base64_decode(const unsigned char *input, int input_len,
                             unsigned char *output, int *output_len)
```

##### Arguments

| name        | type                   |description              |
|:------------|:-----------------------|-------------------------|
| `input`     | `const unsigned char *`| the data need to decode |
| `input_len` | `int`                  | input data len          |
| `output`    | `unsigned char *`      | already decoded data    |
| `input_len` | `int *`                | decoded data len        |

##### Return

|return             |description              |
|:------------------|-------------------------|
| `unsigned char *` | address of decoded data |
| `NULL`            | something error         |


##### sample code
```c
unsigned char input[] = "QWxpT1MtVGhpbmdzCg==";
unsigned char *output;
int output_len;

base64_decode(input, strlen(input), output, &output_len);
```

you can use linux command: `base64` to test the `base64_decode()` result:

```sh
$ echo "QWxpT1MtVGhpbmdzCg==" | base64 -d
AliOS-Things
```

## Reference

* [base64 Wikipedia](https://en.wikipedia.org/wiki/Base64)
* [base64 decode/encode online](https://www.base64decode.org/)