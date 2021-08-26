#ifndef __BESAUD_API_H__
#define __BESAUD_API_H__

#include "cqueue.h"
#include "bluetooth.h"
#include "me_api.h"

#define BTIF_BESAUD_EVENT_CONTROL_CONNECTED            0x21

#define BTIF_BESAUD_EVENT_CONTROL_DISCONNECTED         0x22

#define BTIF_BESAUD_EVENT_CONTROL_DATA_IND             0x23

#define BTIF_BESAUD_EVENT_CONTROL_DATA_SENT            0x24

#define BTIF_BESAUD_EVENT_CONTROL_SET_IDLE             0x30

typedef uint16_t btif_besaud_event;

typedef void (*btif_besaud_status_change_callback)(btif_besaud_event event);

typedef void (*btif_besaud_data_received_callback)(uint8_t*, uint16_t);

#ifdef __cplusplus
extern "C" {
#endif

uint8_t btif_besaud_is_connected(void);
uint8_t btif_besaud_is_cmd_sending(void);
void btif_besaud_clear_cmd_sending(void);
void btif_besaud_client_create(btif_remote_device_t *dev);
void btif_besaud_server_create(btif_besaud_status_change_callback callback);
void btif_besaud_data_received_register(btif_besaud_data_received_callback callback);
void btif_besaud_send_cmd(uint8_t* cmd, uint16_t len);
bt_status_t btif_besaud_send_cmd_no_wait(uint8_t* cmd, uint16_t len);
void btif_besaud_status_changed(btif_besaud_event event);
btif_remote_device_t *btif_besaud_get_peer_device(void);

#ifdef __cplusplus
}
#endif

#endif

