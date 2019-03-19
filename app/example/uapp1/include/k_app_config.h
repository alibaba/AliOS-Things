

/* user space */
#ifndef RHINO_CONFIG_USER_SPACE
#define RHINO_CONFIG_USER_SPACE              1
#endif

#if (RHINO_CONFIG_USER_SPACE > 0)

#ifndef CONFIG_LWIP_SYSCALL
#define CONFIG_LWIP_SYSCALL                  1
#endif

#endif
