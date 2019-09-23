/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "aos/kernel.h"

#include <network/network.h>
#include "lwip/apps/telnetserver.h"

#define MAX_COMMANDLINE_LENGTH 512U
#define AOS_TELNETD_PRI (AOS_DEFAULT_APP_PRI+1)

typedef struct
{
    char command_buffer[MAX_COMMANDLINE_LENGTH];  /* A buffer which stores the currently being built command. */
    int  buffer_position;                         /* The current write position in the command buffer. */
} CommandInterpreter_t;

static CommandInterpreter_t interpreter;

static bool telnetserver_is_running = false;

static aos_task_t   aos_telnetd_task;

extern int proc_onecmd(int argc, char *argv[]);

/**
 * Clear all characters from the command buffer.
 *
 * @param none
 * @retval none
 */
void ClearCommandBuffer(void) {
        for (int i = 0; i < MAX_COMMANDLINE_LENGTH; ++i) {
                interpreter.command_buffer[i] = '\0';
        }
        interpreter.buffer_position = 0U;
}

/* Parse input line and locate arguments (if any), keeping count of the number
* of arguments and their locations.  Look up and call the corresponding cli
* function if one is found and pass it the argv array.
*
* Returns: 0 on success: the input line contained at least a function name and
*          that function exists and was called.
*          1 on lookup failure: there is no corresponding function for the
*          input line.
*          2 on invalid syntax: the arguments list couldn't be parsed
*/
static int CommandParseLine(char *inbuf)
{
    struct {
        unsigned inArg: 1;
        unsigned inQuote: 1;
        unsigned done: 1;
    } stat;
    static char *argvall[CLI_MAX_ONCECMD_NUM][CLI_MAX_ARG_NUM];
    int argcall[CLI_MAX_ONCECMD_NUM] = {0};
    /*
    static char *argv[CLI_MAX_ONCECMD_NUM][CLI_MAX_ARG_NUM];
    int argc = 0;*/
    int cmdnum = 0;
    int *pargc = &argcall[0];
    int i = 0;
    int ret = 0;

    memset((void *)&argvall, 0, sizeof(argvall));
    memset((void *)&argcall, 0, sizeof(argcall));
    memset(&stat, 0, sizeof(stat));

    do {
        switch (inbuf[i]) {
            case '\0':
                if (stat.inQuote) {
                    return 2;
                }
                stat.done = 1;
                break;

            case '"':
                if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg) {
                    memcpy(&inbuf[i - 1], &inbuf[i], strlen(&inbuf[i]) + 1);
                    --i;
                    break;
                }
                if (!stat.inQuote && stat.inArg) {
                    break;
                }
                if (stat.inQuote && !stat.inArg) {
                    return 2;
                }

                if (!stat.inQuote && !stat.inArg) {
                    stat.inArg = 1;
                    stat.inQuote = 1;
                    (*pargc)++;
                    argvall[cmdnum][(*pargc) - 1] = &inbuf[i + 1];
                } else if (stat.inQuote && stat.inArg) {
                    stat.inArg = 0;
                    stat.inQuote = 0;
                    inbuf[i] = '\0';
                }
                break;

            case ' ':
                if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg) {
                    memcpy(&inbuf[i - 1], &inbuf[i],
                           strlen(&inbuf[i]) + 1);
                    --i;
                    break;
                }
                if (!stat.inQuote && stat.inArg) {
                    stat.inArg = 0;
                    inbuf[i] = '\0';
                }
                break;

            case ';':
                if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg) {
                    memcpy(&inbuf[i - 1], &inbuf[i],
                           strlen(&inbuf[i]) + 1);
                    --i;
                    break;
                }
                if (stat.inQuote) {
                    return 2;
                }
                if (!stat.inQuote && stat.inArg) {
                    stat.inArg = 0;
                    inbuf[i] = '\0';

                    if (*pargc) {
                        if (++cmdnum < CLI_MAX_ONCECMD_NUM) {
                            pargc = &argcall[cmdnum];
                        }
                    }
                }

                break;

            default:
                if (!stat.inArg) {
                    stat.inArg = 1;
                    (*pargc)++;
                    argvall[cmdnum][(*pargc) - 1] = &inbuf[i];
                }
                break;
        }
    } while (!stat.done && ++i < INBUF_SIZE && cmdnum < CLI_MAX_ONCECMD_NUM && (*pargc) < CLI_MAX_ARG_NUM);

    if (stat.inQuote) {
        return 2;
    }

    for ( i = 0; i <= cmdnum && i < CLI_MAX_ONCECMD_NUM ; i++ ) {
        TelnetWriteString("\n");
        ret |= proc_onecmd(argcall[i], argvall[i]);
    }

    return ret;
}

void CommandAddChar(const char character)
{
    if (character != 0x00 && interpreter.buffer_position < MAX_COMMANDLINE_LENGTH)
    {
        if (character == 0x0A || character == 0x0D)
        {
            /* We have reached the end of a command, parse it */
            CommandParseLine(interpreter.command_buffer);
            /* Clear the command buffer */
            ClearCommandBuffer();
        }
        else if (character == 0x08 || character == 0x7F)
        {
            /* Remove the last character from the buffer */
            if (interpreter.buffer_position > 0)
            {
                --interpreter.buffer_position; /* Move the list back one */
                interpreter.command_buffer[interpreter.buffer_position] = '\0'; /* Set the current position to a null character */
            }
       }
       else if (character == 0x20)
       {
           if (interpreter.buffer_position > 0
               && interpreter.command_buffer[interpreter.buffer_position - 1] != 0x20)
           {
               /* Add another character to the buffer */
               interpreter.command_buffer[interpreter.buffer_position] = character; /* Add the character at the current position */
               ++interpreter.buffer_position; /* Move the buffer pointer to the next position */
           }
           /* If the previous character is a space, ignore further spaces */
      }
      else
      {
           /* Add another character to the buffer */
           interpreter.command_buffer[interpreter.buffer_position] = character; /* Add the character at the current position */
           ++interpreter.buffer_position; /* Move the buffer pointer to the next position */
      }
    }
    return ;
}

static void TelnetServerProcess(void* arg)
{
   char character;
   char telcmd[10] = "echo tel";

   if (InitializeTelnetServer() != TELNET_OK)
   {
       return ;
   }

   telnetserver_is_running = true;

   /* set cli aos telnet server mode */
   CommandParseLine(telcmd);

   while(1)
   {
        if(telnetserver_is_running != true)
        {
            aos_task_exit(0);
        }

        if (TelnetIsConnected() == true)
        { /* We have an active Telnet connection to service */
            /* Do server stuff */
            character = TelnetRead();
            if(character != '\0')
            {
                CommandAddChar(character);
            }
            else
            {
                aos_msleep(500);
            }
        }
        else
        {
           //printf("Telnet is not connected, try again later.\n");
           aos_msleep(1000);
        }
        aos_msleep(50);
   }
   return ;
}

void telnetserver_start()
{
   aos_task_new_ext(&aos_telnetd_task, "telnetserver", TelnetServerProcess, NULL, 81920, AOS_TELNETD_PRI);
}

void telnetserver_stop()
{
  telnetserver_is_running = false;
  if(TelnetIsConnected() == true)
  {
     TelnetClose();
  }
}

