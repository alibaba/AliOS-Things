/*******************************************************************************
 *
 * Copyright (c) 2013 Intel Corporation and others.
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
 *    David Navarro, Intel Corporation - initial API and implementation
 *    
 *******************************************************************************/

#include <stdio.h>

#define COMMAND_END_LIST {NULL, NULL, NULL, NULL, NULL}

typedef void (*command_handler_t) (char * args, void * user_data);

typedef struct
{
    char *            name;
    char *            shortDesc;
    char *            longDesc;
    command_handler_t callback;
    void *            userData;
} command_desc_t;


void handle_command(command_desc_t * commandArray, char * buffer);
char* get_end_of_arg(char* buffer);
char * get_next_arg(char * buffer, char **end);
int check_end_of_args(char* buffer);

void output_buffer(FILE * stream, uint8_t * buffer, int length, int indent);
void output_tlv(FILE * stream, uint8_t * buffer, size_t buffer_len, int indent);
void dump_tlv(FILE * stream, int size, lwm2m_data_t * dataP, int indent);
void output_data(FILE * stream, lwm2m_media_type_t format, uint8_t * buffer, int length, int indent);
void print_status(FILE * stream, uint8_t status);

size_t base64_decode(uint8_t * dataP, size_t dataLen, uint8_t ** bufferP);