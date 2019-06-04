#ifndef __SYSCALL_H
#define __SYSCALL_H

#include <stddef.h>

size_t SYSCALL(unsigned int syscall_no, void *arg);

#endif /* __SYSCALL_H */

