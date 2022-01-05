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
#include "dtls_debug.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * Fills \p buf with \p len random bytes. This is the default
 * implementation for prng().  You might want to change prng() to use
 * a better PRNG on your specific platform.
 */
int
dtls_prng(unsigned char *buf, size_t len) {
  while (len--)
    *buf++ = rand() & 0xFF;
  return 1;
}

void
dtls_prng_init(unsigned seed) {
  FILE *urandom = fopen("/dev/urandom", "r");
  unsigned char buf[sizeof(unsigned long)];
  (void)seed;

  if (!urandom) {
    dtls_emerg("cannot initialize PRNG\n");
    return;
  }

  if (fread(buf, 1, sizeof(buf), urandom) != sizeof(buf)) {
    dtls_emerg("cannot initialize PRNG\n");
    return;
  }

  fclose(urandom);
  srand((unsigned long)*buf);
}

