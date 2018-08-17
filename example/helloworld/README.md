## helloworld

### Content
- [helloworld](#helloworld)
    - [Content](#content)
    - [Overview](#overview)
        - [Dependent components](#dependent-components)
    - [Supported boards](#supported-boards)
    - [Build](#build)
    - [Run](#run)

### Overview

`helloworld` is a very simple example,  just print task name every 5 seconds with function name and code line as a prefix. helloworld source files:

```sh
helloworld
    ├── helloworld.c
    └── helloworld.mk
    └── ucube.mk
```

To learn how the helloword works please refer to [Hello World](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-APP-DEV-Guide) and [Yloop Event Framework](https://github.com/alibaba/AliOS-Things/wiki/Yloop-Event-Framework).

#### Dependent components

`yloop`  `cli`

### Supported boards

actually, `helloworld` supports all the boards.

### Build

using [aos-cube](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uCube) to build helloworld for [linuxhost](../../board/linuxhost/README.md):

```sh
aos make helloworld@linuxhost
```

### Run

execute `out/helloworld@linuxhost/binary/helloworld@linuxhost.elf`:

```sh
./out/helloworld@linuxhost/binary/helloworld@linuxhost.elf
```

and `task name` will print every 5 seconds like this:

```sh
 [   0.000]<D> main [setrlimit_for_vfs#132] : Limit max open files to 64
cpu num is 1
 [   0.000]<V> AOS [application_start#15] : application started.
 [   1.000]<V> AOS [app_delayed_action#9] : helloworld app_delayed_action:9 app_task

 [   6.000]<V> AOS [app_delayed_action#9] : helloworld app_delayed_action:9 app_task

 [  11.000]<V> AOS [app_delayed_action#9] : helloworld app_delayed_action:9 app_task

 [  16.000]<V> AOS [app_delayed_action#9] : helloworld app_delayed_action:9 app_task

 [  21.000]<V> AOS [app_delayed_action#9] : helloworld app_delayed_action:9 app_task

 [  26.000]<V> AOS [app_delayed_action#9] : helloworld app_delayed_action:9 app_task

```