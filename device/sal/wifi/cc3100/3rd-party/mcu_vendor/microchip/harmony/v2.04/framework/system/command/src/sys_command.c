/*******************************************************************************
  Command Processor System Service Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    SYS_CMD.c

  Summary:
    Command Processor System Service Implementation.

  Description:
    This file contains the source code for the Command Processor System
    Service.  It provides a way to interact with the Command Processor subsystem
    to manage the ASCII command requests from the user supported by the system.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "system_definitions.h"
#include "system_config.h"
#include "system/command/sys_command.h"
#include "system/console/sys_console.h"

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

typedef struct _tagCmdNode
{
    struct _tagCmdNode* next;
    struct _tagCmdNode* prev;
    char    cmdBuff[SYS_CMD_MAX_LENGTH+1] SYS_CMD_BUFFER_DMA_READY;  // command itself
}cmdNode;   // simple command history

typedef struct
{
    cmdNode*    head;
    cmdNode*    tail;
}cmdDlList;     // doubly linked command list

// *****************************************************************************
// *****************************************************************************
// Section: Global Variable Definitions
// *****************************************************************************
// *****************************************************************************

extern SYS_MODULE_OBJ sysConsoleObjects[];

static SYS_CMD_DEVICE_LIST cmdIODevList = {0, 0, 0};

static char printBuff[SYS_CMD_PRINT_BUFFER_SIZE] SYS_CMD_BUFFER_DMA_READY;
static char readBuff[SYS_CMD_READ_BUFFER_SIZE] SYS_CMD_BUFFER_DMA_READY;
static int printBuffPtr = 0;

static SYS_CMD_INIT_DATA _cmdAppData =
{
    .bytesRead = 0
};

static SYS_CMD_DESCRIPTOR_TABLE   _usrCmdTbl[MAX_CMD_GROUP] = { {0} };    // current command table

static cmdDlList        _cmdList = {0, 0};       // root of the command history

static cmdNode*         _pCurrCmdN;      // history pointer

static const char       _seqUpArrow[ESC_SEQ_SIZE] = "[A";
static const char       _seqDownArrow[ESC_SEQ_SIZE] = "[B";
static const char       _seqRightArrow[ESC_SEQ_SIZE] = "[C";
static const char       _seqLeftArrow[ESC_SEQ_SIZE] = "[D";  // standard VT100 escape sequences

// prototypes

static int      CommandReset(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int      CommandQuit(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);              // command quit
static int      CommandHelp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);              // help
static void     CommandCleanup(void);           // resources clean-up

static int      StringToArgs(char *pRawString, char *argv[]); // Convert string to argc & argv[]
static bool     ParseCmdBuffer(SYS_CMD_DEVICE_NODE* pCmdIO);      // parse the command buffer

static void     ProcessEscSequence(SYS_CMD_DEVICE_NODE* pCmdIO);       // process an escape sequence

static void     CmdAddHead(cmdDlList* pL, cmdNode* pN);
static cmdNode* CmdRemoveTail(cmdDlList* pL);

static void     DisplayNodeMsg(SYS_CMD_DEVICE_NODE* pCmdIO, cmdNode* pNext);

static void SendCommandMessage(const void* cmdIoParam, const char* str);
static void SendCommandPrint(const void* cmdIoParam, const char* format, ...);
static void SendCommandCharacter(const void* cmdIoParam, char c);
static bool IsCommandReady(const void* cmdIoParam);
static char GetCommandCharacter(const void* cmdIoParam);
static size_t ReadCommandCharacter(const void* cmdIoParam);

const SYS_CMD_API sysConsoleApi =
{
    SendCommandMessage,
    SendCommandPrint,
    SendCommandCharacter,
    IsCommandReady,
    GetCommandCharacter,
    ReadCommandCharacter,
};

// built-in command table
static const SYS_CMD_DESCRIPTOR    _builtinCmdTbl[]=
{
    {"reset",   CommandReset,   ": Reset host"},
    {"q",       CommandQuit,    ": quit command processor"},
    {"help",    CommandHelp,    ": help"},
};

// *****************************************************************************
// *****************************************************************************
// Section: SYS CMD Operation Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    bool SYS_CMD_Initialize( const SYS_MODULE_INIT * const init  )

  Summary:
    Initializes data for the instance of the Command Processor module.

  Description:
    This function initializes the Command Processor module.
    It also initializes any internal data structures.

  Precondition:
    None.

  Parameters:
    init            - Pointer to a data structure containing any data necessary
                      to initialize the sys command. This pointer may be null if no
                      data is required because static overrides have been
                      provided.

  Returns:
    If successful, returns true.
    If there is an error, returns false.

  Remarks:
    This routine should only be called once during system initialization.
*/
bool SYS_CMD_Initialize(const SYS_MODULE_INIT * const init )
{
    SYS_CMD_INIT *initConfig = (SYS_CMD_INIT*)init;
    int         ix;

    CommandCleanup();       // just in case we have to deallocate previous data

    // construct the command history list
    for(ix = 0; ix<COMMAND_HISTORY_DEPTH; ix++)
    {
        cmdNode* pN;
        pN = (cmdNode*)malloc(sizeof(*pN));

        if(!pN)
        {
            return false;
        }
        pN->cmdBuff[0] = '\0';
        CmdAddHead(&_cmdList, pN);
    }
    _pCurrCmdN = 0;

    // the console handle should be needed here but there's only one console for now
    if (initConfig != NULL)
    {
        SYS_CMDIO_ADD(&sysConsoleApi, &initConfig->consoleCmdIOParam, initConfig->consoleCmdIOParam);
    }
    else
    {
        SYS_CMDIO_ADD(&sysConsoleApi, NULL, SYS_CMD_SINGLE_CHARACTER_READ_CONSOLE_IO_PARAM);
    }

    _cmdAppData.moduleInFd = STDIN_FILENO;
    _cmdAppData.moduleOutFd = STDOUT_FILENO;

    return true;
}

