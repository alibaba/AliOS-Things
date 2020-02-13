## Contents

```sh
@componentname@_app
├── Config.in       # kconfig file
├── aos.mk          # aos build system file(for make)
├── app.config      # aos app config file
├── main.c          # program entry main()
└── app_main.c      # application entry application_start()
```

## Introduction

The **@componentname@_app** ...

### Dependencies

### Supported Boards


### Build

```sh
# build
aos create project -b boardname -d destdir -t @componentname@_app appname
aos make
```
