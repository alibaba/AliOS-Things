#ifndef _BLE_ADV_DEFS_
#define _BLE_ADV_DEFS_

/* EIR/AD data type definitions */
#define EIRADV_DATA_FLAGS                   0x01 /* AD flags */
#define EIRADV_DATA_UUID16_SOME             0x02 /* 16-bit UUID, more available */
#define EIRADV_DATA_UUID16_ALL              0x03 /* 16-bit UUID, all listed */
#define EIRADV_DATA_UUID32_SOME             0x04 /* 32-bit UUID, more available */
#define EIRADV_DATA_UUID32_ALL              0x05 /* 32-bit UUID, all listed */
#define EIRADV_DATA_UUID128_SOME            0x06 /* 128-bit UUID, more available */
#define EIRADV_DATA_UUID128_ALL             0x07 /* 128-bit UUID, all listed */
#define EIRADV_DATA_NAME_SHORTENED          0x08 /* Shortened name */
#define EIRADV_DATA_NAME_COMPLETE           0x09 /* Complete name */
#define EIRADV_DATA_TX_POWER                0x0a /* Tx Power */
#define EIRADV_DATA_SOLICIT16               0x14 /* Solicit UUIDs, 16-bit */
#define EIRADV_DATA_SOLICIT128              0x15 /* Solicit UUIDs, 128-bit */
#define EIRADV_DATA_SVC_DATA16              0x16 /* Service data, 16-bit UUID */
#define EIRADV_DATA_GAP_APPEARANCE          0x19 /* GAP appearance */
#define EIRADV_DATA_SOLICIT32               0x1f /* Solicit UUIDs, 32-bit */
#define EIRADV_DATA_SVC_DATA32              0x20 /* Service data, 32-bit UUID */
#define EIRADV_DATA_SVC_DATA128             0x21 /* Service data, 128-bit UUID */
#define EIRADV_DATA_MESH_PROV               0x29 /* Mesh Provisioning PDU */
#define EIRADV_DATA_MESH_MESSAGE            0x2a /* Mesh Networking PDU */
#define EIRADV_DATA_MESH_BEACON             0x2b /* Mesh Beacon */
#define EIRADV_DATA_MANUFACTURER_DATA       0xff /* Manufacturer Specific Data */

#define AD_FLAG_LIMITED                0x01 /* Limited Discoverable */
#define AD_FLAG_GENERAL                0x02 /* General Discoverable */
#define AD_FLAG_NO_BREDR               0x04 /* BR/EDR not supported */

/**
 * This structure holds the advertisement data.
 *
 * Note: The final advertisement data may consist of 
 *       multiple elements of this type.
 */
struct adv_data {
    uint8_t type;
    uint8_t data_len;
    const uint8_t *data;
};

/**
 * Helper to declare elements of adv_data arrays
 *
 * @param[in]  _type      Type of advertising data field
 * @param[in]  _data      Pointer to the data field payload
 * @param[in]  _data_len  Number of bytes behind the _data pointer
 */
#define ADV_DATA(_type, _data, _data_len) \
    { \
        .type = (_type), \
        .data_len = (_data_len), \
        .data = (const uint8_t *)(_data), \
    }

/**
 * Helper to declare elements of adv_data arrays
 *
 * @param[in]  _type   Type of advertising data field
 * @param[in]  _bytes  Variable number of single-byte parameters
 */
#define ADV_DATA_BYTES(_type, _bytes...) \
    ADV_DATA(_type, ((uint8_t []) { _bytes }), \
    sizeof((uint8_t []) { _bytes }))

#endif
