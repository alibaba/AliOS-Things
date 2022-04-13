#ifndef __ALSA_RESAMPLE_HOOK_H__
#define __ALSA_RESAMPLE_HOOK_H__
#ifdef ALSA_RESAMPLE_USER_HOOK_EN
/**
 * state:
 * 0: start data stream
 * 1: stop data stream
*/
typedef void (*alsa_resample_user_cb_t)(uint8_t state, void * user_arg);
void alsa_resample_register_user_callback(alsa_resample_user_cb_t cb, void * arg);
#endif
#endif