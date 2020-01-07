/*
 * Extracted (and adjusted) from the Sodium library v1.0.16
 * https://libsodium.org
 */

#ifndef SIPHASH24_H
#define SIPHASH24_H

#define siphash24_BYTES 8U
#define siphash24_KEYBYTES 16U

int siphash24(unsigned char *out, const unsigned char *in,
              unsigned long long inlen, const unsigned char *k);

#endif /* OWL_SIPHASH24_H */
