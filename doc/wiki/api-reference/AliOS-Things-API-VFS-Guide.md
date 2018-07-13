# API INDEX

  * [1 aos_open](#1-aos_open)
  * [2 aos_close](#2-aos_close)
  * [3 aos_read](#3-aos_read)
  * [4 aos_write](#4-aos_write)
  * [5 aos_ioctl](#5-aos_ioctl)
  * [6 aos_poll](#6-aos_poll)
  * [7 aos_fcntl](#7-aos_fcntl)
  * [8 aos_lseek](#8-aos_lseek)
  * [9 aos_sync](#9-aos_sync)
  * [10 aos_stat](#10-aos_stat)
  * [11 aos_unlink](#11-aos_unlink)
  * [12 aos_rename](#12-aos_rename)
  * [13 aos_opendir](#13-aos_opendir)
  * [14 aos_closedir](#14-aos_closedir)
  * [15 aos_readdir](#15-aos_readdir)
  * [16 aos_mkdir](#16-aos_mkdir)

------

## 1-aos_open

```c
int aos_open(const char *path, int flags)
```

- [x] **Description**

  Open the file or device by its path.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | path |  the path of the file or device to open.   |
  | [in] | flags | the mode of open operation. |

- [x] **Returns**

  the new file descriptor, negative error on failure.

## 2-aos_close

```c
int aos_close(int fd)
```

- [x] **Description**

  Close the file or device by its file descriptor.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | fd | the file descriptor of the file or device. |

- [x] **Returns**

  0 on success, negative error on failure.

## 3-aos_read

```c
ssize_t aos_read(int fd, void *buf, size_t nbytes)
```

- [x] **Description**

  Read the contents of a file or device into a buffer.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  fd |     the file descriptor of the file or device.   |
  | [in] |  nbytes | the number of bytes to read.   |
  | [out] | buf |    the buffer to read in to. |

- [x] **Returns**

  The number of bytes read, 0 at end of file, negative error on failure.

## 4-aos_write

```c
ssize_t aos_write(int fd, const void *buf, size_t nbytes)
```

- [x] **Description**

  Write the contents of a buffer to file or device.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | fd |     the file descriptor of the file or device.   |
  | [in] | nbytes | the number of bytes to write.   |
  | [in] | buf |    the buffer to write from. |

- [x] **Returns**

  The number of bytes written, negative error on failure.

## 5-aos_ioctl

```c
int aos_ioctl(int fd, int cmd, unsigned long arg)
```

- [x] **Description**

  This is a wildcard API for sending controller specific commands.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | fd |  the file descriptior of the file or device.   |
  | [in] | cmd | A controller specific command.   |
  | [in] | arg | Argument to the command, interpreted according to the command. |

- [x] **Returns**

  any return from the command.

## 6-aos_poll

```c
int aos_poll(struct pollfd *fds, int nfds, int timeout)
```

- [x] **Description**

  A mechanism to multiplex input/output over a set of file descriptors.
  For every file descriptor provided, poll() examines it for any events registered for that particular file descriptor.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | fds |     a point to the array of pollfd struct carrying a file descriptor and bitmasks of events.   |
  | [in] | nfhs |    number of file descriptors.   |
  | [in] | timeout | timer value to timeout or -1 for loop forever. |

- [x] **Returns**

  number of file descriptors selected (for which revents is non-zero). 0 if timed out with nothing selected. -1 for error.

## 7-aos_fcntl

```c
int aos_fcntl(int fd, int cmd, int val)
```

- [x] **Description**

  Performs one of the operations described below on the open file descriptor, The operation is determined by cmd.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | fd |  the file descriptor of the file or device.   |
  | [in] | cmd | the operation of the file or device.   |
  | [in] | val | it depends on whether cmd need params. |

- [x] **Returns**

  0 on success, negative error on failure.

## 8-aos_lseek

```c
off_t aos_lseek(int fd, off_t offset, int whence)
```

- [x] **Description**

  Move the file position to a given offset from a given location.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | fd |     the file descriptor of the file.   |
  | [in] | offset | The offset from whence to move to.   |
  | [in] | whence | The start of where to seek.SEEK_SET to start from beginning of file.SEEK_CUR to start from current position in file.SEEK_END to start from end of file. |

- [x] **Returns**

  The new offset of the file.

## 9-aos_sync

```c
int aos_sync(int fd)
```

- [x] **Description**

  Flush any buffers associated with the file.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | fd | the file descriptor of the file. |

- [x] **Returns**

  0 on success, negative error code on failure.

## 10-aos_stat

```c
int aos_stat(const char *path, struct stat *st)
```

- [x] **Description**

  Store information about the file in a stat structure.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  path | The path of the file to find information about.   |
  | [out] | st |   The stat buffer to write to. |

- [x] **Returns**

  0 on success, negative error code on failure.

## 11-aos_unlink

```c
int aos_unlink(const char *path)
```

- [x] **Description**

  Remove a file from the filesystem.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | path | The path of the file to remove. |

- [x] **Returns**

  0 on success, negative error code on failure.

## 12-aos_rename

```c
int aos_rename(const char *oldpath, const char *newpath)
```

- [x] **Description**

  Rename a file in the filesystem.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | oldpath | The path of the file to rename.   |
  | [in] | newpath | The path to rename it to. |

- [x] **Returns**

  0 on success, negative error code on failure.

## 13-aos_opendir

```c
aos_dir_t *aos_opendir(const char *path)
```

- [x] **Description**

  Open a directory on the filesystem.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | path | the path of the directory to open. |

- [x] **Returns**

  a point of directory stream on success, NULL on failure.

## 14-aos_closedir

```c
int aos_closedir(aos_dir_t *dir)
```

- [x] **Description**

  Close a directory.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | dir | the handle of the directory to close. |

- [x] **Returns**

  0 on success, negative error code on failure.

## 15-aos_readdir

```c
aos_dirent_t *aos_readdir(aos_dir_t *dir)
```

- [x] **Description**

  Read the next directory entry.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | dir | the handle of the directory to read. |

- [x] **Returns**

  a pointer to a dirent structure.

## 16-aos_mkdir

```c
int aos_mkdir(const char *path)
```

- [x] **Description**

  Create the directory, if they do not already exist.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | path | the path of the directory. |

- [x] **Returns**

  0 on success, negative error code on failure.
