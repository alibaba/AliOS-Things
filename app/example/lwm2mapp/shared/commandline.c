/*******************************************************************************
 *
 * Copyright (c) 2013, 2014, 2015 Intel Corporation and others.
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
 *    Fabien Fleutot - Please refer to git log
 *    
 *******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <inttypes.h>
#include "liblwm2m.h"
#include "internals.h"

#include "commandline.h"

#define HELP_COMMAND "help"
#define HELP_DESC    "Type '"HELP_COMMAND" [COMMAND]' for more details on a command."
#define UNKNOWN_CMD_MSG "Unknown command. Type '"HELP_COMMAND"' for help."


static command_desc_t * prv_find_command(command_desc_t * commandArray,
                                         char * buffer,
                                         size_t length)
{
    int i;

    if (length == 0) return NULL;

    i = 0;
    while (commandArray[i].name != NULL
        && (strlen(commandArray[i].name) != length || strncmp(buffer, commandArray[i].name, length)))
    {
        i++;
    }

    if (commandArray[i].name == NULL)
    {
        return NULL;
    }
    else
    {
        return &commandArray[i];
    }
}

static void prv_displayHelp(command_desc_t * commandArray,
                            char * buffer)
{
    command_desc_t * cmdP;
    int length;

    // find end of first argument
    length = 0;
    while (buffer[length] != 0 && !isspace(buffer[length]&0xff))
        length++;

    cmdP = prv_find_command(commandArray, buffer, length);

    if (cmdP == NULL)
    {
        int i;

        lwm2m_log(LOG_DEBUG, HELP_COMMAND"\t"HELP_DESC"\n");

        for (i = 0 ; commandArray[i].name != NULL ; i++)
        {
            lwm2m_log(LOG_DEBUG, "%s\t%s\n", commandArray[i].name, commandArray[i].shortDesc);
        }
    }
    else
    {
        lwm2m_log(LOG_DEBUG, "%s\n", cmdP->longDesc?cmdP->longDesc:cmdP->shortDesc);
    }
}


void handle_command(command_desc_t * commandArray,
                    char * buffer)
{
    command_desc_t * cmdP;
    int length;

    // find end of command name
    length = 0;
    while (buffer[length] != 0 && !isspace(buffer[length]&0xFF))
        length++;

    cmdP = prv_find_command(commandArray, buffer, length);
    if (cmdP != NULL)
    {
        while (buffer[length] != 0 && isspace(buffer[length]&0xFF))
            length++;
        cmdP->callback(buffer + length, cmdP->userData);
    }
    else
    {
        if (!strncmp(buffer, HELP_COMMAND, length))
        {
            while (buffer[length] != 0 && isspace(buffer[length]&0xFF))
                length++;
            prv_displayHelp(commandArray, buffer + length);
        }
        else
        {
            lwm2m_log(LOG_ERR, UNKNOWN_CMD_MSG"\n");
        }
    }
}

static char* prv_end_of_space(char* buffer)
{
    while (isspace(buffer[0]&0xff))
    {
        buffer++;
    }
    return buffer;
}

char* get_end_of_arg(char* buffer)
{
    while (buffer[0] != 0 && !isspace(buffer[0]&0xFF))
    {
        buffer++;
    }
    return buffer;
}

char * get_next_arg(char * buffer, char** end)
{
    // skip arg
    buffer = get_end_of_arg(buffer);
    // skip space
    buffer = prv_end_of_space(buffer);
    if (NULL != end)
    {
        *end = get_end_of_arg(buffer);
    }

    return buffer;
}

int check_end_of_args(char* buffer)
{
    buffer = prv_end_of_space(buffer);

    return (0 == buffer[0]);
}

/**********************************************************
 * Display Functions
 */

static void print_indent(FILE * stream,
                         int num)
{
    int i;

    for ( i = 0 ; i < num ; i++)
        lwm2m_log(LOG_DEBUG, "    ");
}

void output_buffer(FILE * stream,
                   uint8_t * buffer,
                   int length,
                   int indent)
{
    int i;

    if (length == 0) lwm2m_log(LOG_DEBUG, "\n");

    if (buffer == NULL) return;

    i = 0;
    while (i < length)
    {
        uint8_t array[16];
        int j;

        print_indent(stream, indent);
        memcpy(array, buffer+i, 16);
        for (j = 0 ; j < 16 && i+j < length; j++)
        {
            lwm2m_log(LOG_DEBUG, "%02X ", array[j]);
            if (j%4 == 3) lwm2m_log(LOG_DEBUG, " ");
        }
        if (length > 16)
        {
            while (j < 16)
            {
                lwm2m_log(LOG_DEBUG, "   ");
                if (j%4 == 3) lwm2m_log(LOG_DEBUG, " ");
                j++;
            }
        }
        lwm2m_log(LOG_DEBUG, " ");
        for (j = 0 ; j < 16 && i+j < length; j++)
        {
            if (isprint(array[j]))
                lwm2m_log(LOG_DEBUG, "%c", array[j]);
            else
                lwm2m_log(LOG_DEBUG, ".");
        }
        lwm2m_log(LOG_DEBUG, "\n");
        i += 16;
    }
}

