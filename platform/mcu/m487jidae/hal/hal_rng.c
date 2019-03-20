/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "objects.h"
#include "nu_modutil.h"
#include "nu_bitutil.h"
#include "hal_status.h"
#include <k_api.h>

#include "aos/hal/rng.h"
#include "board.h"

#define DEF_MISSION_TIMEOUT		100

kmutex_t 	port_mutex;
ksem_t 		prng_sem;
static int  g_prng_inited = 0;

volatile int  g_Crypto_Int_done = 0;

void CRYPTO_IRQHandler()
{
  	if (PRNG_GET_INT_FLAG(CRPT))
	{
		krhino_sem_give (&prng_sem);
		PRNG_CLR_INT_FLAG(CRPT);
	}
    if (AES_GET_INT_FLAG(CRPT))
    {
        g_Crypto_Int_done = 1;
        AES_CLR_INT_FLAG(CRPT);
    }	
}

/**
  * @brief  Read the PRNG key.
  * @param[in]   crpt        Reference to Crypto module.
  * @param[out]  u32RandKey  The key buffer to store newly generated PRNG key.
  * @return None
  */
static void PRNG_Recv(CRPT_T *crpt, uint32_t u32RandKey[], int expect_size)
{
    uint32_t  i, wcnt;

    wcnt = (((crpt->PRNG_CTL & CRPT_PRNG_CTL_KEYSZ_Msk) >> CRPT_PRNG_CTL_KEYSZ_Pos) + 1U) * 2U;

		wcnt = ( wcnt > expect_size ) ? expect_size: wcnt;
	
    for (i = 0U; i < wcnt; i++)
        u32RandKey[i] = crpt->PRNG_KEY[i];

    crpt->PRNG_CTL &= ~CRPT_PRNG_CTL_SEEDRLD_Msk;
}

/**
 * Initialize pseudo random generator engine.
 *
 * @param[in]   random       the random device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
static int32_t hal_rng_init(random_dev_t* random)
{
	/* Unlock register lock protect */
	SYS_UnlockReg();                   

  /* Enable CRYPTO module clock */
  CLK_EnableModuleClock(CRPT_MODULE);
		
	NVIC_EnableIRQ(CRPT_IRQn);
	PRNG_ENABLE_INT(CRPT);

	if ( krhino_sem_create(&prng_sem, "uartSend", 0) != RHINO_SUCCESS )
		goto exit_hal_rng_init;

	if ( krhino_mutex_create(&port_mutex, "uartMutex") != RHINO_SUCCESS )
		goto exit_hal_rng_init;
		
	g_prng_inited = 1;
	
	return HAL_OK;
	
exit_hal_rng_init:
	return HAL_ERROR;
}

/**
 * Finalize pseudo random generator engine.
 *
 * @param[in]   random       the random device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
static int32_t hal_rng_finalize(random_dev_t* random)
{
	// Finalize
	if ( g_prng_inited )
	{
		krhino_sem_del(&prng_sem);
		krhino_mutex_del(&port_mutex);
		PRNG_DISABLE_INT(CRPT);
		NVIC_DisableIRQ(CRPT_IRQn);
	}
	g_prng_inited = 0;
}

/**
 * Fill in a memory buffer with random data
 *
 * @param[in]   random       the random device
 * @param[out]  inBuffer     Point to a valid memory buffer, this function will fill
 *                           in this memory with random numbers after executed
 * @param[in]   inByteCount  Length of the memory buffer (bytes)
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
static int32_t hal_rng_read(random_dev_t* random, void *buf, int32_t bytes)
{
	int offset=0;
	int mission_len=0;
	uint8_t* pbuf =  (uint8_t*)buf;
	
	if ( !random || !buf || (bytes==0 ) )
		return HAL_ERROR;
		
	// Initialize
	if ( hal_rng_init(random)<0 || !g_prng_inited  )
		return HAL_ERROR;

  /* Wait for Lock */
  while ( krhino_mutex_lock(&port_mutex, 0xFFFFFFFF) != RHINO_SUCCESS );

	while ( bytes > 0 )
	{
		/* start PRNG with seed 0x55 */
		PRNG_Open(CRPT, PRNG_KEY_SIZE_256, PRNG_SEED_CONT, (unsigned int)aos_now_ms());
		PRNG_Start(CRPT);
		
		if ( krhino_sem_take(&prng_sem, DEF_MISSION_TIMEOUT) != RHINO_SUCCESS )
			goto exit_hal_rng_read;

		mission_len = ( bytes >= 32 ) ? 32: bytes;

		memset ( (void*) &pbuf[offset], 0, mission_len );
		PRNG_Recv( CRPT, &pbuf[offset], mission_len );

		bytes -= mission_len;
		offset += mission_len;

	} //while
	
  /* Unlock */
  krhino_mutex_unlock(&port_mutex);
		
	return HAL_OK;

exit_hal_rng_read:

  krhino_mutex_unlock(&port_mutex);
	
	if ( g_prng_inited )
		hal_rng_finalize(random);
	
	return HAL_ERROR;
}

/**
 * Fill in a memory buffer with random data
 *
 * @param[in]   random       the random device
 * @param[out]  inBuffer     Point to a valid memory buffer, this function will fill
 *                           in this memory with random numbers after executed
 * @param[in]   inByteCount  Length of the memory buffer (bytes)
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_random_num_read(random_dev_t random, void *buf, int32_t bytes)
{
	return hal_rng_read(&random, buf, bytes);
}
