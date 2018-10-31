/*******************************************************************************
  Command Processor System Service Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_command.h

  Summary:
    Command Processor System Service interface definition.

  Description:
    This file contains the interface definition for the Command Processor System
    Service.  It provides a way to interact with the Command Processor subsystem
    to manage the ASCII command requests from the user supported by the system.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014-2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _SYS_CMD_H
#define _SYS_CMD_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "system_config.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: SYS CMD Data Types and Definitions
// *****************************************************************************
// *****************************************************************************

//SYS_CMD_BUFFER_DMA_READY is defined in system_config.h by MHC, this is here in-case MHC is not used
#ifndef SYS_CMD_BUFFER_DMA_READY
    #define SYS_CMD_BUFFER_DMA_READY //Define this for MX
    /* #define SYS_CMD_BUFFER_DMA_READY        __attribute__((coherent)) __attribute__((aligned(4))) //Define this for MZ */
#endif

#ifdef SYS_CMD_REMAP_SYS_CONSOLE_MESSAGE

#undef SYS_CONSOLE_MESSAGE
#define SYS_CONSOLE_MESSAGE(message) SYS_CMD_MESSAGE(message)

#undef SYS_PRINT
#define SYS_PRINT(fmt, ...) SYS_CMD_PRINT(fmt, ##__VA_ARGS__)

#undef SYS_MESSAGE
#define SYS_MESSAGE(message) SYS_CMD_MESSAGE(message)

#undef SYS_CONSOLE_PRINT
#define SYS_CONSOLE_PRINT(fmt, ...) SYS_CMD_PRINT(fmt, ##__VA_ARGS__)

#endif

#ifdef SYS_CMD_REMAP_SYS_DEBUG_MESSAGE

#undef SYS_DEBUG_MESSAGE
#define SYS_DEBUG_MESSAGE(level, message)   do { if((level) <= gblErrLvl) SYS_CMD_MESSAGE(message); } while (0)

#undef SYS_DEBUG_PRINT
#define SYS_DEBUG_PRINT(level, fmt, ...)    do { if((level) <= gblErrLvl) SYS_CMD_PRINT(fmt, ##__VA_ARGS__); } while (0)

#undef SYS_ERROR_PRINT
#define SYS_ERROR_PRINT(level, fmt, ...)    do { if((level) <= gblErrLvl) SYS_CMD_PRINT(fmt, ##__VA_ARGS__); } while (0)

#undef SYS_DEBUG
#define SYS_DEBUG(level,message)    SYS_DEBUG_MESSAGE(level,message)

#undef SYS_ERROR
#define SYS_ERROR(level,fmt, ...)   SYS_ERROR_PRINT(level, fmt, ##__VA_ARGS__)

#endif

#ifndef SYS_COMMAND_INCLUDE_APIS
    #ifndef SYS_CMD_ENABLE

        #ifndef SYS_CMD_MESSAGE
            #define SYS_CMD_MESSAGE(message)
        #endif

        #ifndef SYS_CMD_PRINT
            #define SYS_CMD_PRINT(fmt, ...)
        #endif

    #endif
#endif

#ifndef SYS_CMD_ENABLE
	#define SYS_CMD_READY_TO_READ()
#endif

#ifndef SYS_CMD_DEVICE_MAX_INSTANCES
    #define SYS_CMD_DEVICE_MAX_INSTANCES            1
#endif


// *****************************************************************************
/* SYS CMD Processor Buffer Maximum Length

  Summary:
    Command Processor System Service Command Buffer Maximum Length definition.

  Description:
    This macro defines the maximum length of the command buffer.

  Remarks:
    None.

*/
#define         SYS_CMD_MAX_LENGTH  128


// *****************************************************************************
/* SYS CMD Processor Read Buffer Size

  Summary:
    Command Processor System Service Read Buffer Size definition.

  Description:
    This macro defines the maximum size of the command buffer.

  Remarks:
    None.

*/
#define         SYS_CMD_READ_BUFFER_SIZE 128


// *****************************************************************************
/* SYS CMD Processor Maximum Number of Command Group

  Summary:
    Command Processor System Service Maximum Number of Command Group definitions.

  Description:
    This macro defines the maximum number of command groups.

  Remarks:
    None.

*/
#define         MAX_CMD_GROUP   8


// *****************************************************************************
/* SYS CMD Processor Maximum Number of Command Arguments

  Summary:
    Command Processor System Service Maximum Number of Argument definitions.

  Description:
    This macro defines the maximum number of arguments per command.

  Remarks:
    None.

*/
#define         MAX_CMD_ARGS    15


// *****************************************************************************
/* SYS CMD Processor Command History Depth

  Summary:
    Command Processor System Service Maximum Depth of Command History.

  Description:
    This macro defines the maximum depth of the command history.

  Remarks:
    None.

*/
#define         COMMAND_HISTORY_DEPTH   3


