/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_INTERPT_H
#define HAL_INTERPT_H

typedef void (*hal_interpt_t)(int32_t vec, void *para);

typedef struct {
    hal_interpt_t fun;
    void         *para;
} hal_interpt_desc_t;


/**
 * Interrupt vector init
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int32_t hal_interpt_init(void);

/**
 * Mask specified interrupt vector
 *
 *
 * @param[in] vec specified interrupt vector
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int32_t hal_interpt_mask(int32_t vec);

/**
 * Unmask specified interrupt vector
 *
 *
 * @param[in] vec specified interrupt vector
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int32_t hal_interpt_umask(int32_t vec);

/**
 * Install specified interrupt vector
 *
 *
 * @param[in] vec specified interrupt vector
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int32_t hal_interpt_install(int32_t vec, hal_interpt_t handler, 
                                     void *para,  char *name);
#endif

