#ifndef MKSH_H
#define MKSH_H

int fsfd_init_sh(task_group_t *group);

int fsfd_exit_sh(task_group_t *group);

int fsfd_close(int fd);

int socketfd_init_sh(task_group_t *group);

void socketfd_exit_sh(task_group_t *group);

int socketfd_close(int fd);

int fsfd_close(int fd);

int cb_init_sh(task_group_t *group);

void cb_exit_sh(task_group_t *group);

int res_init_sh(task_group_t *group);

void res_exit_sh(task_group_t *group);

int ucli_init(task_group_t *group);

void ucli_exit_sh(task_group_t *group);

void msg_exit_sh(uint32_t pid);

#endif /* MKSH_H */