// *****************************************************************************
/* SYS CMD Processor Command Terminal Support Definitions

  Summary:
    Command Processor System Service Terminal Support Definitions.

  Description:
    These definitions are used by the Command Processor System Service to
    support VT100 ASCII terminal.

  Remarks:
    None.

*/
#define         LINE_TERM       "\r\n"          // line terminator
#define         _promptStr      ">"             // prompt string
#define         ESC_SEQ_SIZE    2               // standard VT100 escape sequences


// *****************************************************************************
/*
  Summary:
    Command Callback Function Handle.
	
  Description:
    Command Callback Function Handle.
	
  Remarks:
    None.
*/
typedef void (*SYS_CMD_CallbackFunction) (void *handle);


// *****************************************************************************
/*
  Summary:
    Identifies a particular Command I/O instance.
	
  Description:
    Command I/O Handle. This event handle identifies a registered instance of a 
	Command IO object. Every time the application that tries to access the parameters 
	with respect to a particular event, this event handle is used to refer to that
    event.
	
  Remarks:
    None.
*/
typedef const void* SYS_CMD_HANDLE;


// *****************************************************************************
/*
  Summary:
    Identifies a message function API.
	
  Description:
    Message function API. This handle identifies the interface structure of the 
	message function API within the Command I/O encapsulation.
	
  Remarks:
    None.
*/
typedef void (*SYS_CMD_MSG_FNC)(const void* cmdIoParam, const char* str);


// *****************************************************************************
/*
  Summary:
    Identifies a print function API.
	
  Description:
    Print function API. This handle identifies the interface structure of the 
	print function API within the Command I/O encapsulation.
	
  Remarks:
    None.
*/
typedef void (*SYS_CMD_PRINT_FNC)(const void* cmdIoParam, const char* format, ...);


// *****************************************************************************
/*
  Summary:
    Identifies a single character print function API.
	
  Description:
    Single Character Print function API. This handle identifies the interface 
	structure of single character print function API within the Command I/O 
	encapsulation.
	
  Remarks:
    None.
*/
typedef void (*SYS_CMD_PUTC_FNC)(const void* cmdIoParam, char c);


// *****************************************************************************
/*
  Summary:
    Identifies a data available function API.
	
  Description:
    Ready Status Check function API. This handle identifies the interface structure 
	of the data available function API within the Command IO encapsulation.
	
  Remarks:
    None.
*/
typedef bool (*SYS_CMD_DATA_RDY_FNC)(const void* cmdIoParam);


// *****************************************************************************
/*
  Summary:
    Identifies a get single character function API.
	
  Description:
    Get Single Character function API. This handle identifies the interface structure 
	of the get single character function API within the Command I/O encapsulation.
	
  Remarks:
    None.
*/
typedef char (*SYS_CMD_GETC_FNC)(const void* cmdIoParam);


// *****************************************************************************
/*
  Summary:
    Identifies a read single character function API.
	
  Description:
    Read Single Character function API

    This handle identifies the interface structure of read single character
    function API within the Command I/O encapsulation.
	
  Remarks:
    None.
*/
typedef size_t (*SYS_CMD_READC_FNC)(const void* cmdIoParam);


// *****************************************************************************
/* SYS CMD API structure

  Summary:
    Identifies the Command API structure.

  Description:
    This structure identifies the Command API structure.

  Remarks:
    None.
*/
typedef struct
{
    // Message function API
    SYS_CMD_MSG_FNC msg;
	
    // Print function API
    SYS_CMD_PRINT_FNC print;

    // Put single char function API
    SYS_CMD_PUTC_FNC  putc;

    // Data available API
    SYS_CMD_DATA_RDY_FNC isRdy;

    // Get single data API
    SYS_CMD_GETC_FNC getc;

    // Read single data API
    SYS_CMD_READC_FNC readc;
    
}SYS_CMD_API;


// *****************************************************************************
/* SYS CMD EVENT structure

  Summary:
    Identifies the Command Event Types

  Description:
    This structure identifies the Command Event Types.

  Remarks:
    None.
*/
typedef enum
{
    SYS_CMD_EVENT_WRITE_COMPLETE,

    SYS_CMD_EVENT_READ_COMPLETE,
} SYS_CMD_EVENT;


// *****************************************************************************
/* SYS CMD State Machine States

   Summary
    Defines the various states that can be achieved by a Command instance.

   Description
    This enumeration defines the various states that can be achieved by the
    command operation.

   Remarks:
    None.
*/
typedef enum
{
    SYS_CMD_STATE_DISABLE,

    SYS_CMD_STATE_SETUP_READ,

    SYS_CMD_STATE_WAIT_FOR_READ_DONE,

    SYS_CMD_STATE_PROCESS_FULL_READ
}SYS_CMD_STATE;


