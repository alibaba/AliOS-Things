// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _SSL_OPT_H_
#define _SSL_OPT_H_

/*
 * Enable OpenSSL debugging function.
 *
 * If the option is enabled, "SSL_DEBUG" works.
 */
//#define CONFIG_OPENSSL_DEBUG

#ifdef CONFIG_OPENSSL_DEBUG

/*
 * OpenSSL debugging level.
 *
 * Only function whose debugging level is higher than "OPENSSL_DEBUG_LEVEL" works.
 *
 * For example:
 *     If OPENSSL_DEBUG_LEVEL = 2, you use function "SSL_DEBUG(1, "malloc failed")". 
 *     Because 1 < 2, it will not print.
 */
//#define CONFIG_OPENSSL_DEBUG_LEVEL

/*
 * If the option is enabled, low-level module debugging function of OpenSSL is enabled, 
 * e.g. mbedtls internal debugging function.
 */
//#define CONFIG_OPENSSL_LOWLEVEL_DEBUG

#endif /* CONFIG_OPENSSL_DEBUG */

/*
 * OpenSSL function needs "assert" function to check if input parameters are valid.
 *
 * If you want to use assert debugging function, "OPENSSL_DEBUG" should be enabled.
 *
 * You must only select one of following:
 *     1. CONFIG_OPENSSL_ASSERT_DO_NOTHING
 *     2. CONFIG_OPENSSL_ASSERT_EXIT
 *     3. CONFIG_OPENSSL_ASSERT_DEBUG (depend on "CONFIG_OPENSSL_DEBUG")
 *     4. CONFIG_OPENSSL_ASSERT_DEBUG_EXIT (depend on "CONFIG_OPENSSL_DEBUG")
 *     5. CONFIG_OPENSSL_ASSERT_DEBUG_BLOCK (depend on "CONFIG_OPENSSL_DEBUG")
 */

/*
 * Do nothing and "SSL_ASSERT" does not work.
 */
//#define CONFIG_OPENSSL_ASSERT_DO_NOTHING

/*
 * Enable assert exiting, it will check and return error code.
 */
#define CONFIG_OPENSSL_ASSERT_EXIT
  
#ifdef CONFIG_OPENSSL_DEBUG
      
/*
 * Enable assert debugging, it will check and show debugging message.
 */
//#define CONFIG_OPENSSL_ASSERT_DEBUG

/*
 * Enable assert debugging and exiting, it will check, show debugging message and return error code.
 */
//#define CONFIG_OPENSSL_ASSERT_DEBUG_EXIT

/*
 * Enable assert debugging and blocking, it will check, show debugging message and block by "while (1);".
 */
//#define CONFIG_OPENSSL_ASSERT_DEBUG_BLOCK

#endif /* CONFIG_OPENSSL_DEBUG */

#endif

