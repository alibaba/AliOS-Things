/*******************************************************************************
 *
 * Copyright (c) 2011-2019 Olaf Bergmann (TZI) and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v. 1.0 which accompanies this distribution.
 *
 * The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Olaf Bergmann  - initial API and implementation
 *    Hauke Mehrtens - memory optimization, ECC integration
 *    Jon Shallow    - platform dependent prng support
 *
 *******************************************************************************/

/** 
 * @file prng.h
 * @brief Pseudo Random Numbers
 */

#ifndef _DTLS_PRNG_H_
#define _DTLS_PRNG_H_

#include "tinydtls.h"
#include <stdlib.h>

/** 
 * @defgroup prng Pseudo Random Numbers
 * @{
 */

/**
 * Fills \p buf with \p len random bytes. This is the default
 * implementation for prng().  You might want to change prng() to use
 * a better PRNG on your specific platform.
 *
 * @buf The buffer to fill
 * @len the length of the buffer to fill
 *
 * @return 1 buffer filled
 */
int dtls_prng(unsigned char *buf, size_t len);

/**
 * Seeds the random number generator used by the function dtls_prng()
 *
 * @seed The seed to prime the random number generator
 */
void dtls_prng_init(unsigned seed);

/** @} */

#endif /* _DTLS_PRNG_H_ */