// *****************************************************************************
/* Function:
    bool SYS_CMD_READY_TO_WRITE( void )

  Summary:
    This function allows upper layer application to confirm that the command
    module is ready to write output to the Console System Service

  Description:
    This function allows upper layer application to confirm that the command
    module is ready to write output to the Console System Service

  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Returns:
    If ready, returns true.
    If not ready, returns false.

  Remarks:
    None.
*/
bool SYS_CMD_READY_TO_WRITE()
{
    short i;
    SYS_CMD_DEVICE_NODE* pCmdIO;
    const void*      cmdIoParam;

    for (i=0; i<cmdIODevList.num; i++)
    {
        pCmdIO = SYS_CMDIO_GET_HANDLE(i);

        if(pCmdIO)
        {
            cmdIoParam = pCmdIO->cmdIoParam;

            //Check if this command IO is ready
            if(!(*pCmdIO->pCmdApi->isRdy)(cmdIoParam))
            {
                return false;
            }
        }
    }
    return true;
}

// *****************************************************************************
/* Function:
    bool SYS_CMD_READY_TO_READ( void )

  Summary:
    This function allows upper layer application to confirm that the command
    module is ready to accept command input

  Description:
    This function allows upper layer application to confirm that the command
    module is ready to accept command input

  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Returns:
    If ready, returns true.
    If not ready, returns false.

  Remarks:
    None.
*/
bool SYS_CMD_READY_TO_READ()
{
    short i;
    SYS_CMD_DEVICE_NODE* pCmdIO;
    const void*      cmdIoParam;

    for (i=0; i<cmdIODevList.num; i++)
    {
        pCmdIO = SYS_CMDIO_GET_HANDLE(i);

        if(pCmdIO)
        {
            cmdIoParam = pCmdIO->cmdIoParam;

            //Check if this command IO is ready
            if(!(*pCmdIO->pCmdApi->isRdy)(cmdIoParam))
            {
                return false;
            }

            if (pCmdIO->cmdIoParam == NULL ||
                    *(uint8_t*)pCmdIO->cmdIoParam != SYS_CMD_FULL_COMMAND_READ_CONSOLE_IO_PARAM)
            {
//                if (pCmdIO->cmdState != SYS_CMD_STATE_SETUP_READ)                 //original code
                if ( (pCmdIO->cmdState != SYS_CMD_STATE_SETUP_READ)  &&             //modified code when CMD_Tasks() called before CONSOLE_Tasks()
                        (pCmdIO->cmdState != SYS_CMD_STATE_WAIT_FOR_READ_DONE) )
                {
                    SYS_CONSOLE_Flush(SYS_CONSOLE_INDEX_0);

                    (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, LINE_TERM LINE_TERM _promptStr);
                    pCmdIO->cmdState = SYS_CMD_STATE_SETUP_READ;
                }
            }
        }
    }
    return true;
}

