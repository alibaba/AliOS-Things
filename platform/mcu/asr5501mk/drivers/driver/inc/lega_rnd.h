#ifndef _LEGA_RND_H_
#define _LEGA_RND_H_

typedef struct {
    char *p_workspace;
    uint8_t *seed;
    uint16_t seed_size;
    uint8_t *rnd_data;
    uint16_t rnd_size;
} lega_rnd_dev_t;

int lega_rnd_init(lega_rnd_dev_t *rnd);

int lega_rnd_generate(lega_rnd_dev_t *rnd);

int lega_rnd_finalize(lega_rnd_dev_t *rnd);

#endif //_LEGA_RND_H_