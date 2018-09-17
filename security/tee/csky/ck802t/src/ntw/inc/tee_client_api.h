/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _TEE_CLIENT_API_H_
#define _TEE_CLIENT_API_H_

#include "tee_types.h"

/*
 * Constants
 */
#define TEEC_CONFIG_SHAREDMEM_MAX_SIZE (0x000A000) /* 40k */

/*
 * API Error Codes
 */
#define TEEC_SUCCESS (0x00000000)
#define TEEC_ERROR_GENERIC (0xFFFF0000)
#define TEEC_ERROR_ACCESS_DENIED (0xFFFF0001)
#define TEEC_ERROR_CANCEL (0xFFFF0002)
#define TEEC_ERROR_ACCESS_CONFLICT (0xFFFF0003)
#define TEEC_ERROR_EXCESS_DATA (0xFFFF0004)
#define TEEC_ERROR_BAD_FORMAT (0xFFFF0005)
#define TEEC_ERROR_BAD_PARAMETERS (0xFFFF0006)
#define TEEC_ERROR_BAD_STATE (0xFFFF0007)
#define TEEC_ERROR_ITEM_NOT_FOUND (0xFFFF0008)
#define TEEC_ERROR_NOT_IMPLEMENTED (0xFFFF0009)
#define TEEC_ERROR_NOT_SUPPORTED (0xFFFF000A)
#define TEEC_ERROR_NO_DATA (0xFFFF000B)
#define TEEC_ERROR_OUT_OF_MEMORY (0xFFFF000C)
#define TEEC_ERROR_BUSY (0xFFFF000D)
#define TEEC_ERROR_COMMUNICATION (0xFFFF000E)
#define TEEC_ERROR_SECURITY (0xFFFF000F)
#define TEEC_ERROR_SHORT_BUFFER (0xFFFF0010)
#define TEEC_ERROR_TARGET_DEAD (0xFFFF3024)

/*
 * Return Code for returnOrigins
 */
#define TEEC_ORIGIN_API (0x00000001)
#define TEEC_ORIGIN_COMMS (0x00000002)
#define TEEC_ORIGIN_TEE (0x00000003)
#define TEEC_ORIGIN_TRUSTED_APP (0x00000004)

/*
 * Parameter Types
 */
#define TEEC_NONE (0x00000000)
#define TEEC_VALUE_INPUT (0x00000001)
#define TEEC_VALUE_OUTPUT (0x00000002)
#define TEEC_VALUE_INOUT (0x00000003)
#define TEEC_MEMREF_TEMP_INPUT (0x00000005)
#define TEEC_MEMREF_TEMP_OUTPUT (0x00000006)
#define TEEC_MEMREF_TEMP_INOUT (0x00000007)
#define TEEC_MEMREF_WHOLE (0x0000000C)
#define TEEC_MEMREF_PARTIAL_INPUT (0x0000000D)
#define TEEC_MEMREF_PARTIAL_OUTPUT (0x0000000E)
#define TEEC_MEMREF_PARTIAL_INOUT (0x0000000F)

/*
 * Session Login Methods
 */
#if 0
#define TEEC_LOGIN_PUBLIC (0x00000000)
#define TEEC_LOGIN_USER (0x00000001)
#define TEEC_LOGIN_GROUP (0x00000002)
#define TEEC_LOGIN_APPLICATION (0x00000004)
#define TEEC_LOGIN_USER_APPLICATION (0x00000005)
#define TEEC_LOGIN_GROUP_APPLICATION (0x00000006)
#endif

#define TEEC_PARAM_TYPES(t0, t1, t2, t3) \
    ((t0) | ((t1) << 4) | ((t2) << 8) | ((t3) << 12))

/*
 * Shared Memory Control Flags
 */
#define TEEC_MEM_INPUT (0x00000001)
#define TEEC_MEM_OUTPUT (0x00000002)

typedef uint32_t TEEC_Result;
typedef void *   TEEC_Context_Handle;
typedef void *   TEEC_Session_Handle;
typedef void *   TEEC_Operation_Handle;
typedef void *   TEEC_SharedMemory_Handle;

