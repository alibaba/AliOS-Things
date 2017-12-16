/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <aos/kernel.h>

#include "yunit.h"

#include "aos/aos.h"
#include "yts.h"
#include "umesh.h"

extern int dda_enable(int agent_id);
extern int dda_service_init(void);
extern int dda_service_start(void);
extern void dda_service_stop(void);
extern void dda_service_deinit(void);
static int ddm_pid, ddm_fd = -1;
static int start_ddm(void)
{
    int pipefd[2];
    int pid;
    int ret;

    ret = pipe(pipefd);
    if (ret < 0) {
        return -1;
    }

    pid = fork();
    if (pid == 0) {
        const char **argv;
        yts_get_args(&argv);
        char * const args[] = {
            (char *)argv[0],
            "--mesh-master",
            "-l",
            "/tmp/ddm_log_ut.txt",
            "tools/dda/configs/mesh_topology.txt",
            NULL
        };
        close(pipefd[1]);
        dup2(pipefd[0], fileno(stdin));
        execve(args[0], args, NULL);
    }
    close(pipefd[0]);
    ddm_fd = pipefd[1];

    return pid;
}

void ur_ut_send_cmd_to_ddm(const char *cmd)
{
    int ret;
    ret = write(ddm_fd, cmd, strlen(cmd));
    if (ret < 0)
        perror("write ddm:");
    ret = write(ddm_fd, "\n", 1);
    if (ret < 0)
        perror("write ddm:");
}

void start_dda(void)
{
    dda_enable(11);
    dda_service_init();
    dda_service_start();
}

static void stop_dda(void *args)
{
    dda_service_stop();
    dda_service_deinit();
}

static bool dda_connected;
static void input_event_executor(input_event_t *eventinfo, void* cb_para)
{
    if (eventinfo->code != CODE_DDA_ON_CONNECTED)
        return;

    dda_connected = true;
}

static int init(void)
{
    aos_register_event_filter(EV_DDA, input_event_executor, NULL);

    ddm_pid = start_ddm();
    ur_ut_send_cmd_to_ddm("log off 0");

    start_dda();

    int cnt = 0;
    while (!dda_connected && cnt++ < 100) {
        aos_msleep(100);
    }

    YUNIT_ASSERT(UR_ERROR_NONE == umesh_stop());

    aos_msleep(1000);

    return cnt < 100 ? 0 : -1;
}

static int cleanup(void)
{
    aos_schedule_call(stop_dda, NULL);

    ur_ut_send_cmd_to_ddm("goto master");
    ur_ut_send_cmd_to_ddm("ls");
    ur_ut_send_cmd_to_ddm("q");

    waitpid(ddm_pid, NULL, 0);

    aos_register_event_filter(EV_DDA, input_event_executor, NULL);

    return 0;
}

static void setup(void)
{
}

static void teardown(void)
{
}

extern void test_uradar_sid_router_case(void);
extern void test_uradar_vector_router_case(void);
extern void test_uradar_6lowpan_case(void);
extern void test_uradar_sid_allocator_case(void);
extern void test_uradar_rsid_allocator_case(void);
extern void test_uradar_urmesh_case(void);
extern void test_uradar_misc_case(void);
extern void test_uradar_lwip_adapter_case(void);
extern void test_uradar_mesh_mgmt_case(void);
extern void test_uradar_cli_case(void);
extern void test_uradar_mcast_case(void);
extern void test_uradar_1hop_case(void);
extern void test_uradar_1mobile_case(void);
extern void test_uradar_topology_line_case(void);
extern void test_uradar_scales_5nodes_case(void);
extern void test_uradar_scales_10nodes_case(void);
extern void test_uradar_scales_20nodes_case(void);
extern void test_uradar_scales_30nodes_case(void);
extern void test_uradar_layer_routing_line_topology_case(void);
extern void test_uradar_layer_routing_2mobile_case(void);
extern void test_uradar_dest_become_unreachable_case(void);
extern void test_uradar_asymmetric_link_case(void);
extern void test_uradar_topo_case(void);
extern void test_hal_mesh_case(void);
extern void test_diags_case(void);
extern void test_umesh_4super_7nodes_case(void);

/* submodule ut */
static yunit_test_case_t krhino_uradar_sub_testcases[] = {
    { "hal_mesh", test_hal_mesh_case },
    { "diags", test_diags_case },
    { "sid_router", test_uradar_sid_router_case },
    { "vector_router", test_uradar_vector_router_case },
    { "6lowpan", test_uradar_6lowpan_case },
    { "sid_allocator", test_uradar_sid_allocator_case },
    { "rsid_allocator", test_uradar_rsid_allocator_case },
    { "urmesh", test_uradar_urmesh_case },
    { "misc", test_uradar_misc_case },
    { "lwip_adapter", test_uradar_lwip_adapter_case },
    { "mesh_mgmt", test_uradar_mesh_mgmt_case },
    { "cli", test_uradar_cli_case },
    YUNIT_TEST_CASE_NULL
};

