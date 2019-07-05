/*******************************************************************************
 *
 * Copyright (c) 2014 Bosch Software Innovations GmbH, Germany.
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
 *    Bosch Software Innovations GmbH - Please refer to git log
 *
 *******************************************************************************/
/*
 *  event_handler.c
 *
 *  Callback for value changes.
 *
 *  Created on: 21.01.2015
 *  Author: Achim Kraus
 *  Copyright (c) 2014 Bosch Software Innovations GmbH, Germany. All rights reserved.
 */
#include "lwm2m_api.h"
#include "lwm2mclient.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef OSAL_RHINO
#include "aos/kernel.h"
#endif

#ifdef LWM2M_EMBEDDED_MODE

static void prv_value_change(void* context,
                             const char* uriPath,
                             const char * value,
                             size_t valueLength)
{
    lwm2m_uri_t uri;
    if (lwm2m_string_to_uri(uriPath, strlen(uriPath), &uri))
    {
        handle_value_changed(context, &uri, value, valueLength);
    }
}

void init_value_change(lwm2m_context_t * lwm2m)
{
    system_setValueChangedHandler(lwm2m, prv_value_change);
}

#else

void init_value_change(lwm2m_context_t * lwm2m)
{
}

void system_reboot()
{
#ifdef OSAL_RHINO
    aos_reboot();
#else
    exit(1);
#endif
}

#endif
