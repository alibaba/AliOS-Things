#ifndef SA_CFG_H
#define SA_CFG_H

struct sa_cfg {
    char buf_mp[4096];
};

void cfg_sa_write_cfg(struct sa_cfg *new_cfg, uint16_t len);
void cfg_dump_sa_table(int len);
#if 1
void cfg_dump_sa_pre_table();
void cfg_dump_sa_post_table();
#endif
extern const struct sa_cfg g_sa_cfg;

#endif /* end of include guard: SA_CFG_H */