// *****************************************************************************
/* Function:
    bool SYS_CMD_ADDGRP(const SYS_CMD_DESCRIPTOR* pCmdTbl, int nCmds,
                            const char* groupName,
                            const char* menuStr)

  Summary:
    API to allow clients to add command process to the Command Processor System
    Service.

  Description:
    Client modules may call this function to add command process to the Command
    Processor System Service

  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Example:

    static const SYS_CMD_DESCRIPTOR    cmdTbl[]=
    {
        {"command_as_typed_at_the_prompt",
        _Function_Name_That_Supports_The_Command,
        ": Helpful description of the command for the user"},
    };

    bool APP_AddCommandFunction()
    {
        if (!SYS_CMD_ADDGRP(cmdTbl, sizeof(cmdTbl)/sizeof(*cmdTbl),
                        "Command Group Name", ": Command Group Description"))
        {
            return false;
        }
        return true;
    }

  Returns:
    If successful, returns true.
    If there is an error, returns false.

  Remarks:
    None.
*/
bool  SYS_CMD_ADDGRP(const SYS_CMD_DESCRIPTOR* pCmdTbl, int nCmds, const char* groupName, const char* menuStr)
{
    int i, groupIx = -1, emptyIx = -1;
    int insertIx;

    // Check if there is space for new command group; If this table already added, also simply update.
    for (i=0; i<MAX_CMD_GROUP; i++)
    {
        if(_usrCmdTbl[i].pCmd == 0)
        {   // empty slot
            emptyIx = i;
        }
        else if(_usrCmdTbl[i].pCmd == pCmdTbl)
        {   // already have this group; sanity check against the group name
            if(strcmp(groupName, _usrCmdTbl[i].cmdGroupName) != 0)
            {   // name mismatch
                return false;
            }

            groupIx = i;
            break;
        }
    }

    // reference the command group
    if (groupIx != -1)
    {
        insertIx = groupIx;
    }
    else if(emptyIx != -1)
    {
        insertIx = emptyIx;
    }
    else
    {
        return false;
    }

    _usrCmdTbl[insertIx].pCmd = pCmdTbl;
    _usrCmdTbl[insertIx].nCmds = nCmds;
    _usrCmdTbl[insertIx].cmdGroupName = groupName;
    _usrCmdTbl[insertIx].cmdMenuStr = menuStr;
    return true;

}

