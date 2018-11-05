/*******************************************************************************
  MIIM Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim_local.h

  Summary:
    MIIM driver local declarations and definitions.

  Description:
    This file contains the MIIM driver's local declarations and definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _DRV_MIIM_LOCAL_H
#define _DRV_MIIM_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "system_config.h"

#include "osal/osal.h"

#include "../drv_miim.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// maximum register value that can be accessed.
// This driver uses standard MMIM routines, and has 5 bits for register index
#define DRV_MIIM_MAX_REG_INDEX_VALUE        0x1f

// maximum PHY address value that can be used.
// This driver uses standard MMIM routines, and has 5 bits for PHY address
#define DRV_MIIM_MAX_ADDRESS_VALUE        0x1f


// debugging
#define DRV_MIIM_DEBUG_MASK_BASIC        (0x0001)

// enable IPV4 debugging levels
#define DRV_MIIM_DEBUG_LEVEL  (DRV_MIIM_DEBUG_MASK_BASIC)



// private object definitions
//

typedef struct _TAG_SGL_LIST_NODE
{
	struct _TAG_SGL_LIST_NODE*		next;		// next node in list
    void*                           data[0];    // generic payload    
}SGL_LIST_NODE;	// generic linked list node definition


typedef struct
{
	SGL_LIST_NODE*	head;	// list head
	SGL_LIST_NODE*	tail;
    int             nNodes; // number of nodes in the list

}SINGLE_LIST;	// single linked list


#if (DRV_MIIM_CLIENT_OP_PROTECTION)
// client identification structure
typedef struct _tag_DRV_MIIM_CLI_OP_STAMP
{
    struct _tag_DRV_MIIM_CLI_OP_STAMP* next;       // safe cast to single list node
    struct _tag_DRV_MIIM_OP_DCPT*   pOpDcpt;    // descriptor describing the operation
    struct _tag_DRV_MIIM_CLIENT_DCPT* pClientDcpt;   // owner client 
    uint32_t                        tStamp;     // time at which operation was started
}DRV_MIIM_CLI_OP_STAMP;

#endif  // (DRV_MIIM_CLIENT_OP_PROTECTION)
// MIIM operation types
typedef enum
{
    DRV_MIIM_OP_NONE    = 0,        // no operation started
    DRV_MIIM_OP_READ,               // read operation 
    DRV_MIIM_OP_WRITE,              // write operation 
    DRV_MIIM_OP_SCAN,               // scan operation 
}DRV_MIIM_OP_TYPE;


// MIIM operation transfer status
typedef enum
{
    DRV_MIIM_TXFER_NONE = 0,      // operation not started
    DRV_MIIM_TXFER_START,         // command to start the operation
    DRV_MIIM_TXFER_RDWR_WAIT_COMPLETE, // read/write operation waiting to complete
    DRV_MIIM_TXFER_SCAN_STALE,    // scan operation ongoing, stale result
    DRV_MIIM_TXFER_REPORT_STATE,  // from this state on reports are needed
    DRV_MIIM_TXFER_SCAN_VALID = DRV_MIIM_TXFER_REPORT_STATE,    // scan operation ongoing, fresh result available
    DRV_MIIM_TXFER_DONE,          // operation completed successfully
    DRV_MIIM_TXFER_TMO_START,     // operation timed out on start
    DRV_MIIM_TXFER_TMO_END,       // operation timed out on end
    DRV_MIIM_TXFER_ERROR,         // unexpected error has occurred

} DRV_MIIM_TXFER_STAT;

// MIIM operation report actions
typedef enum
{
    DRV_MIIM_REPORT_ACT_NONE = 0,   // no action is required
    DRV_MIIM_REPORT_ACT_DELETE,     // operation needs to be deleted
    DRV_MIIM_REPORT_ACT_WAIT_COMPLETE, // operation needs to be stored in the complete list
} DRV_MIIM_REPORT_ACT;

//  queue to which an operation may belong
typedef enum
{
    DRV_MIIM_QTYPE_NONE = 0,        // invalid
    DRV_MIIM_QTYPE_FREE,            // free, available
    DRV_MIIM_QTYPE_BUSY,            // busy, scheduled
    DRV_MIIM_QTYPE_COMPLETE,        // completed but waiting for client acknowledge
}DRV_MIIM_QUEUE_TYPE;

// MIIM schedule descriptor
typedef struct
{
    // input data
    unsigned int                    regIx;      // register for the operation
    unsigned int                    phyAdd;     // PHY address to use for the operation
    uint8_t                         opType;     // DRV_MIIM_OP_TYPE: type of operation to schedule
    uint8_t                         opFlags;    // flags associated with the operation
    uint16_t                        opData;     // initData
    // output data
    struct _tag_DRV_MIIM_OBJ_STRUCT* pObj;      // object to which this operation belongs
    struct _tag_DRV_MIIM_OP_DCPT*   pOpDcpt;    // operation that's been scheduled 
    DRV_MIIM_OPERATION_HANDLE       opHandle;   // handle to be returned to the client
}DRV_MIIM_SCHEDULE_DATA;

// MIIM operation descriptor
typedef struct _tag_DRV_MIIM_OP_DCPT
{
    struct _tag_DRV_MIIM_OP_DCPT*   next;       // safe cast to a SGL_LIST_NODE
    uint8_t                         opType;     // DRV_MIIM_OP_TYPE: type of operation in progress
    uint8_t                         regIx;      // register for the operation
    uint8_t                         phyAdd;     // PHY address to use for the operation
    uint8_t                         opFlags;    // DRV_MIIM_OPERATION_FLAGS: flags associated with the operation
    uint16_t                        opData;     // associated data
    uint8_t                         opStat;     // DRV_MIIM_TXFER_STAT value: current status
    uint8_t                         qType;      // DRV_MIIM_QUEUE_TYPE value: current queue the operation is in
    struct _tag_DRV_MIIM_CLIENT_DCPT* pOwner;    // owner of this operation 
#if (DRV_MIIM_CLIENT_OP_PROTECTION)
    DRV_MIIM_CLI_OP_STAMP*          pCliStamp;  // client stamp to match
    uint32_t                        tStamp;     // time at which operation was started
#endif  // (DRV_MIIM_CLIENT_OP_PROTECTION)
}DRV_MIIM_OP_DCPT;


typedef struct _tag_DRV_MIIM_CLIENT_DCPT
{
    uint16_t                            clientInUse;    // True if in use
    uint16_t                            clientIx;       // client number
    struct _tag_DRV_MIIM_OBJ_STRUCT*    parentObj;      // parent object to which this client belongs
    DRV_MIIM_OPERATION_CALLBACK         cbackHandler;   // client notification when operation is done
    DRV_MIIM_CLIENT_STATUS              cliStatus;      // Client Status
}DRV_MIIM_CLIENT_DCPT;

//  MIIM Object Flags
//  Currently 16 bits only
typedef enum
{
    DRV_MIIM_OBJ_FLAG_NONE          = 0x00,     // default, no flag set
    DRV_MIIM_OBJ_FLAG_IN_USE        = 0x01,     // object properly initialized and in use
    DRV_MIIM_OBJ_FLAG_SETUP_DONE    = 0x02,     // at least one client called the setup routine
    DRV_MIIM_OBJ_FLAG_EXCLUSIVE     = 0x04,     // some client opened in exclusive mode
    DRV_MIIM_OBJ_FLAG_IS_SCANNING   = 0x08,     // object is in scan mode

    // add other flags here

}DRV_MIIM_OBJ_FLAGS;

// MIIM Driver Hardware Instance Object
// TODO aa: it'd be nice to allow clients to use their own ethphyId? But not requesting that all clients perform set up...
//          So the MIIM object has a default ethphyId, that's used when a client doesn't specify its own...
//          This could lead to further optimizations, allowing parallel operations when ID's are different!
typedef struct _tag_DRV_MIIM_OBJ_STRUCT
{
    OSAL_SEM_HANDLE_TYPE objSem;  // synchronization object: protection for access to the IGMP
                                          // lists between user threads and task thread
    uint16_t            objFlags;       // DRV_MIIM_OBJ_FLAGS: object associated flags
    uint16_t            numClients;     // Number of active clients
    SYS_STATUS          objStatus;      // Status of module
    SYS_MODULE_INDEX    iModule;        // Module instance number
    uintptr_t       ethphyId;       // The peripheral Id associated with the object
    DRV_MIIM_CLIENT_DCPT objClients[DRV_MIIM_INSTANCE_CLIENTS]; // array of clients
    DRV_MIIM_OP_DCPT    opPool[DRV_MIIM_INSTANCE_OPERATIONS];      // pool of operations
    SINGLE_LIST         freeOpList;     // available operations
    SINGLE_LIST         busyOpList;     // scheduled operations
    SINGLE_LIST         completeOpList; // completed operations that need to be polled by the clients
#if (DRV_MIIM_CLIENT_OP_PROTECTION)
    DRV_MIIM_CLI_OP_STAMP stampPool[DRV_MIIM_INSTANCE_OPERATIONS];      // pool of stamps, matching operations
    SINGLE_LIST         freeStampList;   // list with available client stamps
    SINGLE_LIST         busyStampList;   // list with scheduled client stamps

#endif  // (DRV_MIIM_CLIENT_OP_PROTECTION)

}DRV_MIIM_OBJ;


// helpers
//
void  Helper_SingleListInitialize(SINGLE_LIST* pL);



// adds node to tail
void  Helper_SingleListTailAdd(SINGLE_LIST* pL, SGL_LIST_NODE* pN);


// removes the head node
SGL_LIST_NODE*  Helper_SingleListHeadRemove(SINGLE_LIST* pL);

// removes a node anywhere in the list
// Note: this is lengthy!
SGL_LIST_NODE*  Helper_SingleListNodeRemove(SINGLE_LIST* pL, SGL_LIST_NODE* pN);

#endif //#ifndef _DRV_MIIM_LOCAL_H

/*******************************************************************************
 End of File
*/

