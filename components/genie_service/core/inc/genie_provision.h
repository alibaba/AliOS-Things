#ifndef __GENIE_PROVISION_H__
#define __GENIE_PROVISION_H__

#include <stdint.h>
#include <genie_storage.h>

#define GENIE_PROVISON_UUID_LEN (16)

#define UNPROV_ADV_FEATURE1_UUID_VERSION 0x02
#define UNPROV_ADV_FEATURE1_SILENT_ADV 0x01
#define UNPROV_ADV_FEATURE1_SILENT_UNMASK 0xFE
#define UNPROV_ADV_FEATURE2_GENIE_MESH_STACK_V1 0x10 //bit4-7
#define UNPROV_ADV_FEATURE2_GENIE_MESH_STACK_V2 0x20 //bit4-7
#define UNPROV_ADV_FEATURE2_GENIE_MESH_STACK_V3 0x30 //bit4-7
#define UNPROV_ADV_FEATURE2_ULTRA_PROV_FLAG 0x01  //bit0
#define UNPROV_ADV_FEATURE2_AUTH_FLAG 0x02        //bit1

#define MESH_PROVISIONING_TIMEOUT 60 * 1000

typedef enum _genie_provision_state_e
{
    GENIE_PROVISION_UNPROV,
    GENIE_PROVISION_START,
    GENIE_PROVISION_SUCCESS
} genie_provision_state_e;

typedef struct _genie_provision_s
{
    uint16_t addr;
    uint32_t seq;
    uint8_t devkey[16];
    mesh_netkey_para_t netkey;
    mesh_appkey_para_t appkey;
} genie_provision_t;

/**
 * @brief get the trituple info formatted in uuid
 * @return the string refering to uuid
 */
uint8_t *genie_provision_get_uuid(void);

void genie_provision_clear_silent_flag(void);

int genie_provision_set_state(genie_provision_state_e state);

genie_provision_state_e genie_provision_get_state(void);

int genie_provision_get_saved_data(genie_provision_t *p_genie_provision);

#ifdef GENIE_ULTRA_PROV
#define GENIE_PROV_ADV_TYPE (0xFF)
#define GENIE_PROV_FIXED_BYTE (0x0D)

typedef enum _genie_prov_status_e
{
    GENIE_PROV_STATUS_RECV_RANDOM,
    GENIE_PROV_STATUS_SEND_CONFIRM_DATA,
    GENIE_PROV_STATUS_RECV_PROV_DATA,
    GENIE_PROV_STATUS_SEND_COMPLETED
} genie_prov_status_e;

void genie_provision_ultra_prov_free(void);
int genie_provision_ultra_prov_handle(uint8_t frame_type, void *frame_buf);
#endif

/**
 * @brief
 */
void genie_provision_pbadv_timer_start(uint32_t prov_timeout);

/**
 * @brief
 */
void genie_provision_pbadv_timer_stop(void);

/**
 * @brief
 */
void genie_provision_prov_timer_start(void);

/**
 * @brief
 */
void genie_provision_prov_timer_stop(void);

/**
 * @brief
 */
void genie_provision_start_slient_pbadv(void);

#endif
