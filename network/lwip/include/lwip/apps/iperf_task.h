
#pragma once

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/* for iperf task */
#define IPERF_NAME "iperf"
#define IPERF_STACKSIZE 1536
#define IPERF_PRIO 6

#define IPERF_COMMAND_BUFFER_NUM (18)
#define IPERF_COMMAND_BUFFER_SIZE (20) // 4 bytes align


/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

void iperf_udp_run_server(char *parameters[]);
void iperf_tcp_run_server(char *parameters[]);
void iperf_udp_run_client(char *parameters[]);
void iperf_tcp_run_client(char *parameters[]);

void iperf_get_current_time(uint32_t *s, uint32_t *ms);

