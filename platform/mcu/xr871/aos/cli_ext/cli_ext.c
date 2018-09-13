
extern void cli_cmd_add_upgrade(void);
extern void cli_cmd_add_mem(void);

void aos_cli_ext_init(void)
{
    cli_cmd_add_upgrade();
    cli_cmd_add_mem();
}
