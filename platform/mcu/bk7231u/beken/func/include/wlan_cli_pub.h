/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 BEKEN Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of BEKEN Corporation.
 *
 */
#ifndef __WLAN_CLI_PUB_H__
#define __WLAN_CLI_PUB_H__

#include "include.h"
#include "BkDriverUart.h"

#define CLI_UART        BK_UART_2

#define RX_WAIT         BEKEN_WAIT_FOREVER
#define SEND_WAIT       BEKEN_WAIT_FOREVER

#define RET_CHAR        '\n'
#define END_CHAR		'\r'
#define PROMPT			"\r\n# "
#define EXIT_MSG		"exit"

#define NUM_BUFFERS		1
#define MAX_COMMANDS	64
#define INBUF_SIZE      128
#define OUTBUF_SIZE     2048

#define LOG_SERVICE_BUFLEN 100

/** Structure for registering CLI commands */
struct cli_command
{
    /** The name of the CLI command */
    const char *name;
    /** The help text associated with the command */
    const char *help;
    /** The function that should be invoked for this command. */
    void (*function) (char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
};

struct cli_st
{
    int initialized;
    const struct cli_command *commands[MAX_COMMANDS];
    unsigned int num_commands;
    int echo_disabled;
	
    unsigned int bp;	/* buffer pointer */
    char inbuf[INBUF_SIZE];
	
    char outbuf[OUTBUF_SIZE];
} ;


#define cmd_printf(...) do{\
                                if (xWriteBufferLen > 0) {\
                                    snprintf(pcWriteBuffer, xWriteBufferLen, __VA_ARGS__);\
                                    xWriteBufferLen-= os_strlen(pcWriteBuffer);\
                                    pcWriteBuffer+= os_strlen(pcWriteBuffer);\
                                }\
                             }while(0)


#define CLI_ARGS char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv

#define is_print(c)           ((u8_t)c >= 0x20 && (u8_t)c <= 0x7f)

/** Register a CLI command
 *
 * This function registers a command with the command-line interface.
 *
 * \param[in] command The structure to register one CLI command
 * \return 0 on success
 * \return 1 on failure
 */
int cli_register_command(const struct cli_command *command);

/** Unregister a CLI command
 *
 * This function unregisters a command from the command-line interface.
 *
 * \param[in] command The structure to unregister one CLI command
 * \return 0 on success
 * \return 1 on failure
 */
int cli_unregister_command(const struct cli_command *command);



/** Stop the CLI thread and carry out the cleanup
 *
 * \return kNoErr on success
 * \return error code otherwise.
 *
 */
int cli_stop(void);

/** Register a batch of CLI commands
 *
 * Often, a module will want to register several commands.
 *
 * \param[in] commands Pointer to an array of commands.
 * \param[in] num_commands Number of commands in the array.
 * \return 0 on success
 * \return 1 on failure
 */
int cli_register_commands(const struct cli_command *commands, int num_commands);

/** Unregister a batch of CLI commands
 *
 * \param[in] commands Pointer to an array of commands.
 * \param[in] num_commands Number of commands in the array.
 * \return 0 on success
 * \return 1 on failure
 */
int cli_unregister_commands(const struct cli_command *commands,
                            int num_commands);

/* Get a CLI msg
 *
 * If an external input task wants to use the CLI, it can use
 * cli_get_cmd_buffer() to get a command buffer that it can then
 * submit to the CLI later using cli_submit_cmd_buffer().
 *
 * \param buff Pointer to a char * to place the buffer pointer in.
 * \return 0 on success
 * \return error code otherwise.
 */
int cli_getchar(char *inbuf);
int cli_getchars(char *inbuf, int len);
int cli_get_all_chars_len(void);
int cli_getchars_prefetch(char *inbuf, int len);

/* Send CLI output msg
 *
 * \param buff Pointer to a char * buffer.
 * \return 0 on success
 * \return error code otherwise.
 */
int cli_printf(const char *buff, ...);


// library CLI APIs
int cli_init(void);
void wifistate_Command(CLI_ARGS);
void wifidebug_Command(CLI_ARGS);
void wifiscan_Command(CLI_ARGS);
void ifconfig_Command(CLI_ARGS);
void arp_Command(CLI_ARGS);
void ping_Command(CLI_ARGS);
void dns_Command(CLI_ARGS);
void socket_show_Command(CLI_ARGS);
void memory_show_Command(CLI_ARGS);
void memory_dump_Command(CLI_ARGS);
void memory_set_Command(CLI_ARGS);
void memp_dump_Command(CLI_ARGS);
void driver_state_Command(CLI_ARGS);

#ifdef MOC
void task_Command(CLI_ARGS);
#endif

#endif

