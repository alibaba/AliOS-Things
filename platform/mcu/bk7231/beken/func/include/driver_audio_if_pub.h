#ifndef DRIVER_AUDIO_IF_PUB_H
#define DRIVER_AUDIO_IF_PUB_H

extern void aud_open(void);
extern void aud_close(void);
extern uint8_t aud_get_channel(void);
extern uint16_t aud_get_buffer_size(void);
extern void  aud_fill_buffer( uint8_t *buff, uint16_t len );
extern uint16_t aud_get_buffer_length(uint8_t *buff, uint16_t len);
extern void aud_initial(uint32_t freq, uint32_t channels, uint32_t bits_per_sample);
extern uint16_t aud_get_fill_size(void);
extern int32_t aud_hw_init(void);
extern uint8_t is_aud_opened(void);
#endif