// *****************************************************************************
/* SYS CMD Console I/O Parameter

   Summary
    Defines whether the command module is single character or full command read

   Description
    This enumeration defines whether the command module is single character or 
	full command read.

   Remarks:
    None.
*/
typedef enum
{
    SYS_CMD_SINGLE_CHARACTER_READ_CONSOLE_IO_PARAM = 0,
            
    SYS_CMD_FULL_COMMAND_READ_CONSOLE_IO_PARAM = 1,

    SYS_CMD_TELNET_COMMAND_READ_CONSOLE_IO_PARAM = 2
}SYS_CMD_CONSOLE_IO_PARAM;


// *****************************************************************************
/* SYS CMD INIT structure

  Summary:
    Identifies the system command initialize structure.

  Description:
    This structure identifies the system command initialize structure.

  Remarks:
    None.
*/

typedef struct
{
    /* System module initialization */
    SYS_MODULE_INIT moduleInit;

    uint8_t         consoleCmdIOParam;

    SYS_CMD_CONSOLE_IO_PARAM cmdIoType;

} SYS_CMD_INIT;


// *****************************************************************************
/* SYS CMD Command Instance Node Structure

   Summary
    Defines the data structure to store each command instance.

   Description
    This data structure stores all the data relevant to a uniquely entered 
    command instance.  It is a node for a linked list structure to support the
    Command Processor System Service's command history feature

   Remarks:
    None.
*/
typedef struct SYS_CMD_DEVICE_NODE
{
	char*            cmdPnt;
	char*            cmdEnd;
	char             cmdBuff[SYS_CMD_MAX_LENGTH+1] SYS_CMD_BUFFER_DMA_READY;
	const SYS_CMD_API*  pCmdApi;	// Cmd IO APIs
        const void*         cmdIoParam; // channel specific parameter
        SYS_CMD_CONSOLE_IO_PARAM    cmdIoType;
        struct SYS_CMD_DEVICE_NODE* next;
        SYS_CMD_STATE  cmdState;
}SYS_CMD_DEVICE_NODE;


// *****************************************************************************
/* SYS CMD Command List Structure

   Summary
    Defines the list structure to store a list of command instances.

   Description
    This data structure defines he linked list structure to support the
    Command Processor System Service's command history feature

   Remarks:
    None.
*/
typedef struct
{
	int num;
	SYS_CMD_DEVICE_NODE* head;
	SYS_CMD_DEVICE_NODE* tail;
}SYS_CMD_DEVICE_LIST;


// *****************************************************************************
/* SYS CMD Command App Init Structure

   Summary
    Defines the data structure to store each command.

   Description
    This data structure stores all the data relevant to a uniquely entered
    command.  It is a node for a linked list structure to support the command
    history functionality

   Remarks:
    None.
*/
typedef struct
{
        size_t          bytesRead;
        int             seqBytesRead;
        char            seqBuff[ESC_SEQ_SIZE + 1];

        SYS_MODULE_INDEX  moduleIndices[SYS_CMD_DEVICE_MAX_INSTANCES];
        int               moduleInFd;
        int               moduleOutFd;
}SYS_CMD_INIT_DATA;


// *****************************************************************************
// *****************************************************************************
// Section: SYS CMD External Interface Definitions
// *****************************************************************************
// *****************************************************************************

//************************************************************************
/*
  SYS CMD Command Function

  Summary:
    Identifies the command process function API.

  Description:
    Command Process Function API. This handle identifies the interface structure 
	of the command process function API.
	
  Remarks:
    None.
*/
typedef int (*SYS_CMD_FNC)(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv);


// *****************************************************************************
/* SYS CMD Command Descriptor Structure

   Summary
    Defines the descriptor structure for a single command process.

   Description
    This data structure stores the information for a single unique command
    process.

   Remarks:
    None.
*/
typedef struct
{
    const char*     cmdStr;        // string identifying the command
    SYS_CMD_FNC    cmdFnc;         // function to execute for this command
    const char*     cmdDescr;      // simple command description
} SYS_CMD_DESCRIPTOR;              // a simple command descriptor


// *****************************************************************************
/* SYS CMD Command Process Table Structure

   Summary
    Defines the command table structure for the Command Processor
    System Service.

   Description
    This data structure stores is used to store a list of command process.

   Remarks:
    None.
*/
typedef struct
{
    int                     nCmds;          // number of commands available in the table
    const SYS_CMD_DESCRIPTOR*    pCmd;      // pointer to an array of command descriptors
    const char*             cmdGroupName;   // name identifying the commands
    const char*             cmdMenuStr;     // help string
} SYS_CMD_DESCRIPTOR_TABLE;                 // table containing the supported commands


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
    - true  - Indicates success
	- false - Indicates an error occurred

  Remarks:
    This routine should only be called once during system initialization.
