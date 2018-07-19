#ifndef USER_CFG_H
#define USER_CFG_H

struct user_cfg {
    char buf_user[0];
};

void cfg_user_write_cfg(uint8_t *new_user_data, uint16_t len);
void cfg_user_read_cfg(uint8_t *flash_user_data, uint16_t len);

void cfg_dump_user_table(int len);
#if 1
void cfg_dump_user_pre_table();
void cfg_dump_user_post_table();
#endif
extern const struct user_cfg g_user_cfg;

#define SET_CFG(flag,bit) (flag = flag | 1<<bit)
#define CHECK_CFG_OK(flag) ((flag & 0x0F) == 0x0F)
#define CLEAR_CFG(flag)	  (flag = 0)

#define RW_BLOCK_SIZE	(512)
#define RW_TOTAL_SIZE	(4*512)

#endif /* end of include guard: USER_CFG_H */
