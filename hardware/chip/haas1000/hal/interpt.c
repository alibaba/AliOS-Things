/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/interpt.h"


/**
 * Interrupt vector init
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int32_t hal_interpt_init(void)
{
	return 0;
}

/**
 * Mask specified interrupt vector
 *
 *
 * @param[in]  vec  specified interrupt vector
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int32_t hal_interpt_mask(int32_t vec)
{
	return 0;
}

/**
 * Unmask specified interrupt vector
 *
 *
 * @param[in]  vec  specified interrupt vector
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int32_t hal_interpt_umask(int32_t vec)
{
	return 0;
}

/**
 * Install specified interrupt vector
 *
 *
 * @param[in]  vec  specified interrupt vector
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int32_t hal_interpt_install(int32_t vec, hal_interpt_t handler, void *para,  char *name)
{
	return 0;
}

