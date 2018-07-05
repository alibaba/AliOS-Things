# API INDEX

  * [1 aos_cli_register_command](#1-aos_cli_register_command)
  * [2 aos_cli_unregister_command](#2-aos_cli_unregister_command)
  * [3 aos_cli_register_commands](#3-aos_cli_register_commands)
  * [4 aos_cli_unregister_commands](#4-aos_cli_unregister_commands)
  * [5 aos_cli_printf](#5-aos_cli_printf)
  * [6 aos_cli_init](#6-aos_cli_init)
  * [7 aos_cli_stop](#7-aos_cli_stop)

------

## 1-aos_cli_register_command

```c
int aos_cli_register_command(const struct cli_command *command)
```

- [x] **Description**

  This function registers a command with the command-line interface.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | command | The structure to register one CLI command |

- [x] **Returns**

  0 on success, error code otherwise.

## 2-aos_cli_unregister_command

```c
int aos_cli_unregister_command(const struct cli_command *command)
```

- [x] **Description**

  This function unregisters a command from the command-line interface.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | command | The structure to unregister one CLI command |

- [x] **Returns**

  0 on success,  error code otherwise.

## 3-aos_cli_register_commands

```c
int aos_cli_register_commands(const struct cli_command *commands, int num_commands)
```

- [x] **Description**

  Register a batch of CLI commands
  Often, a module will want to register several commands.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | commands |     Pointer to an array of commands.   |
  | [in] | num_commands | Number of commands in the array. |

- [x] **Returns**

  0 on success， error code otherwise.

## 4-aos_cli_unregister_commands

```c
int aos_cli_unregister_commands(const struct cli_command *commands, int num_commands)
```

- [x] **Description**

  Unregister a batch of CLI commands

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | commands |     Pointer to an array of commands.   |
  | [in] | num_commands | Number of commands in the array. |

- [x] **Returns**

  0 on success, error code otherwise.

## 5-aos_cli_printf

```c
int aos_cli_printf(const char *buff, ...)
```

- [x] **Description**

  Print CLI msg

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | buff | Pointer to a char * buffer. |

- [x] **Returns**

  0  on success, error code otherwise.

## 6-aos_cli_init

```c
int aos_cli_init(void)
```

- [x] **Description**

  CLI initial function

- [x] **Parameters**

  None.

- [x] **Returns**

  0 on success, error code otherwise

## 7-aos_cli_stop

```c
int aos_cli_stop(void)
```

- [x] **Description**

  Stop the CLI thread and carry out the cleanup

- [x] **Parameters**

  None.

- [x] **Returns**

  0 on success, error code otherwise.