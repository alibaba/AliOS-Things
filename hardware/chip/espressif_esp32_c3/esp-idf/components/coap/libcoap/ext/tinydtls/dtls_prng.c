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

#include "tinydtls.h"

#if defined (WITH_CONTIKI)
#include "platform-specific/dtls_prng_contiki.c"

#elif defined (ESPIDF_VERSION)
#include "platform-specific/dtls_prng_espidf.c"

#elif defined (RIOT_VERSION)
#include "platform-specific/dtls_prng_riot.c"

#elif defined (WITH_POSIX)
#include "platform-specific/dtls_prng_posix.c"

#else
#error platform specific prng not defined

#endif
