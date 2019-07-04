## Contents

```sh
@projectname@
├── @projectname@.c # @projectname@ source code
├── Config.in       # kconfig file
├── aos.mk          # aos build system file(for make)
└── k_app_config.h  # aos app config file
```

## Introduction

The **@projectname@** ...

### Dependencies

### Supported Boards

- @boardname@

### Build

```sh
# generate @projectname@@@boardname@ default config
aos make @projectname@@@boardname@ -c config

# build
aos make
```
