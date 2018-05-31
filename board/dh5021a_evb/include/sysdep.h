#ifndef __SYSDEP_H__
#define __SYSDEP_H__

#include <config.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>

#include <time.h>
#include <sys/time.h>

/* fs */
//#include <k_vfs.h>
#include <poll.h>
#include <fcntl.h>

/* network */
#ifdef CONFIG_MODULES_NET_PROTOCOL
#include <netdb.h>
#include <arpa/inet.h>
#endif

/* aliyuniot use */
#define HEAP_CHECK

#endif

