#include <stdio.h>
#include <stdlib.h>
#include "aos/kernel.h"
#include "uagent.h"
#include "aos/errno.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

static int test_uagent_register_normal(void)
{
    const int ret = uagent_register(UAGENT_MOD_ULOG, "ulog",
            "", UAGENT_FUNC_USER_BASE, "dummy",
            NULL, NULL);
    return ret;
}

static int test_uagent_register_error_input(void)
{
    const int ret = uagent_register(UAGENT_MOD_ULOG, NULL,
            NULL, UAGENT_FUNC_USER_BASE, NULL,
            NULL, NULL);
    return ret;
}

static int test_uagent_unregister_normal(void)
{
    const int ret = uagent_unregister(UAGENT_MOD_ULOG, UAGENT_FUNC_USER_BASE);

    return ret;
}

static int test_uagent_request_service_normal(void)
{
    const int ret = uagent_request_service(UAGENT_MOD_UAGENT, UAGENT_MOD_ULOG, UAGENT_FUNC_USER_BASE, strlen("ulog level=5") + 1, "ulog level=5");

    return ret;
}

static int test_uagent_request_service_error_input_1(void)
{
    const int ret = uagent_request_service(UAGENT_MOD_UAGENT, UAGENT_MOD_ULOG, UAGENT_FUNC_USER_BASE, 12, NULL);

    return ret;
}

static int test_uagent_request_service_error_input_2(void)
{
    const int ret = uagent_request_service(UAGENT_MOD_UAGENT, UAGENT_MOD_ULOG, UAGENT_FUNC_USER_BASE, 65535, "ulog level=5");

    return ret;
}

static int test_uagent_send_normal(void)
{
    const int ret = uagent_send(UAGENT_MOD_ULOG, UAGENT_FUNC_USER_BASE, strlen("helloworld") + 1, "helloworld", 0);
    return ret;
}

static int test_uagent_send_error_input(void)
{
    const int ret = uagent_send(UAGENT_MOD_ULOG, UAGENT_FUNC_USER_BASE, strlen("helloworld") + 1, NULL, 0);
    return ret;
}

static int test_uagent_ext_comm_start_normal(char *pk, char *dn)
{
    const int ret = uagent_ext_comm_start(pk, dn);
    return ret;
}

static int test_uagent_ext_comm_start_error_input(void)
{
    const int ret = uagent_ext_comm_start(NULL, NULL);
    return ret;
}

int test_uagent_init(void *handle, char *pk, char *dn)
{
    int ret = 0;
    ret = uagent_init();
    if (ret != 0) {
        printf("uAgent Init Failed ret = %d\n", ret);
        return ret;
    }
    ret = uagent_mqtt_client_set(handle);
    if (ret != 0) {
        printf("uAgent mqtt handle set failed ret = %d\n", ret);
        return ret;
    }
    ret = uagent_ext_comm_start(pk, dn);
    if (ret != 0) {
        printf("uAgent ext comm  start failed ret = %d\n", ret);
        return ret;
    }
    return 0;
}

void test_uagent_start(int argc, char **argv)
{
    int ret = 0;
    ret = test_uagent_register_normal();

    aos_msleep(2000);

    ret = test_uagent_register_error_input();
    aos_msleep(2000);

    ret = test_uagent_request_service_normal();
    aos_msleep(2000);

    ret = test_uagent_request_service_error_input_1();
    aos_msleep(2000);

    ret = test_uagent_request_service_error_input_2();
    aos_msleep(2000);

    ret = test_uagent_ext_comm_start_error_input();
    aos_msleep(2000);

    ret = test_uagent_send_normal();
    aos_msleep(2000);

    ret = test_uagent_send_error_input();
    aos_msleep(2000);

    ret = test_uagent_unregister_normal();
    aos_msleep(2000);
    return 0;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(test_uagent_start, uagent_example, uagent component base example)
#endif
