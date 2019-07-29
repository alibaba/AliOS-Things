/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_DIAGS_H
#define UR_DIAGS_H

#include "core/mesh_forwarder.h"
#include "utilities/message.h"

ur_error_t handle_diags_command(message_t *message, bool dest_reached);

ur_error_t send_trace_route_request(network_context_t *network,
                                    ur_addr_t *dest);

#endif  /* UR_DIAGS_H */
