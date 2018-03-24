/*
 *  LibNoPoll: A websocket library
 *  Copyright (C) 2013 Advanced Software Production Line, S.L.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free
 *  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307 USA
 *  
 *  You may find a copy of the license under this software is released
 *  at COPYING file. This is LGPL software: you are welcome to develop
 *  proprietary applications using this library without any royalty or
 *  fee but returning back any change, improvement or addition in the
 *  form of source code, project image, documentation patches, etc.
 *
 *  For commercial support on build Websocket enabled solutions
 *  contact us:
 *          
 *      Postal address:
 *         Advanced Software Production Line, S.L.
 *         Edificio Alius A, Oficina 102,
 *         C/ Antonio Suarez Nº 10,
 *         Alcalá de Henares 28802 Madrid
 *         Spain
 *
 *      Email address:
 *         info@aspl.es - http://www.aspl.es/nopoll
 */
#ifndef __NOPOLL_MSG_H__
#define __NOPOLL_MSG_H__

#include <nopoll.h>

BEGIN_C_DECLS

const unsigned char *  nopoll_msg_get_payload (noPollMsg * msg);

int          nopoll_msg_get_payload_size (noPollMsg * msg);

noPollMsg  * nopoll_msg_new (void);

nopoll_bool  nopoll_msg_ref (noPollMsg * msg);

int          nopoll_msg_ref_count (noPollMsg * msg);

nopoll_bool  nopoll_msg_is_final (noPollMsg * msg);

nopoll_bool  nopoll_msg_is_fragment (noPollMsg * msg);

noPollOpCode nopoll_msg_opcode (noPollMsg * msg);

noPollMsg  * nopoll_msg_join (noPollMsg * msg, noPollMsg * msg2);

void         nopoll_msg_unref (noPollMsg * msg);

END_C_DECLS

#endif
