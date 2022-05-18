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
 *    Achim Kraus    - session recovery
 *    Sachin Agrawal - rehandshake support
 *    Jon Shallow    - platform dependent prng support
 *
 *******************************************************************************/

#include "tinydtls.h"
#include "dtls_prng.h"
#include <string.h>
#include "random.h"

int
dtls_prng(unsigned char *buf, size_t len)
{
#ifdef HAVE_PRNG
  return contiki_prng_impl(buf, len);
#else /* ! HAVE_PRNG */
  /**
   * Fills \p buf with \p len random bytes. This is the default
   * implementation for prng().  You might want to change prng() to use
   * a better PRNG on your specific platform.
   */
  unsigned short v = random_rand();
  while (len > sizeof(v)) {
    memcpy(buf, &v, sizeof(v));
    len -= sizeof(v);
    buf += sizeof(v);
    v = random_rand();
  }

  memcpy(buf, &v, len);
  return 1;
}
#endif /* ! HAVE_PRNG */

void
dtls_prng_init(unsigned seed)
{
 /* random_init() messes with the radio interface of the CC2538 and
  * therefore must not be called after the radio has been
  * initialized. */
#ifndef CONTIKI_TARGET_CC2538DK
  random_init(seed);
#endif
}

