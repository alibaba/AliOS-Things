/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: session.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "session.pb-c.h"
void   session_data__init
                     (SessionData         *message)
{
  static const SessionData init_value = SESSION_DATA__INIT;
  *message = init_value;
}
size_t session_data__get_packed_size
                     (const SessionData *message)
{
  assert(message->base.descriptor == &session_data__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t session_data__pack
                     (const SessionData *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &session_data__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t session_data__pack_to_buffer
                     (const SessionData *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &session_data__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
SessionData *
       session_data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (SessionData *)
     protobuf_c_message_unpack (&session_data__descriptor,
                                allocator, len, data);
}
void   session_data__free_unpacked
                     (SessionData *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &session_data__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor session_data__field_descriptors[3] =
{
  {
    "sec_ver",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_ENUM,
    0,   /* quantifier_offset */
    offsetof(SessionData, sec_ver),
    &sec_scheme_version__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "sec0",
    10,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(SessionData, proto_case),
    offsetof(SessionData, sec0),
    &sec0_payload__descriptor,
    NULL,
    0 | PROTOBUF_C_FIELD_FLAG_ONEOF,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "sec1",
    11,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(SessionData, proto_case),
    offsetof(SessionData, sec1),
    &sec1_payload__descriptor,
    NULL,
    0 | PROTOBUF_C_FIELD_FLAG_ONEOF,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned session_data__field_indices_by_name[] = {
  1,   /* field[1] = sec0 */
  2,   /* field[2] = sec1 */
  0,   /* field[0] = sec_ver */
};
static const ProtobufCIntRange session_data__number_ranges[2 + 1] =
{
  { 2, 0 },
  { 10, 1 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor session_data__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "SessionData",
  "SessionData",
  "SessionData",
  "",
  sizeof(SessionData),
  3,
  session_data__field_descriptors,
  session_data__field_indices_by_name,
  2,  session_data__number_ranges,
  (ProtobufCMessageInit) session_data__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCEnumValue sec_scheme_version__enum_values_by_number[2] =
{
  { "SecScheme0", "SEC_SCHEME_VERSION__SecScheme0", 0 },
  { "SecScheme1", "SEC_SCHEME_VERSION__SecScheme1", 1 },
};
static const ProtobufCIntRange sec_scheme_version__value_ranges[] = {
{0, 0},{0, 2}
};
static const ProtobufCEnumValueIndex sec_scheme_version__enum_values_by_name[2] =
{
  { "SecScheme0", 0 },
  { "SecScheme1", 1 },
};
const ProtobufCEnumDescriptor sec_scheme_version__descriptor =
{
  PROTOBUF_C__ENUM_DESCRIPTOR_MAGIC,
  "SecSchemeVersion",
  "SecSchemeVersion",
  "SecSchemeVersion",
  "",
  2,
  sec_scheme_version__enum_values_by_number,
  2,
  sec_scheme_version__enum_values_by_name,
  1,
  sec_scheme_version__value_ranges,
  NULL,NULL,NULL,NULL   /* reserved[1234] */
};