// *****************************************************************************
/* Function:
    bool SYS_CMD_Tasks( void )

  Summary:
    Maintains the Command Processor System Service's internal state machine.

  Description:
    This function is used to maintain the Command Processor System Service
    internal state machine.

  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Returns:
    If successfully, returns true.
    If there is an error, returns false.

  Remarks:
    None.
*/
bool SYS_CMD_Tasks(void)
{
    short i;
    SYS_CMD_DEVICE_NODE* pCmdIO;

    for (i=0; i<cmdIODevList.num; i++)
    {
        pCmdIO = SYS_CMDIO_GET_HANDLE(i);

        if(pCmdIO)
        {
            //Check if this command IO is ready
            if(!(*pCmdIO->pCmdApi->isRdy)(pCmdIO) &&
                    pCmdIO->cmdIoType != SYS_CMD_TELNET_COMMAND_READ_CONSOLE_IO_PARAM)
            {
                continue;
            }

            switch(pCmdIO->cmdState)
            {
                case SYS_CMD_STATE_DISABLE:
                    //App layer is not ready to process commands yet
                    break;
                case SYS_CMD_STATE_SETUP_READ:
                    {
                        readBuff[0] = '\0';

                        if ( pCmdIO->cmdIoType != SYS_CMD_TELNET_COMMAND_READ_CONSOLE_IO_PARAM)
                        {
                            _cmdAppData.bytesRead = (*pCmdIO->pCmdApi->readc)(pCmdIO); /* Read data from console. */
                            _cmdAppData.seqBuff[0] = '\0';

                            if (pCmdIO->cmdIoParam == NULL || *(uint8_t*)pCmdIO->cmdIoParam == SYS_CMD_SINGLE_CHARACTER_READ_CONSOLE_IO_PARAM)
                            {
                                pCmdIO->cmdState = SYS_CMD_STATE_WAIT_FOR_READ_DONE;
                            }
                            else if (*(uint8_t*)pCmdIO->cmdIoParam == SYS_CMD_FULL_COMMAND_READ_CONSOLE_IO_PARAM)
                            {
                                pCmdIO->cmdState = SYS_CMD_STATE_PROCESS_FULL_READ;
                            }
                        }
                        else /* cmdIoParm is an I/O Socket, such as telnet */
                        {
                            readBuff[0] = (*pCmdIO->pCmdApi->getc)(pCmdIO->cmdIoParam); /* Read data from console. */
                            pCmdIO->cmdState = SYS_CMD_STATE_WAIT_FOR_READ_DONE;
                        }
                    }
                    break;
                case SYS_CMD_STATE_WAIT_FOR_READ_DONE:
                    {
                        if((readBuff[0] == '\r') || (readBuff[0] == '\n'))
                        {
                            pCmdIO->cmdState = SYS_CMD_STATE_SETUP_READ;

                            // new command assembled
                            if(pCmdIO->cmdPnt ==  pCmdIO->cmdBuff)
                            {   // just an extra \n or \r
                                (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, LINE_TERM _promptStr);
                                return true;
                            }
                            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, LINE_TERM);
                            *pCmdIO->cmdPnt = 0;
                            pCmdIO->cmdPnt = pCmdIO->cmdEnd = pCmdIO->cmdBuff;

                            if(!ParseCmdBuffer(pCmdIO))
                            {
                                //Command not processed, show prompt
                                (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, _promptStr);
                            }
                            else
                            {
                                //Command being processed, temporarily disable prompt until command completes
                                pCmdIO->cmdState = SYS_CMD_STATE_DISABLE;
                            }

                            return true;
                        }
                        else if(readBuff[0] == '\b')
                        {
                            pCmdIO->cmdState = SYS_CMD_STATE_SETUP_READ;

                            if(pCmdIO->cmdPnt > pCmdIO->cmdBuff)
                            {
                                (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "\b \b");
                                pCmdIO->cmdPnt--; pCmdIO->cmdEnd--;
                            }
                        }
                        else if(readBuff[0] == 0x1b)
                        {
                            _cmdAppData.seqBuff[0] = readBuff[1];
                            _cmdAppData.seqBuff[1] = readBuff[2];

                            // This is an escape sequence, start reading which cursor character
                            ProcessEscSequence(pCmdIO);

                            readBuff[0] = '\0';
                            readBuff[1] = '\0';
                            readBuff[2] = '\0';

                            pCmdIO->cmdState = SYS_CMD_STATE_SETUP_READ;
                        }
                        else if(pCmdIO->cmdEnd-pCmdIO->cmdBuff<SYS_CMD_MAX_LENGTH)
                        {
                            pCmdIO->cmdState = SYS_CMD_STATE_SETUP_READ;

                            if (readBuff[0]!='\0')
                            {
                                (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, readBuff);
                                *pCmdIO->cmdPnt++ = readBuff[0];
                            }

                            if(pCmdIO->cmdPnt > pCmdIO->cmdEnd)
                            {
                                pCmdIO->cmdEnd = pCmdIO->cmdPnt;
                            }
                        }
                        else
                        {
                            pCmdIO->cmdState = SYS_CMD_STATE_SETUP_READ;

                            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, " *** Command Processor buffer exceeded. Retry. ***" LINE_TERM);
                            pCmdIO->cmdPnt = pCmdIO->cmdEnd = pCmdIO->cmdBuff;
                            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, _promptStr);
                        }
                    }
                    break;
                case SYS_CMD_STATE_PROCESS_FULL_READ:
                    {
                        if (readBuff[0]!='\0')
                        {
                            readBuff[strlen(readBuff)-1] = '\0';

                            strcpy(pCmdIO->cmdBuff, readBuff);

                            ParseCmdBuffer(pCmdIO);

                            pCmdIO->cmdState = SYS_CMD_STATE_SETUP_READ;

                            return true;
                        }
                    }
                    break;
                }
            }
        }
    return true;
}

