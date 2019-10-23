#include "task_group.h"

#if (RHINO_CONFIG_UCLI)
#include "ucli.h"
#endif

#if (RHINO_CONFIG_URES)
#include "res.h"
#endif

#if (RHINO_CONFIG_UCALLBACK)
#include "callback.h"
#endif

#if (RHINO_CONFIG_UFS)
#include "fsfd.h"
#include "fs/vfs_api.h"
#endif

#if (RHINO_CONFIG_UTCPIP)
#include "socketfd.h"
#include "lwip/sockets.h"
#endif

#if (RHINO_CONFIG_UIPC)
#include "ipc.h"
#endif

int fsfd_init_sh(task_group_t *group)
{
#if (RHINO_CONFIG_UFS)
    return fsfd_init(group);
#else
    return 0;
#endif
}

int fsfd_exit_sh(task_group_t *group)
{
#if (RHINO_CONFIG_UFS)
    fsfd_exit(group);
#endif
}

int fsfd_close(int fd)
{
#if (RHINO_CONFIG_UFS)
    return vfs_close(fd);
#else
    return 0;
#endif
}

int socketfd_init_sh(task_group_t *group)
{
#if (RHINO_CONFIG_UTCPIP)
    return socketfd_init(group);
#else
    return 0;
#endif
}

void socketfd_exit_sh(task_group_t *group)
{
#if (RHINO_CONFIG_UTCPIP)
    socketfd_exit(group);
#endif
}

int socketfd_close(int fd)
{
#if (RHINO_CONFIG_UFS)
    return lwip_close(fd);
#else
    return 0;
#endif
}

int cb_init_sh(task_group_t *group)
{
#if (RHINO_CONFIG_UCALLBACK)
    return cb_init(group);
#else
    return 0;
#endif
}

void cb_exit_sh(task_group_t *group)
{
#if (RHINO_CONFIG_UCALLBACK)
    cb_exit(group);
#endif
}

int res_init_sh(task_group_t *group)
{
#if (RHINO_CONFIG_URES)
    return res_init(group);
#else
    return 0;
#endif
}

void res_exit_sh(task_group_t *group)
{
#if (RHINO_CONFIG_URES)
    res_exit(group);
#endif
}

int ucli_init_sh(task_group_t *group)
{
#if (RHINO_CONFIG_UCLI)
    return ucli_init(group);
#else
    return 0;
#endif
}

void ucli_exit_sh(task_group_t *group)
{
#if (RHINO_CONFIG_UCLI)
    ucli_exit(group);
#endif
}

void msg_exit_sh(uint32_t pid)
{
#if (RHINO_CONFIG_UCLI)
    msg_exit(pid);
#endif
}