*/
bool SYS_CMD_Initialize(const SYS_MODULE_INIT * const init );


// *****************************************************************************
/* Function:
    bool SYS_CMD_ADDGRP(const SYS_CMD_DESCRIPTOR* pCmdTbl, int nCmds,
                            const char* groupName,
                            const char* menuStr)

  Summary:
    Allows clients to add command process to the Command Processor System
    Service.

  Description:
    Client modules may call this function to add command process to the Command
    Processor System Service.

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
    - true  - Indicates success
	- false - Indicates an error occurred

  Remarks:
    None.
*/
bool    SYS_CMD_ADDGRP(const SYS_CMD_DESCRIPTOR* pCmdTbl, int nCmds, const char* groupName, const char* menuStr);

// *****************************************************************************
/* Function:
    bool SYS_CMD_READY_TO_READ( void )

  Summary:
    Allows upper layer application to confirm that the command
    module is ready to accept command input

  Description:
    This function allows upper layer application to confirm that the command
    module is ready to accept command input

  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Returns:
    - true  - Indicates command module is ready
    - false - Indicates command module is not ready

  Remarks:
    None.
*/
#ifdef SYS_CMD_ENABLE
bool    SYS_CMD_READY_TO_READ( void );
#endif

// *****************************************************************************
/* Function:
    bool SYS_CMD_READY_TO_WRITE( void )

  Summary:
    Allows upper layer application to confirm that the command
    module is ready to write output to the Console System Service.

  Description:
    This function allows upper layer application to confirm that the command
    module is ready to write output to the Console System Service.

  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Returns:
    - true  - Indicates command module is ready
    - false - Indicates command module is not ready

  Remarks:
    None.
*/
#ifdef SYS_CMD_ENABLE
bool    SYS_CMD_READY_TO_WRITE( void );
#endif


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
#ifdef SYS_CMD_ENABLE
void SYS_CMD_RegisterCallback(SYS_CMD_CallbackFunction cbFunc, SYS_CMD_EVENT event);
#endif


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
    - true  - Indicates success
	- false - Indicates an error occurred

  Remarks:
    None.
*/
#ifdef SYS_CMD_ENABLE
bool     SYS_CMD_Tasks(void);
#endif

// *****************************************************************************
/* Function:
    void SYS_CMD_MESSAGE (const char* message)

  Summary:
    Outputs a message to the Command Processor System Service console.

  Description:
    This function outputs a message to the Command Processor System Service
    console.

  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
#ifdef SYS_CMD_ENABLE
void    SYS_CMD_MESSAGE(const char* message);
#endif

// *****************************************************************************
/* Function:
    void SYS_CMD_PRINT(const char *format, ...)

  Summary:
    Outputs a printout to the Command Processor System Service console.

  Description:
    This function outputs a printout to the Command Processor System Service
    console.

  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
#ifdef SYS_CMD_ENABLE
void    SYS_CMD_PRINT(const char *format, ...);
#endif

// *****************************************************************************
/* Function:
    SYS_CMD_DEVICE_NODE* SYS_CMDIO_GET_HANDLE(short num)

  Summary:
    Gets the CMDIO Handle base via index.

  Description:
    This function returns the handle for the CMDIO when supplied with an index.

  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    short num

  Returns:
    - SYS_CMD_DEVICE_NODE Handle for the CMDIO - Indicates success
	- NULL - Indicates not successful

  Remarks:
    None.
*/
#ifdef SYS_CMD_ENABLE
SYS_CMD_DEVICE_NODE* SYS_CMDIO_GET_HANDLE(short num);
#endif

// *****************************************************************************
/* Function:
    SYS_CMD_DEVICE_NODE* SYS_CMDIO_ADD(const SYS_CMD_API* opApi, const void* cmdIoParam, 
	                                   const SYS_CMD_CONSOLE_IO_PARAM cmdIoType)

  Summary:
    Adds CMDIO parameters to the Command Processor System Service console.

  Description:
    This function adds CMDIO Parameters to the Command Processor System Service 
	console.

  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
#ifdef SYS_CMD_ENABLE
	SYS_CMD_DEVICE_NODE* SYS_CMDIO_ADD(const SYS_CMD_API* opApi, const void* cmdIoParam, 
	                                   const SYS_CMD_CONSOLE_IO_PARAM cmdIoType);
#endif

// *****************************************************************************
/* Function:
    bool SYS_CMD_DELETE(SYS_CMD_DEVICE_NODE* pDevNode);

  Summary:
    Removes CMDIO parameters from the Command Processor System Service console.

  Description:
    This function removes CMDIO parameters from the Command Processor System Service 
	console.

  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
#ifdef SYS_CMD_ENABLE
bool SYS_CMD_DELETE(SYS_CMD_DEVICE_NODE* pDevNode);
#endif

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif