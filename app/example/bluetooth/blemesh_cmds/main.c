/* main.c - Application main entry point */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/printk.h>

#include <bluetooth/bluetooth.h>
#include <api/mesh.h>

#ifdef CONFIG_AOS_CLI
static void handle_bt_mesh_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    struct mesh_shell_cmd *mesh_cmds = NULL, *p;
    char *cmd_str, no_match = 1;;

    if (strcmp(argv[0], "bt_mesh") != 0) {
        return;
    }

    if (argc <= 1) {
        cmd_str = "help";
    } else {
        cmd_str = argv[1];
    }

    mesh_cmds = bt_mesh_get_shell_cmd_list();
    if (mesh_cmds) {
        p = mesh_cmds;
        while (p->cmd_name != NULL) {
            if (strcmp(p->cmd_name, cmd_str) != 0) {
                p++;
                continue;
            }
            if (p->cb) {
                no_match = 0;
                p->cb(argc - 1, &(argv[1]));
            }
            break;
        }
    }
}

static struct cli_command mcmd = { .name     = "bt_mesh",
                                   .help     = "bt_mesh [cmd] [options]",
                                   .function = handle_bt_mesh_cmd };
#endif

extern int hci_driver_init();
int application_start(int argc, char **argv)
{
#ifdef CONFIG_AOS_CLI
    aos_cli_register_command(&mcmd);
#endif
    hci_driver_init();
    aos_loop_run();
    return 0;
}