// *****************************************************************************
/* Function:
    void SYS_CMD_MESSAGE (const char* message)

  Summary:
    Outputs a message to the Command Processor System Service console.

  Description:
    This function outputs a message to the Command Processor System Service
    console.
.
  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void SYS_CMD_MESSAGE(const char* message)
{
    SendCommandMessage(NULL, message);
}

// *****************************************************************************
/* Function:
    void SYS_CMD_PRINT(const char *format, ...)

  Summary:
    Outputs a printout to the Command Processor System Service console.

  Description:
    This function outputs a printout to the Command Processor System Service
    console.
.
  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void SYS_CMD_PRINT(const char* format, ...)
{
    char tmpBuf[SYS_CMD_PRINT_BUFFER_SIZE];
    size_t len = 0;
    size_t padding = 0;
    va_list args;
    va_start( args, format );

    len = vsnprintf(tmpBuf, SYS_CMD_PRINT_BUFFER_SIZE, format, args);

    va_end( args );

    if (len > 0 && len < SYS_CMD_PRINT_BUFFER_SIZE)
    {
        tmpBuf[len] = '\0';

        if (len + printBuffPtr >= SYS_CMD_PRINT_BUFFER_SIZE)
        {
            printBuffPtr = 0;
        }

        strcpy(&printBuff[printBuffPtr], tmpBuf);
        SendCommandMessage(NULL, &printBuff[printBuffPtr]);

        padding = len % 4;

        if (padding > 0)
        {
            padding = 4 - padding;
        }

        printBuffPtr += len + padding;
    }
}

// *****************************************************************************
/* Function:
    void SYS_CMD_RegisterCallback(SYS_CMD_CallbackFunction cbFunc, SYS_CMD_EVENT event)

  Summary:
    Registers a callback function with the command service that will be
    executed when the lower layer read or write queue is empty.

  Description:
    This function is used by an application to register a callback function
    with the command service. The callback function is called in response to
    an event. Separate callback functions are required for each event.

  Preconditions:
    None.

  Parameters:
    cbFunc              - The name of the callback function
    event               - Enumerated list of events that can trigger a callback

  Returns:
    None.

  Remarks:
    None.
*/

void SYS_CMD_RegisterCallback(SYS_CMD_CallbackFunction cbFunc, SYS_CMD_EVENT event)
{
    int moduleIndex = 0;

    for (moduleIndex = 0; moduleIndex < SYS_CONSOLE_INSTANCES_NUMBER; ++moduleIndex)
    {
        switch (event)
        {
            case SYS_CMD_EVENT_WRITE_COMPLETE:
                SYS_CONSOLE_RegisterCallback(_cmdAppData.moduleIndices[moduleIndex], cbFunc, SYS_CONSOLE_EVENT_WRITE_COMPLETE);
                break;
            case SYS_CMD_EVENT_READ_COMPLETE:
                SYS_CONSOLE_RegisterCallback(_cmdAppData.moduleIndices[moduleIndex], cbFunc, SYS_CONSOLE_EVENT_READ_COMPLETE);
                break;
            default:
                break;
        }
    }
}

SYS_CMD_DEVICE_NODE* SYS_CMDIO_GET_HANDLE(short num)
{
    SYS_CMD_DEVICE_NODE* pNode = cmdIODevList.head;

    if (num == 0) return ((pNode)?pNode:NULL);

    while(num && pNode)
    {
        pNode = pNode->next;
        num--;
    }

    return pNode;
}

SYS_CMD_DEVICE_NODE* SYS_CMDIO_ADD(const SYS_CMD_API* opApi, const void* cmdIoParam, const SYS_CMD_CONSOLE_IO_PARAM cmdIoType)
{
    // Create node
    SYS_CMD_DEVICE_NODE* pDevNode;

    pDevNode = (SYS_CMD_DEVICE_NODE*)malloc(sizeof(*pDevNode));
    if (!pDevNode)
    {
        return 0;
    }
    pDevNode->pCmdApi = opApi;
    pDevNode->cmdIoParam = cmdIoParam;
    pDevNode->cmdPnt = pDevNode->cmdEnd = pDevNode->cmdBuff;
    pDevNode->cmdIoType = cmdIoType;
    //pDevNode->index = cmdIODevList.num;

    if (cmdIoType == SYS_CMD_SINGLE_CHARACTER_READ_CONSOLE_IO_PARAM)
    {
        pDevNode->cmdState = SYS_CMD_STATE_DISABLE;
    }
    else
    {
        pDevNode->cmdState = SYS_CMD_STATE_SETUP_READ;
    }

    // Insert node at end
    cmdIODevList.num++;

    pDevNode->next = NULL;
    if(cmdIODevList.head == NULL)
    {
        cmdIODevList.head = pDevNode;
        cmdIODevList.tail = pDevNode;
    }
    else
    {
        cmdIODevList.tail->next = pDevNode;
        cmdIODevList.tail = pDevNode;
    }

    return pDevNode;
}


