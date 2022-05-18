/*******************************************************************************
 *
 * Copyright (c) 2019 Olaf Bergmann (TZI) and others.
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
 *    Jon Shallow    - split out wrapper code to support external rijndael code
 *
 *
 *******************************************************************************/

#include "rijndael.h"

/* setup key context for encryption only */
int
rijndael_set_key_enc_only(rijndael_ctx *ctx, const u_char *key, int bits)
{
	int rounds;

	rounds = rijndaelKeySetupEnc(ctx->ek, key, bits);
	if (rounds == 0)
		return -1;

	ctx->Nr = rounds;
#ifdef WITH_AES_DECRYPT
	ctx->enc_only = 1;
#endif

	return 0;
}

#ifdef WITH_AES_DECRYPT
/* setup key context for both encryption and decryption */
int
rijndael_set_key(rijndael_ctx *ctx, const u_char *key, int bits)
{
	int rounds;

	rounds = rijndaelKeySetupEnc(ctx->ek, key, bits);
	if (rounds == 0)
		return -1;
	if (rijndaelKeySetupDec(ctx->dk, key, bits) != rounds)
		return -1;

	ctx->Nr = rounds;
	ctx->enc_only = 0;

	return 0;
}

void
rijndael_decrypt(rijndael_ctx *ctx, const u_char *src, u_char *dst)
{
	rijndaelDecrypt(ctx->dk, ctx->Nr, src, dst);
}
#endif

void
rijndael_encrypt(rijndael_ctx *ctx, const u_char *src, u_char *dst)
{
	rijndaelEncrypt(ctx->ek, ctx->Nr, src, dst);
}
