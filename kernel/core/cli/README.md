## Contents

## Introduction
**cli** (Command Line Interface) provides a simple, small, extensible and RAM efficient method of enabling other modules or applications to process command line input.

### Features
- Command Register
- Command Execute

### Directories

```sh
cli
├── cli_adapt.c           # CLI OS adapt layer implementation
├── cli.c                 # CLI command manage and character process
├── cli_default_command.c # CLI built-in commands implementation
├── cli_dumpsys.c         # CLI dump system command implementation
├── include
│   ├── cli_adapt.h       # CLI OS adapt layer declaration
│   ├── cli_conf.h        # CLI configuration
│   └── dumpsys.h         # CLI dumpsys interface declaration
└──README.md
```

## Reference
- [为你的AliOS Things应用增加自定义cli命令](https://yq.aliyun.com/articles/669091)