bool SYS_CMD_DELETE(SYS_CMD_DEVICE_NODE* pDevNode)
{
    SYS_CMD_DEVICE_NODE* p_listnode = cmdIODevList.head;
    SYS_CMD_DEVICE_NODE* pre_listnode;

    //root list is empty or null node to be delete
    if((p_listnode==NULL) || (pDevNode==NULL))
    {
        return false;
    }

    // Head will be delted
    if(p_listnode == pDevNode)
    {
        cmdIODevList.num--;
        //Root list has only one node
        if(cmdIODevList.tail == pDevNode)
        {
            cmdIODevList.head = NULL;
            cmdIODevList.tail = NULL;
        }
        else
        {
            cmdIODevList.head = p_listnode->next;
        }
        free(pDevNode);
        return true;
    }
    // Not head delete
    pre_listnode = p_listnode;
    while (p_listnode)
    {
        if(p_listnode == pDevNode)
        {
            pre_listnode->next = p_listnode->next;
            // Deleted node is tail
            if (cmdIODevList.tail==pDevNode) {
                cmdIODevList.tail = pre_listnode;
            }
            cmdIODevList.num--;
            free(pDevNode);
            return true;
        }
        pre_listnode = p_listnode;
        p_listnode   = p_listnode->next;
    }

    return false;
}

// ignore the console handle for now, we support a single system console
static void SendCommandMessage(const void* cmdIoParam, const char* message)
{
    int consoleIndex = 0;

    for (consoleIndex = 0; consoleIndex < SYS_CONSOLE_DEVICE_MAX_INSTANCES; ++consoleIndex)
    {
        SYS_CONSOLE_Write(consoleIndex, STDOUT_FILENO, message, strlen(message));
    }
}

static void SendCommandPrint(const void* cmdIoParam, const char* format, ...)
{
    char tmpBuf[SYS_CMD_PRINT_BUFFER_SIZE];
    size_t len = 0;
    size_t padding = 0;
    va_list args;
    va_start( args, format );

    len = vsnprintf(tmpBuf, SYS_CMD_PRINT_BUFFER_SIZE, format, args);

    va_end( args );


    if (len > 0 && len < SYS_CMD_PRINT_BUFFER_SIZE)
    {
        tmpBuf[len] = '\0';

        if (len + printBuffPtr >= SYS_CMD_PRINT_BUFFER_SIZE)
        {
            printBuffPtr = 0;
        }

        strcpy(&printBuff[printBuffPtr], tmpBuf);
        SendCommandMessage(NULL, &printBuff[printBuffPtr]);

        padding = len % 4;

        if (padding > 0)
        {
            padding = 4 - padding;
        }

        printBuffPtr += len + padding;
    }
}

static void SendCommandCharacter(const void* cmdIoParam, char c)
{
    int consoleIndex = 0;

    for (consoleIndex = 0; consoleIndex < SYS_CONSOLE_DEVICE_MAX_INSTANCES; ++consoleIndex)
    {
        if (SYS_CONSOLE_Status((SYS_MODULE_OBJ)consoleIndex) != SYS_STATUS_READY)
            continue;

        SYS_CONSOLE_Write(consoleIndex, STDOUT_FILENO, (const char*)&c, 1);
    }
}

static bool IsCommandReady(const void* cmdIoParam)
{
    int consoleIndex = 0;
    bool ready = false;

    for (consoleIndex = 0; consoleIndex < SYS_CONSOLE_DEVICE_MAX_INSTANCES; ++consoleIndex)
    {
        ready |= SYS_CONSOLE_Status((SYS_MODULE_OBJ)consoleIndex) == SYS_STATUS_READY;
    }

    return ready;
}


static char GetCommandCharacter(const void* cmdIoParam)
{
    return '\0';
}

static size_t ReadCommandCharacter(const void* cmdIoParam)
{
    int readConsoleIndex = 0;

    size_t readyBytes = 0;

    for (readConsoleIndex = 0; readConsoleIndex < SYS_CONSOLE_DEVICE_MAX_INSTANCES; ++readConsoleIndex)
    {
        if (SYS_CONSOLE_Status((SYS_MODULE_OBJ)readConsoleIndex) == SYS_STATUS_READY)
        {
            readyBytes = SYS_CONSOLE_Read(readConsoleIndex, _cmdAppData.moduleInFd, readBuff, 1);
        }

        if (readyBytes > 0)
            break;
    }

    return readyBytes;
}

// implementation
static int CommandReset(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** System Reboot ***\r\n" );

    SYS_RESET_SoftwareReset();

    return 1;
}

// quit
static int CommandQuit(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    SYS_CMD_DEVICE_NODE* pCmdIoNode;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** Quitting the Command Processor. Bye ***\r\n" );

    CommandCleanup();

    while ((pCmdIoNode = SYS_CMDIO_GET_HANDLE(0)) != NULL)
    {
        if(!SYS_CMD_DELETE(pCmdIoNode))
        {
            free(pCmdIoNode);
        }
    }

    return 1;
}

