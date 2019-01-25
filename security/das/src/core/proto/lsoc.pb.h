/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.9.1 at Mon Nov  5 09:56:33 2018. */

#ifndef PB_LSOC_PB_H_INCLUDED
#define PB_LSOC_PB_H_INCLUDED
#include "pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _Domain {
    Domain_APP = 0,
    Domain_SYS = 1,
    Domain_NET = 2
} Domain;
#define _Domain_MIN Domain_APP
#define _Domain_MAX Domain_NET
#define _Domain_ARRAYSIZE ((Domain)(Domain_NET+1))

typedef enum _Code {
    Code_INF = 0,
    Code_ATS = 1,
    Code_MES = 2,
    Code_UPT = 3,
    Code_ACC = 4
} Code;
#define _Code_MIN Code_INF
#define _Code_MAX Code_ACC
#define _Code_ARRAYSIZE ((Code)(Code_ACC+1))

typedef enum _AtsCmdData_Type {
    AtsCmdData_Type_FULL = 0,
    AtsCmdData_Type_FILE = 1,
    AtsCmdData_Type_LIBRARY = 2,
    AtsCmdData_Type_NET = 3
} AtsCmdData_Type;
#define _AtsCmdData_Type_MIN AtsCmdData_Type_FULL
#define _AtsCmdData_Type_MAX AtsCmdData_Type_NET
#define _AtsCmdData_Type_ARRAYSIZE ((AtsCmdData_Type)(AtsCmdData_Type_NET+1))

typedef enum _MesCmdData_Type {
    MesCmdData_Type_FULL = 0,
    MesCmdData_Type_FILE = 1,
    MesCmdData_Type_NET = 3
} MesCmdData_Type;
#define _MesCmdData_Type_MIN MesCmdData_Type_FULL
#define _MesCmdData_Type_MAX MesCmdData_Type_NET
#define _MesCmdData_Type_ARRAYSIZE ((MesCmdData_Type)(MesCmdData_Type_NET+1))

/* Struct definitions */
typedef struct _AtsCmdData {
    AtsCmdData_Type type;
/* @@protoc_insertion_point(struct:AtsCmdData) */
} AtsCmdData;

typedef struct _AtsDevData {
    uint64_t cmd;
/* @@protoc_insertion_point(struct:AtsDevData) */
} AtsDevData;

typedef struct _AtsEndData {
    uint64_t cmd;
    uint32_t total;
    char sum[96];
/* @@protoc_insertion_point(struct:AtsEndData) */
} AtsEndData;

typedef struct _AtsFilData {
    uint64_t cmd;
    char path[96];
    char attribute[96];
    char signature[96];
/* @@protoc_insertion_point(struct:AtsFilData) */
} AtsFilData;

typedef struct _AtsNetData {
    uint64_t cmd;
/* @@protoc_insertion_point(struct:AtsNetData) */
} AtsNetData;

typedef struct _AtsSysData {
    uint64_t cmd;
/* @@protoc_insertion_point(struct:AtsSysData) */
} AtsSysData;

typedef struct _InfData {
    char version[96];
    char device[96];
    char state[96];
/* @@protoc_insertion_point(struct:InfData) */
} InfData;

typedef struct _MesCmdData {
    MesCmdData_Type type;
/* @@protoc_insertion_point(struct:MesCmdData) */
} MesCmdData;

typedef struct _MesEndData {
    uint64_t cmd;
    uint32_t total;
/* @@protoc_insertion_point(struct:MesEndData) */
} MesEndData;

typedef struct _MesFilData {
    uint64_t cmd;
    char sum[96];
    char mac[96];
/* @@protoc_insertion_point(struct:MesFilData) */
} MesFilData;

typedef struct _MesNetData {
    uint64_t cmd;
    char sum[96];
    char mac[96];
/* @@protoc_insertion_point(struct:MesNetData) */
} MesNetData;

typedef struct _Message {
    Domain domain;
    Code code;
    uint64_t timestamp;
    pb_size_t which_data;
    union {
        InfData infData;
        AtsCmdData atsCmdData;
        AtsEndData atsEndData;
        AtsFilData atsFilData;
        MesCmdData mesCmdData;
        MesEndData mesEndData;
        MesFilData mesFilData;
    } data;
/* @@protoc_insertion_point(struct:Message) */
} Message;