/* compound case */
static yunit_test_case_t krhino_uradar_com_testcases[] = {
    { "mcast", test_uradar_mcast_case },
    { "1hop", test_uradar_1hop_case },
    { "1mobile", test_uradar_1mobile_case },
    { "top_line", test_uradar_topology_line_case },
    { "layer_routing_simple", test_uradar_layer_routing_line_topology_case },
    { "layer_routing_2mobile", test_uradar_layer_routing_2mobile_case },
    { "dest_unreachable", test_uradar_dest_become_unreachable_case },
    { "asymmetric_link", test_uradar_asymmetric_link_case },
    { "topology", test_uradar_topo_case },
    { "4super_7nodes", test_umesh_4super_7nodes_case },
    YUNIT_TEST_CASE_NULL
};

/* stress case */
static yunit_test_case_t krhino_uradar_stress_testcases[] = {
    { "scales_5", test_uradar_scales_5nodes_case },
    { "scales_10", test_uradar_scales_10nodes_case },
    { "scales_20", test_uradar_scales_20nodes_case },
    { "scales_30", test_uradar_scales_30nodes_case },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_case_t krhino_uradar_mini_testcases[] = {
    /* submodule ut */
    { "sid_router", test_uradar_sid_router_case },
    { "6lowpan", test_uradar_6lowpan_case },
    { "sid_allocator", test_uradar_sid_allocator_case },
    { "rsid_allocator", test_uradar_rsid_allocator_case },
    { "lwip_adapter", test_uradar_lwip_adapter_case },
    { "mesh_mgmt", test_uradar_mesh_mgmt_case },
    /* compound case */
    { "mcast", test_uradar_mcast_case },
    { "1hop", test_uradar_1hop_case },
    { "top_line", test_uradar_topology_line_case },
    { "layer_routing_simple", test_uradar_layer_routing_line_topology_case },
    { "layer_routing_2mobile", test_uradar_layer_routing_2mobile_case },
    { "topology", test_uradar_topo_case },
    /* stress case */
    { "scales_5", test_uradar_scales_5nodes_case },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_case_t krhino_uradar_testcases[] = {
    /* submodule ut */
    { "hal_mesh", test_hal_mesh_case },
    { "diags", test_diags_case },
    { "sid_router", test_uradar_sid_router_case },
    { "vector_router", test_uradar_vector_router_case },
    { "6lowpan", test_uradar_6lowpan_case },
    { "sid_allocator", test_uradar_sid_allocator_case },
    { "rsid_allocator", test_uradar_rsid_allocator_case },
    { "urmesh", test_uradar_urmesh_case },
    { "misc", test_uradar_misc_case },
    { "lwip_adapter", test_uradar_lwip_adapter_case },
    { "mesh_mgmt", test_uradar_mesh_mgmt_case },
    { "cli", test_uradar_cli_case },
    /* compound case */
    { "mcast", test_uradar_mcast_case },
    { "1hop", test_uradar_1hop_case },
    { "1mobile", test_uradar_1mobile_case },
    { "top_line", test_uradar_topology_line_case },
    { "layer_routing_simple", test_uradar_layer_routing_line_topology_case },
    { "layer_routing_2mobile", test_uradar_layer_routing_2mobile_case },
    { "dest_unreachable", test_uradar_dest_become_unreachable_case },
    { "asymmetric_link", test_uradar_asymmetric_link_case },
    { "topology", test_uradar_topo_case },
    { "4super_7nodes", test_umesh_4super_7nodes_case },
    /* stress case */
    { "scales_5", test_uradar_scales_5nodes_case },
    { "scales_10", test_uradar_scales_10nodes_case },
    { "scales_20", test_uradar_scales_20nodes_case },
    { "scales_30", test_uradar_scales_30nodes_case },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "mesh", init, cleanup, setup, teardown, krhino_uradar_testcases },
    YUNIT_TEST_SUITE_NULL
};

static yunit_test_suite_t sub_suites[] = {
    { "mesh_sub", init, cleanup, setup, teardown, krhino_uradar_sub_testcases },
    YUNIT_TEST_SUITE_NULL
};

static yunit_test_suite_t com_suites[] = {
    { "mesh_com", init, cleanup, setup, teardown, krhino_uradar_com_testcases },
    YUNIT_TEST_SUITE_NULL
};

static yunit_test_suite_t stress_suites[] = {
    { "mesh_stress", init, cleanup, setup, teardown, krhino_uradar_stress_testcases },
    YUNIT_TEST_SUITE_NULL
};

static yunit_test_suite_t mini_suites[] = {
    { "mesh_mini", init, cleanup, setup, teardown, krhino_uradar_mini_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_uradar(void)
{
    int argc;
    const char **argv;

    argc = yts_get_args(&argv);
    if (argc > 1) {
        yunit_add_test_suites(sub_suites);
        yunit_add_test_suites(com_suites);
        yunit_add_test_suites(stress_suites);
        yunit_add_test_suites(mini_suites);
    }

    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_uradar);