// Remove command history list
static void CommandCleanup(void)
{
    cmdNode* pN;

    memset(_usrCmdTbl, 0x0, sizeof(_usrCmdTbl));

    while( (pN = CmdRemoveTail(&_cmdList)) )
    {
        free(pN);
    }

    _pCurrCmdN = 0;
}

static int CommandHelp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int             ix, groupIx;
    const SYS_CMD_DESCRIPTOR*  pDcpt;
    const SYS_CMD_DESCRIPTOR_TABLE* pTbl, *pDTbl;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    if(argc == 1)
    {   // no params help; display basic info
        bool hadHeader = false;
        pTbl = _usrCmdTbl;
        for (groupIx=0; groupIx < MAX_CMD_GROUP; groupIx++)
        {
            if (pTbl->pCmd)
            {
                if(!hadHeader)
                {
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "------- Supported command groups ------");
                    hadHeader = true;
                }
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** ");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, pTbl->cmdGroupName);
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, pTbl->cmdMenuStr);
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, " ***");
            }
            pTbl++;
        }

        // display the basic commands
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "---------- Built in commands ----------");
        for(ix = 0, pDcpt = _builtinCmdTbl; ix < sizeof(_builtinCmdTbl)/sizeof(*_builtinCmdTbl); ix++, pDcpt++)
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** ");
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, pDcpt->cmdStr);
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, pDcpt->cmdDescr);
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, " ***");
        }

        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM);
    }
    else
    {   // we have a command group name
        pDTbl = 0;
        pTbl = _usrCmdTbl;
        for (groupIx=0; groupIx < MAX_CMD_GROUP; groupIx++)
        {
            if (pTbl->pCmd)
            {
                if(strcmp(pTbl->cmdGroupName, argv[1]) == 0)
                {   // match
                    pDTbl = pTbl;
                    break;
                }
            }
            pTbl++;
        }

        if(pDTbl)
        {
            for(ix = 0, pDcpt = pDTbl->pCmd; ix < pDTbl->nCmds; ix++, pDcpt++)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** ");
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, pDcpt->cmdStr);
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, pDcpt->cmdDescr);
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, " ***");
            }

            (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "Unknown command group. Try help" LINE_TERM );
        }
    }

    return 0;
}

static bool ParseCmdBuffer(SYS_CMD_DEVICE_NODE* pCmdIO)
{
    int  argc = 0;
    char *argv[MAX_CMD_ARGS + 1] = {};
    static char saveCmd[SYS_CMD_MAX_LENGTH+1];
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    int            ix, grp_ix;
    const SYS_CMD_DESCRIPTOR* pDcpt;

    strncpy(saveCmd, pCmdIO->cmdBuff, sizeof(saveCmd));     // make a copy of the command

    // parse a command string to *argv[]
    argc = StringToArgs(saveCmd, argv);

    if(argc != 0)
    {   // ok, there's smth here

        // add it to the history list
        cmdNode* pN = CmdRemoveTail(&_cmdList);
        strncpy(pN->cmdBuff, pCmdIO->cmdBuff, sizeof(saveCmd)); // Need save non-parsed string
        CmdAddHead(&_cmdList, pN);
        _pCurrCmdN = 0;

        // try built-in commands first
        for(ix = 0, pDcpt = _builtinCmdTbl; ix < sizeof(_builtinCmdTbl)/sizeof(*_builtinCmdTbl); ix++, pDcpt++)
        {
            if(!strcmp(argv[0], pDcpt->cmdStr))
            {   // command found
                return (*pDcpt->cmdFnc)(pCmdIO, argc, argv);     // call command handler
            }
        }
        // search user commands
        for (grp_ix=0; grp_ix<MAX_CMD_GROUP; grp_ix++)
        {
            if (_usrCmdTbl[grp_ix].pCmd == 0)
            {
               continue;
            }

            for(ix = 0, pDcpt = _usrCmdTbl[grp_ix].pCmd; ix < _usrCmdTbl[grp_ix].nCmds; ix++, pDcpt++)
            {
                if(!strcmp(argv[0], pDcpt->cmdStr))
                {
                    // command found
                    return !(*pDcpt->cmdFnc)(pCmdIO, argc, argv);
                }
            }
        }

        // command not found
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, " *** Command Processor: unknown command. ***\r\n");
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, " *** Command Processor: Please type in a command***" LINE_TERM);
    }

    return false;
}

