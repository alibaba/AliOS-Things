/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _BT_DIS_H_
#define _BT_DIS_H_

typedef void *dis_handle_t;

typedef struct system_id_t {
    uint64_t manufacturer_id;            /* manufacturer-defined identifier */
    uint32_t organizationally_unique_id; /* Organizationally Unique Identifier (OUI). */
} system_id_t;

typedef struct regulatory_cert_data_list_t {
    uint8_t *list;      /* Pointer the byte array containing the encoded opaque structure based on IEEE 11073-20601 specification. */
    uint8_t  list_len;  /* Length of the byte array. */
} regulatory_cert_data_list_t;

enum {
    VEND_ID_SOURCE_BLUTTOOTH_SIG = 0x01,
    VEND_ID_SOURCE_USB = 0x02,
};

typedef struct pnp_id_t {
    uint8_t  vendor_id_source;  /* Vendor ID Source, VEND_ID_SOURCE_BLUTTOOTH_SIG or VEND_ID_SOURCE_USB*/
    uint16_t vendor_id;         /* Vendor ID. */
    uint16_t product_id;        /* Product ID. */
    uint16_t product_version;   /* Product Version. */
} pnp_id_t;

typedef struct dis_info_t {
    char *manufacturer_name;                          /* Manufacturer Name String */
    char *model_number;                               /* Model Number String */
    char *serial_number;                              /* Serial Number String */
    char *hardware_revison;                           /* Hardware Revision String */
    char *firmware_revision;                          /* Firmware Revision String */
    char *software_revision;                          /* Software Revision String */
    system_id_t *system_id;                            /* System ID */
    regulatory_cert_data_list_t *regu_cert_data_list;  /* IEEE 11073-20601 Regulatory Certification Data List */
    pnp_id_t *pnp_id;                                  /* PnP ID */
} dis_info_t;

dis_handle_t dis_init(dis_info_t *info);

#endif
