#include "blecfg_pub.h"

#include "aos/kv.h"

#ifdef AOS_COMP_CLI
#include "aos/cli.h"

extern char g_blecfg_devname[128];

typedef int (*ble_netCfg_shell_cmd_function_t)(int argc, char *argv[]);

struct ble_netCfg_shell_cmd {
    const char *cmd_name;
    ble_netCfg_shell_cmd_function_t cb;
    const char *help;
    const char *desc;
};

int BleCfg_cli_cmd_start(int argc, char *argv[])
{
    BleCfg_stack_start();
    return 0;
}

int BleCfg_cli_cmd_stop(int argc, char *argv[])
{
    BleCfg_stack_stop();
    return 0;
}

int BleCfg_cli_cmd_name(int argc, char *argv[])
{
    uint8_t *bt_addr = NULL;

    printf("netconfig device name %s\r\n", g_blecfg_devname);

#ifdef CHIP_HAAS1000
    extern uint8_t *factory_section_get_bt_address(void);
    bt_addr = factory_section_get_bt_address();
    printf("BT address %02x-%02x-%02x-%02x-%02x-%02x\r\n",
            bt_addr[0], bt_addr[1], bt_addr[2], bt_addr[3], bt_addr[4], bt_addr[5]);
#endif

    return 0;
}

static const struct ble_netCfg_shell_cmd ble_netCfg_commands[] = {
    { "start", BleCfg_cli_cmd_start, "None" },
    { "stop", BleCfg_cli_cmd_stop, "None" },
    { "name", BleCfg_cli_cmd_name, "None" },
    { NULL, NULL, NULL }
};

static void BleCfg_cli_cmd_handler(int argc, char **argv)
{
    int i = 0;
    int err;

    if (argc < 2) {
        BLECFG_LOG_INFO("Ble NetConfig support commands");

        for (i = 0; ble_netCfg_commands[i].cmd_name != NULL; i ++) {
            BLECFG_LOG_INFO("    %s %s", ble_netCfg_commands[i].cmd_name, ble_netCfg_commands[i].help);
        }
        return;
    }

    for (i = 0; ble_netCfg_commands[i].cmd_name != NULL; i ++) {
        if (strlen(ble_netCfg_commands[i].cmd_name) == strlen(argv[1]) &&
            !strncmp(ble_netCfg_commands[i].cmd_name, argv[1], strlen(ble_netCfg_commands[i].cmd_name))) {
            if (ble_netCfg_commands[i].cb) {
                err = ble_netCfg_commands[i].cb(argc - 1, &argv[1]);

                if (err) {
                    BLECFG_LOG_INFO("%s execute fail, %d\n", ble_netCfg_commands[i].cmd_name, err);
                }
                break;
            }
        }
    }
}

/*
kv_init has been called in aos_components_init, but can called repeatedly.
#ifdef AOS_COMP_KV
    kv_init();
#endif
*/

static void BleCfg_kv_clear(int argc, char **argv)
{
    int res;
    int idx;
    char *key_to_del[] = {"product_key", "device_name", "device_secret", "wifi_ssid", "wifi_password"};

    for (idx = 0; idx < sizeof(key_to_del) / sizeof(key_to_del[0]); idx++)
    {
        res = aos_kv_del(key_to_del[idx]);
        printf("aos_kv_clear '%s' %s\r\n", key_to_del[idx], res == 0 ? "success." : "fail!");
    }
}

ALIOS_CLI_CMD_REGISTER(BleCfg_kv_clear, kvclear, kv info clear)
ALIOS_CLI_CMD_REGISTER(BleCfg_cli_cmd_handler, ble_netCfg, ble netConfig commands)

#endif