/*
  parse a tring into '*argv[]', delimitor is space or tab
  param pRawString, the whole line of command string
  param argv, parsed argument string array
  return number of parsed argument
*/
static int StringToArgs(char *pRawString, char *argv[]) {
  int argc = 0, i = 0, strsize = 0;

  if(pRawString == NULL)
    return 0;

  strsize = strlen(pRawString);

  while(argc < MAX_CMD_ARGS) {

    // skip white space characters of string head
    while ((*pRawString == ' ') || (*pRawString == '\t')) {
      ++pRawString;
      if (++i >= strsize) {
        return (argc);
      }
    }

    if (*pRawString == '\0') {
      argv[argc] = NULL;
      return (argc);
    }

    argv[argc++] = pRawString;

    // find end of string
    while (*pRawString && (*pRawString != ' ') && (*pRawString != '\t')) {
      ++pRawString;
    }

    if (*pRawString == '\0') {
    argv[argc] = NULL;
    return (argc);
    }

    *pRawString++ = '\0';
  }

  SYS_PRINT("\n\r Too many arguments. Maximum argus supported is %d!\r\n", MAX_CMD_ARGS);

  return (0);
}


static void ProcessEscSequence(SYS_CMD_DEVICE_NODE* pCmdIO)
{
    cmdNode *pNext;

    if(!strcmp(_cmdAppData.seqBuff, _seqUpArrow))
    { // up arrow
        if(_pCurrCmdN)
        {
            pNext = _pCurrCmdN->next;
            if(pNext == _cmdList.head)
            {
                return; // reached the end of list
            }
        }
        else
        {
            pNext = _cmdList.head;
        }

        DisplayNodeMsg(pCmdIO, pNext);
    }
    else if(!strcmp(_cmdAppData.seqBuff, _seqDownArrow))
    { // down arrow
        if(_pCurrCmdN)
        {
            pNext = _pCurrCmdN->prev;
            if(pNext != _cmdList.tail)
            {
                DisplayNodeMsg(pCmdIO, pNext);
            }
        }
    }
    else if(!strcmp(_cmdAppData.seqBuff, _seqRightArrow))
    { // right arrow
        if(pCmdIO->cmdPnt < pCmdIO->cmdEnd)
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, _seqRightArrow);
            pCmdIO->cmdPnt++;
        }
    }
    else if(!strcmp(_cmdAppData.seqBuff, _seqLeftArrow))
    { // left arrow
        if(pCmdIO->cmdPnt > pCmdIO->cmdBuff)
        {
            pCmdIO->cmdPnt--;
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, _seqLeftArrow);
        }
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, " *** Command Processor: unknown command. ***" LINE_TERM);
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, _promptStr);
    }

}

static void DisplayNodeMsg(SYS_CMD_DEVICE_NODE* pCmdIO, cmdNode* pNext)
{
    int oCmdLen, nCmdLen;

    if((nCmdLen = strlen(pNext->cmdBuff)))
    {   // something there
        oCmdLen = pCmdIO->cmdEnd-pCmdIO->cmdBuff;
        while(oCmdLen>nCmdLen)
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "\b \b");     // clear the old command
            oCmdLen--;
        }
        while(oCmdLen--)
        {
            (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "\b");
        }
        strcpy(pCmdIO->cmdBuff, pNext->cmdBuff);
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, "\r\n>");
        (*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, pCmdIO->cmdBuff);
        pCmdIO->cmdPnt = pCmdIO->cmdEnd = pCmdIO->cmdBuff+nCmdLen;
        _pCurrCmdN = pNext;
    }
}


static void CmdAddHead(cmdDlList* pL, cmdNode* pN)
{
    if(pL->head == 0)
    { // empty list, first node
        pL->head = pL->tail = pN;
        pN->next = pN->prev = pN;
    }
    else
    {
        pN->next = pL->head;
        pN->prev = pL->tail;
        pL->tail->next = pN;
        pL->head->prev = pN;
        pL->head = pN;
    }
}


static cmdNode* CmdRemoveTail(cmdDlList* pL)
{
    cmdNode* pN;
    if(pL->head == pL->tail)
    {
        pN = pL->head;
        pL->head = pL->tail = 0;
    }
    else
    {
        pN = pL->tail;
        pL->tail = pN->prev;
        pL->tail->next = pL->head;
        pL->head->prev = pL->tail;
    }
    return pN;
}