/* Default values for struct fields */

/* Initializer values for message structs */
#define InfData_init_default                     {"", "", ""}
#define AtsCmdData_init_default                  {_AtsCmdData_Type_MIN}
#define AtsFilData_init_default                  {0, "", "", ""}
#define AtsNetData_init_default                  {0}
#define AtsSysData_init_default                  {0}
#define AtsDevData_init_default                  {0}
#define AtsEndData_init_default                  {0, 0, ""}
#define MesCmdData_init_default                  {_MesCmdData_Type_MIN}
#define MesFilData_init_default                  {0, "", ""}
#define MesNetData_init_default                  {0, "", ""}
#define MesEndData_init_default                  {0, 0}
#define Message_init_default                     {_Domain_MIN, _Code_MIN, 0, 0, {InfData_init_default}}
#define InfData_init_zero                        {"", "", ""}
#define AtsCmdData_init_zero                     {_AtsCmdData_Type_MIN}
#define AtsFilData_init_zero                     {0, "", "", ""}
#define AtsNetData_init_zero                     {0}
#define AtsSysData_init_zero                     {0}
#define AtsDevData_init_zero                     {0}
#define AtsEndData_init_zero                     {0, 0, ""}
#define MesCmdData_init_zero                     {_MesCmdData_Type_MIN}
#define MesFilData_init_zero                     {0, "", ""}
#define MesNetData_init_zero                     {0, "", ""}
#define MesEndData_init_zero                     {0, 0}
#define Message_init_zero                        {_Domain_MIN, _Code_MIN, 0, 0, {InfData_init_zero}}

/* Field tags (for use in manual encoding/decoding) */
#define AtsCmdData_type_tag                      1
#define AtsDevData_cmd_tag                       1
#define AtsEndData_cmd_tag                       1
#define AtsEndData_total_tag                     2
#define AtsEndData_sum_tag                       3
#define AtsFilData_cmd_tag                       1
#define AtsFilData_path_tag                      2
#define AtsFilData_attribute_tag                 3
#define AtsFilData_checksum_tag                  4
#define AtsNetData_cmd_tag                       1
#define AtsSysData_cmd_tag                       1
#define InfData_version_tag                      1
#define InfData_device_tag                       2
#define InfData_state_tag                        3
#define MesCmdData_type_tag                      1
#define MesEndData_cmd_tag                       1
#define MesEndData_total_tag                     2
#define MesFilData_cmd_tag                       1
#define MesFilData_sum_tag                       2
#define MesFilData_mac_tag                       3
#define MesNetData_cmd_tag                       1
#define MesNetData_sum_tag                       2
#define MesNetData_mac_tag                       3
#define Message_infData_tag                      4
#define Message_atsCmdData_tag                   5
#define Message_atsEndData_tag                   6
#define Message_atsFilData_tag                   7
#define Message_mesCmdData_tag                   11
#define Message_mesEndData_tag                   12
#define Message_mesFilData_tag                   13
#define Message_domain_tag                       1
#define Message_code_tag                         2
#define Message_timestamp_tag                    3

/* Struct field encoding specification for nanopb */
extern const pb_field_t InfData_fields[4];
extern const pb_field_t AtsCmdData_fields[2];
extern const pb_field_t AtsFilData_fields[5];
extern const pb_field_t AtsNetData_fields[2];
extern const pb_field_t AtsSysData_fields[2];
extern const pb_field_t AtsDevData_fields[2];
extern const pb_field_t AtsEndData_fields[4];
extern const pb_field_t MesCmdData_fields[2];
extern const pb_field_t MesFilData_fields[4];
extern const pb_field_t MesNetData_fields[4];
extern const pb_field_t MesEndData_fields[3];
extern const pb_field_t Message_fields[11];

/* Maximum encoded size of messages (where known) */
#define InfData_size                             294
#define AtsCmdData_size                          2
#define AtsFilData_size                          305
#define AtsNetData_size                          11
#define AtsSysData_size                          11
#define AtsDevData_size                          11
#define AtsEndData_size                          115
#define MesCmdData_size                          2
#define MesFilData_size                          207
#define MesNetData_size                          207
#define MesEndData_size                          17
#define Message_size                             323

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define LSOC_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
