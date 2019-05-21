#ifndef _LAYER21_H
#define _LAYER21_H

#include "include.h"

#if (CONFIG_APP_MP3PLAYER == 1)
	/* max frame size, in samples */
#define MPA_FRAME_SIZE 1152 
	
	/* max compressed frame size */
#define MPA_MAX_CODED_FRAME_SIZE 1792
	
#define MPA_MAX_CHANNELS 2
	
#define SBLIMIT 32 /* number of subbands */
	
#define MPA_STEREO  0
#define MPA_JSTEREO 1
#define MPA_DUAL    2
#define MPA_MONO    3

#define HEADER_SIZE 4
#define BACKSTEP_SIZE 512

#define FRAC_BITS   15   /* fractional bits for sb_samples and dct */
#define WFRAC_BITS  14   /* fractional bits for window */
	
#define FRAC_ONE    (1 << FRAC_BITS)

#define int64_t_C(c)     (c ## L)

#define MULL32(a,b)  (((int32_t)(a) * (int32_t)(b)) >> FRAC_BITS)	
#define MULL(a,b) (((int64_t)(a) * (int64_t)(b)) >> FRAC_BITS)
#define MUL64(a,b) ((int64_t)(a) * (int64_t)(b))

#define FIX(a)   ((int)((a) * FRAC_ONE))

/* WARNING: only correct for posititive numbers */
#define FIXR(a)   ((int)((a) * FRAC_ONE + 0.5))
#define FRAC_RND(a) (((a) + (FRAC_ONE/2)) >> FRAC_BITS)
#define SWAP32(val) (uint32_t)((((uint32_t)(val)) & 0x000000FF)<<24|	\
					(((uint32_t)(val)) & 0x0000FF00)<<8 |	\
					(((uint32_t)(val)) & 0x00FF0000)>>8 |	\
					(((uint32_t)(val)) & 0xFF000000)>>24)
typedef int16_t MPA_INT;

typedef struct _GetBitContext 
{
	const uint8_t *buffer, *buffer_end;
	int index;
	int size_in_bits;
} GetBitContext;

typedef struct _L2DecodeContext {
    uint8_t *inbuf_ptr, *inbuf;
    int32_t frame_size;
    int32_t free_format_frame_size; 
    int32_t error_protection;
    int32_t layer;
    int32_t sample_rate;
		int32_t mode;
    int32_t mode_ext;
    int32_t sample_rate_index; /* between 0 and 8 */
    int32_t bit_rate;
    int32_t lsf;
    GetBitContext gb;
    int32_t nb_channels;
    int16_t *synth_buf;//synth_buf[2][512 * 2];
    int32_t synth_buf_offset[2];
    int32_t *sb_samples;
} L2DecodeContext;	

int L2_decode_frame(L2DecodeContext *l2Dec,
			void *data, int *data_size,
			uint8_t * buf, int buf_size);
uint32_t L2_GetFrameSize(L2DecodeContext *s,uint32_t head);
#endif /* CONFIG_APP_MP3PLAYER */
#endif
