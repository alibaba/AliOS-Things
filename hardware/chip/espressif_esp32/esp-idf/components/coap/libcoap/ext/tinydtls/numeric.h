/*******************************************************************************
 *
 * Copyright (c) 2011, 2012, 2013, 2014, 2015 Olaf Bergmann (TZI) and others.
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
 *
 *******************************************************************************/

#ifndef _DTLS_NUMERIC_H_
#define _DTLS_NUMERIC_H_

#include <stdint.h>

#ifndef min
#define min(A,B) ((A) <= (B) ? (A) : (B))
#endif

#ifndef max
#define max(A,B) ((A) < (B) ? (B) : (A))
#endif

/* this one is for consistency... */
static inline int dtls_int_to_uint8(unsigned char *field, uint8_t value)
{
  field[0] = value & 0xff;
  return 1;
}

static inline int dtls_int_to_uint16(unsigned char *field, uint16_t value)
{
  field[0] = (value >> 8) & 0xff;
  field[1] = value & 0xff;
  return 2;
}

static inline int dtls_int_to_uint24(unsigned char *field, uint32_t value)
{
  field[0] = (value >> 16) & 0xff;
  field[1] = (value >> 8) & 0xff;
  field[2] = value & 0xff;
  return 3;
}

static inline int dtls_int_to_uint32(unsigned char *field, uint32_t value)
{
  field[0] = (value >> 24) & 0xff;
  field[1] = (value >> 16) & 0xff;
  field[2] = (value >> 8) & 0xff;
  field[3] = value & 0xff;
  return 4;
}

static inline int dtls_int_to_uint48(unsigned char *field, uint64_t value)
{
  field[0] = (value >> 40) & 0xff;
  field[1] = (value >> 32) & 0xff;
  field[2] = (value >> 24) & 0xff;
  field[3] = (value >> 16) & 0xff;
  field[4] = (value >> 8) & 0xff;
  field[5] = value & 0xff;
  return 6;
}

static inline int dtls_int_to_uint64(unsigned char *field, uint64_t value)
{
  field[0] = (value >> 56) & 0xff;
  field[1] = (value >> 48) & 0xff;
  field[2] = (value >> 40) & 0xff;
  field[3] = (value >> 32) & 0xff;
  field[4] = (value >> 24) & 0xff;
  field[5] = (value >> 16) & 0xff;
  field[6] = (value >> 8) & 0xff;
  field[7] = value & 0xff;
  return 8;
}

static inline uint8_t dtls_uint8_to_int(const unsigned char *field)
{
  return (uint8_t)field[0];
}

static inline uint16_t dtls_uint16_to_int(const unsigned char *field)
{
  return ((uint16_t)field[0] << 8)
	 | (uint16_t)field[1];
}

static inline uint32_t dtls_uint24_to_int(const unsigned char *field)
{
  return ((uint32_t)field[0] << 16)
	 | ((uint32_t)field[1] << 8)
	 | (uint32_t)field[2];
}

static inline uint32_t dtls_uint32_to_int(const unsigned char *field)
{
  return ((uint32_t)field[0] << 24)
	 | ((uint32_t)field[1] << 16)
	 | ((uint32_t)field[2] << 8)
	 | (uint32_t)field[3];
}

static inline uint64_t dtls_uint48_to_int(const unsigned char *field)
{
  return ((uint64_t)field[0] << 40)
	 | ((uint64_t)field[1] << 32)
	 | ((uint64_t)field[2] << 24)
	 | ((uint64_t)field[3] << 16)
	 | ((uint64_t)field[4] << 8)
	 | (uint64_t)field[5];
}

static inline uint64_t dtls_uint64_to_int(const unsigned char *field)
{
  return ((uint64_t)field[0] << 56)
	 | ((uint64_t)field[1] << 48)
	 | ((uint64_t)field[2] << 40)
	 | ((uint64_t)field[3] << 32)
	 | ((uint64_t)field[4] << 24)
	 | ((uint64_t)field[5] << 16)
	 | ((uint64_t)field[6] << 8)
	 | (uint64_t)field[7];
}

#endif /* _DTLS_NUMERIC_H_ */