void output_tlv(FILE * stream,
                uint8_t * buffer,
                size_t buffer_len,
                int indent)
{
    lwm2m_data_type_t type;
    uint16_t id;
    size_t dataIndex;
    size_t dataLen;
    int length = 0;
    int result;

    while (0 != (result = lwm2m_decode_TLV((uint8_t*)buffer + length, buffer_len - length, &type, &id, &dataIndex, &dataLen)))
    {
        print_indent(stream, indent);
        lwm2m_log(LOG_DEBUG, "{\r\n");
        print_indent(stream, indent+1);
        lwm2m_log(LOG_DEBUG, "ID: %d", id);

        lwm2m_log(LOG_DEBUG, " type: ");
        switch (type)
        {
        case LWM2M_TYPE_OBJECT_INSTANCE:
            lwm2m_log(LOG_DEBUG, "Object Instance");
            break;
        case LWM2M_TYPE_MULTIPLE_RESOURCE:
            lwm2m_log(LOG_DEBUG, "Multiple Instances");
            break;
        case LWM2M_TYPE_OPAQUE:
            lwm2m_log(LOG_DEBUG, "Resource Value");
            break;
        default:
            lwm2m_log(LOG_ERR, "unknown (%d)", (int)type);
            break;
        }
        lwm2m_log(LOG_DEBUG, "\n");

        print_indent(stream, indent+1);
        lwm2m_log(LOG_DEBUG, "{\n");
        if (type == LWM2M_TYPE_OBJECT_INSTANCE || type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        {
            output_tlv(stream, buffer + length + dataIndex, dataLen, indent+1);
        }
        else
        {
            int64_t intValue;
            double floatValue;
            uint8_t tmp;

            print_indent(stream, indent+2);
            lwm2m_log(LOG_DEBUG, "data (%d bytes):\n", dataLen);
            output_buffer(stream, (uint8_t*)buffer + length + dataIndex, dataLen, indent+2);

            tmp = buffer[length + dataIndex + dataLen];
            buffer[length + dataIndex + dataLen] = 0;
            if (0 < sscanf((const char *)buffer + length + dataIndex, "%lld", &intValue))
            {
                print_indent(stream, indent+2);
                lwm2m_log(LOG_DEBUG, "data as Integer: %lld\n", intValue);
            }
            if (0 < sscanf((const char*)buffer + length + dataIndex, "%lg", &floatValue))
            {
                print_indent(stream, indent+2);
                lwm2m_log(LOG_DEBUG, "data as Float: %.16g\n", floatValue);
            }
            buffer[length + dataIndex + dataLen] = tmp;
        }
        print_indent(stream, indent+1);
        lwm2m_log(LOG_DEBUG, "}\r\n");
        length += result;
        print_indent(stream, indent);
        lwm2m_log(LOG_DEBUG, "}\r\n");
    }
}

void output_data(FILE * stream,
                 lwm2m_media_type_t format,
                 uint8_t * data,
                 int dataLength,
                 int indent)
{
    int i;

    print_indent(stream, indent);
    lwm2m_log(LOG_DEBUG, "%d bytes received of type ", dataLength);

    switch (format)
    {
    case LWM2M_CONTENT_TEXT:
        lwm2m_log(LOG_DEBUG, "text/plain:\n");
        output_buffer(stream, data, dataLength, indent);
        break;

    case LWM2M_CONTENT_OPAQUE:
        lwm2m_log(LOG_DEBUG, "application/octet-stream:\n");
        output_buffer(stream, data, dataLength, indent);
        break;

    case LWM2M_CONTENT_TLV:
        lwm2m_log(LOG_DEBUG, "application/vnd.oma.lwm2m+tlv:\n");
        output_tlv(stream, data, dataLength, indent);
        break;

    case LWM2M_CONTENT_JSON:
        lwm2m_log(LOG_DEBUG, "application/vnd.oma.lwm2m+json:\n");
        print_indent(stream, indent);
        for (i = 0 ; i < dataLength ; i++)
        {
            lwm2m_log(LOG_DEBUG, "%c", data[i]);
        }
        lwm2m_log(LOG_DEBUG, "\n");
        break;

    case LWM2M_CONTENT_LINK:
        lwm2m_log(LOG_DEBUG, "application/link-format:\n");
        print_indent(stream, indent);
        for (i = 0 ; i < dataLength ; i++)
        {
            lwm2m_log(LOG_DEBUG, "%c", data[i]);
        }
        lwm2m_log(LOG_DEBUG, "\n");
        break;

    default:
        lwm2m_log(LOG_ERR, "Unknown (%d):\n", format);
        output_buffer(stream, data, dataLength, indent);
        break;
    }
}

void dump_tlv(FILE * stream,
              int size,
              lwm2m_data_t * dataP,
              int indent)
{
    int i;

    for(i= 0 ; i < size ; i++)
    {
        print_indent(stream, indent);
        lwm2m_log(LOG_DEBUG, "{\n");
        print_indent(stream, indent+1);
        lwm2m_log(LOG_DEBUG, "id: %d\n", dataP[i].id);

        print_indent(stream, indent+1);
        lwm2m_log(LOG_DEBUG, "type: ");
        switch (dataP[i].type)
        {
        case LWM2M_TYPE_OBJECT:
            lwm2m_log(LOG_DEBUG, "LWM2M_TYPE_OBJECT\n");
            dump_tlv(stream, dataP[i].value.asChildren.count, dataP[i].value.asChildren.array, indent + 1);
            break;
        case LWM2M_TYPE_OBJECT_INSTANCE:
            lwm2m_log(LOG_DEBUG, "LWM2M_TYPE_OBJECT_INSTANCE\n");
            dump_tlv(stream, dataP[i].value.asChildren.count, dataP[i].value.asChildren.array, indent + 1);
            break;
        case LWM2M_TYPE_MULTIPLE_RESOURCE:
            lwm2m_log(LOG_DEBUG, "LWM2M_TYPE_MULTIPLE_RESOURCE\n");
            dump_tlv(stream, dataP[i].value.asChildren.count, dataP[i].value.asChildren.array, indent + 1);
            break;
        case LWM2M_TYPE_UNDEFINED:
            lwm2m_log(LOG_DEBUG, "LWM2M_TYPE_UNDEFINED\n");
            break;
        case LWM2M_TYPE_STRING:
            lwm2m_log(LOG_DEBUG, "LWM2M_TYPE_STRING\n");
            print_indent(stream, indent + 1);
            lwm2m_log(LOG_DEBUG, "\"%.*s\"\n", (int)dataP[i].value.asBuffer.length, dataP[i].value.asBuffer.buffer);
            break;
        case LWM2M_TYPE_OPAQUE:
            lwm2m_log(LOG_DEBUG, "LWM2M_TYPE_OPAQUE\n");
            output_buffer(stream, dataP[i].value.asBuffer.buffer, dataP[i].value.asBuffer.length, indent + 1);
            break;
        case LWM2M_TYPE_INTEGER:
            lwm2m_log(LOG_DEBUG, "LWM2M_TYPE_INTEGER: ");
            print_indent(stream, indent + 1);
            lwm2m_log(LOG_DEBUG, "%lld", dataP[i].value.asInteger);
            lwm2m_log(LOG_DEBUG, "\n");
            break;
        case LWM2M_TYPE_FLOAT:
            lwm2m_log(LOG_DEBUG, "LWM2M_TYPE_FLOAT: ");
            print_indent(stream, indent + 1);
            lwm2m_log(LOG_DEBUG, "%lld", dataP[i].value.asInteger);
            lwm2m_log(LOG_DEBUG, "\n");
            break;
        case LWM2M_TYPE_BOOLEAN:
            lwm2m_log(LOG_DEBUG, "LWM2M_TYPE_BOOLEAN: ");
            lwm2m_log(LOG_DEBUG, "%s", dataP[i].value.asBoolean ? "true" : "false");
            lwm2m_log(LOG_DEBUG, "\n");
            break;
        case LWM2M_TYPE_OBJECT_LINK:
            lwm2m_log(LOG_DEBUG, "LWM2M_TYPE_OBJECT_LINK\r\n");
            break;
        default:
            lwm2m_log(LOG_ERR, "unknown (%d)\n", (int)dataP[i].type);
            break;
        }
        print_indent(stream, indent);
        lwm2m_log(LOG_DEBUG, "}\r\n");
    }
}

#define CODE_TO_STRING(X)   case X : return #X

static const char* prv_status_to_string(int status)
{
    switch(status)
    {
    CODE_TO_STRING(COAP_NO_ERROR);
    CODE_TO_STRING(COAP_IGNORE);
    CODE_TO_STRING(COAP_201_CREATED);
    CODE_TO_STRING(COAP_202_DELETED);
    CODE_TO_STRING(COAP_204_CHANGED);
    CODE_TO_STRING(COAP_205_CONTENT);
    CODE_TO_STRING(COAP_400_BAD_REQUEST);
    CODE_TO_STRING(COAP_401_UNAUTHORIZED);
    CODE_TO_STRING(COAP_404_NOT_FOUND);
    CODE_TO_STRING(COAP_405_METHOD_NOT_ALLOWED);
    CODE_TO_STRING(COAP_406_NOT_ACCEPTABLE);
    CODE_TO_STRING(COAP_500_INTERNAL_SERVER_ERROR);
    CODE_TO_STRING(COAP_501_NOT_IMPLEMENTED);
    CODE_TO_STRING(COAP_503_SERVICE_UNAVAILABLE);
    default: return "";
    }
}

void print_status(FILE * stream,
                  uint8_t status)
{
    lwm2m_log(LOG_DEBUG, "%d.%02d (%s)", (status&0xE0)>>5, status&0x1F, prv_status_to_string(status));
}

/**********************************************************
* Base64 decoding function
*
* WARNING: Bugged for input strings with length < 4
*
*/

#define PRV_B64_PADDING '='

static uint8_t prv_b64Revert(uint8_t value)
{
    if (value >= 'A' && value <= 'Z')
    {
        return (value - 'A');
    }
    if (value >= 'a' && value <= 'z')
    {
        return (26 + value - 'a');
    }
    if (value >= '0' && value <= '9')
    {
        return (52 + value - '0');
    }
    switch (value)
    {
    case '+':
        return 62;
    case '/':
        return 63;
    default:
        return 0;
    }
}

static void prv_decodeBlock(uint8_t input[4],
                            uint8_t output[3])
{
    uint8_t tmp[4];
    int i;

    memset(output, 0, 3);

    for (i = 0; i < 4; i++)
    {
        tmp[i] = prv_b64Revert(input[i]);
    }

    output[0] = (tmp[0] << 2) | (tmp[1] >> 4);
    output[1] = (tmp[1] << 4) | (tmp[2] >> 2);
    output[2] = (tmp[2] << 6) | tmp[3];
}

static size_t base64_decode(uint8_t * dataP,
                     size_t dataLen,
                     uint8_t ** bufferP)
{
    size_t data_index;
    size_t result_index;
    size_t result_len;
    
    if (dataLen % 4) return 0;
    
    result_len = (dataLen >> 2) * 3;
    *bufferP = (uint8_t *)lwm2m_malloc(result_len);
    if (NULL == *bufferP) return 0;
    memset(*bufferP, 0, result_len);
    
    // remove padding
    while (dataP[dataLen - 1] == PRV_B64_PADDING)
    {
        dataLen--;
    }
    
    data_index = 0;
    result_index = 0;
    while (data_index < dataLen)
    {
        prv_decodeBlock(dataP + data_index, *bufferP + result_index);
        data_index += 4;
        result_index += 3;
    }
    switch (data_index - dataLen)
    {
    case 0:
        break;
    case 2:
    {
        uint8_t tmp[2];

        tmp[0] = prv_b64Revert(dataP[dataLen - 2]);
        tmp[1] = prv_b64Revert(dataP[dataLen - 1]);

        *bufferP[result_index - 3] = (tmp[0] << 2) | (tmp[1] >> 4);
        *bufferP[result_index - 2] = (tmp[1] << 4);
        result_len -= 2;
    }
    break;
    case 3:
    {
        uint8_t tmp[3];

        tmp[0] = prv_b64Revert(dataP[dataLen - 3]);
        tmp[1] = prv_b64Revert(dataP[dataLen - 2]);
        tmp[2] = prv_b64Revert(dataP[dataLen - 1]);

        *bufferP[result_index - 3] = (tmp[0] << 2) | (tmp[1] >> 4);
        *bufferP[result_index - 2] = (tmp[1] << 4) | (tmp[2] >> 2);
        *bufferP[result_index - 1] = (tmp[2] << 6);
        result_len -= 1;
    }
    break;
    default:
        // error
        lwm2m_free(*bufferP);
        *bufferP = NULL;
        result_len = 0;
        break;
    }

    return result_len;
}
