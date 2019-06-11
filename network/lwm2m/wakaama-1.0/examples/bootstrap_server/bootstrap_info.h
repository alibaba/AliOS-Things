/*******************************************************************************
 *
 * Copyright (c) 2015 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial implementation
 *
 *******************************************************************************/

#include <stdio.h>
#include "liblwm2m.h"

typedef enum
{
    BS_DELETE = 0,
    BS_WRITE_SECURITY,
    BS_WRITE_SERVER,
    BS_FINISH
} bs_operation_t;

typedef struct
{
    uint16_t    id;
    char *      uri;
    bool        isBootstrap;
    uint32_t    lifetime;
    uint8_t     securityMode;
    uint8_t *   publicKey;
    uint8_t *   privateKey;
    uint8_t *   serverKey;
} server_info_t;

typedef struct _server_tlv_
{
    struct _server_tlv_ * next;  // matches lwm2m_list_t::next
    uint16_t              id;    // matches lwm2m_list_t::id
    uint8_t *   securityData;
    size_t      securityLen;
    uint8_t *   serverData;
    size_t      serverLen;
} bs_server_tlv_t;

typedef struct _command_
{
    struct _command_ * next;
    bs_operation_t  operation;
    lwm2m_uri_t *   uri;
    uint16_t        serverId;
} bs_command_t;

typedef struct _endpoint_info_
{
    struct _endpoint_info_ * next;
    char *          name;
    bs_command_t *  commandList;
} bs_endpoint_info_t;

typedef struct
{
    bs_server_tlv_t *    serverList;
    bs_endpoint_info_t * endpointList;
} bs_info_t;

bs_info_t * bs_get_info(FILE * fd);
void bs_free_info(bs_info_t * infoP);