typedef struct
{
    uint32_t timeLow;
    uint16_t timeMid;
    uint16_t timeHiAndVersion;
    uint8_t  clockSeqAndNode[8];
} TEEC_UUID;

typedef struct
{
    TEEC_Context_Handle imp;
} TEEC_Context;

typedef struct
{
    TEEC_Session_Handle imp;
} TEEC_Session;

typedef struct
{
    void *                   buffer;
    size_t                   size;
    uint32_t                 flags;
    TEEC_SharedMemory_Handle imp;
} TEEC_SharedMemory;

typedef struct
{
    void * buffer;
    size_t size;
} TEEC_TempMemoryReference;

typedef struct
{
    TEEC_SharedMemory *parent;
    size_t             size;
    size_t             offset;
} TEEC_RegisteredMemoryReference;

typedef struct
{
    uint32_t a;
    uint32_t b;
} TEEC_Value;

typedef union
{
    TEEC_TempMemoryReference       tmpref;
    TEEC_RegisteredMemoryReference memref;
    TEEC_Value                     value;
} TEEC_Parameter;

typedef struct
{
    uint32_t              started;
    uint32_t              paramTypes;
    TEEC_Parameter        params[4];
    TEEC_Operation_Handle imp;
} TEEC_Operation;

// FIXME, fake api to init/de-init osa and comm
#if 1
int TEEC_Init(void);
#endif

/**
 * Register a block of existing memory as a shared block
 * within the scope of the specified context.
 *
 * @context[in]     poniter to an initialized TEE context.
 * @sharedMem[out]  pointer to the shared memory structure to register.
 */
TEEC_Result TEEC_RegisterSharedMemory(TEEC_SharedMemory *sharedMem);

/**
 * Allocate shared memory for TEE.
 *
 * @context[in]     poniter to an initialized TEE context.
 * @sharedMem[out]  Pointer to the allocated shared memory.
 */
TEEC_Result TEEC_AllocateSharedMemory(TEEC_SharedMemory *sharedMem);

/**
 * Deregister or deallocate a previously initialized block of shared memory.
 *
 * @sharedMem[in]  Pointer to the shared memory to be or deregistered or
 * dealloacted.
 */
void TEEC_ReleaseSharedMemory(TEEC_SharedMemory *sharedMem);

/**
 * Open a new session with the specified trusted application.
 *
 * @context[in]              Pointer to an initialized TEE context.
 * @session[out]             Pointer to a session structure to open.
 * @destination[in]          Pointer to the UUID of the destination trust
 * application
 * @connectionMethod[in]     The connection method to use.
 * @connectionData[in]       Any necessary data required to support the
 * connection method chosen.
 * @operation[in]            An operation structure to use in the session. May
 * be set to NULL to signify no operation structure needed.
 * @returnOrigin[out]        Pointer to a variable which will contain the return
 * origin. This field may be NULL if the return origin is not needed.
 */
TEEC_Result TEEC_OpenSession(TEEC_Session *   session,
                             const TEEC_UUID *destination,
                             TEEC_Operation * operation);

/**
 * Close s session which has been opened with a trusted application.
 *
 * @session[in] The opened session to close.
 */
void TEEC_CloseSession(TEEC_Session *session);

/**
 * Execute a command in the specified trusted application.
 *
 * @session[in]          The open Session in which the command will be invoked.
 * @commandID[in]        Identifier of the command in the trusted application to
 * invoke.
 * @operation[in/out]    An operation structure to use in the invoke command.
 *                       May be set to NULL to signify no operation structure
 * needed.
 * @returnOrigin[out]    A pointer to a variable which will contain the return
 * origin. This field may be NULL if the return origin is not needed.
 */
TEEC_Result TEEC_InvokeCommand(TEEC_Session *session, uint32_t commandID,
                               TEEC_Operation *operation);

/**
 * Execute a cancel request in the specified trusted application.
 */
#if 0
void TEEC_RequestCancellation(TEEC_Operation *operation);
#endif

#endif /* _TEE_CLIENT_API_H_ */
