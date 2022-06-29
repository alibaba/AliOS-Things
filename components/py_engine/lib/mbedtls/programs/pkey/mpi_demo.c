/*
 *  Simple MPI demonstration program
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 *
 *  This file is provided under the Apache License 2.0, or the
 *  GNU General Public License v2.0 or later.
 *
 *  **********
 *  Apache License 2.0:
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  **********
 *
 *  **********
 *  GNU General Public License v2.0 or later:
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  **********
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#include <stdlib.h>
#define mbedtls_printf          printf
#define mbedtls_exit            exit
#define MBEDTLS_EXIT_SUCCESS    EXIT_SUCCESS
#define MBEDTLS_EXIT_FAILURE    EXIT_FAILURE
#endif /* MBEDTLS_PLATFORM_C */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO)
#include "mbedtls/bignum.h"

#include <stdio.h>
#endif

#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_FS_IO)
int main( void )
{
    mbedtls_printf("MBEDTLS_BIGNUM_C and/or MBEDTLS_FS_IO not defined.\n");
    mbedtls_exit( 0 );
}
#else


int main( void )
{
    int ret = 1;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    mbedtls_mpi E, P, Q, N, H, D, X, Y, Z;

    mbedtls_mpi_init( &E ); mbedtls_mpi_init( &P ); mbedtls_mpi_init( &Q ); mbedtls_mpi_init( &N );
    mbedtls_mpi_init( &H ); mbedtls_mpi_init( &D ); mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y );
    mbedtls_mpi_init( &Z );

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &P, 10, "2789" ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &Q, 10, "3203" ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &E, 10,  "257" ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &N, &P, &Q ) );

    mbedtls_printf( "\n  Public key:\n\n" );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_file( "  N = ", &N, 10, NULL ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_file( "  E = ", &E, 10, NULL ) );

    mbedtls_printf( "\n  Private key:\n\n" );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_file( "  P = ", &P, 10, NULL ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_file( "  Q = ", &Q, 10, NULL ) );

#if defined(MBEDTLS_GENPRIME)
    MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &P, &P, 1 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &Q, &Q, 1 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &H, &P, &Q ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_inv_mod( &D, &E, &H ) );

    mbedtls_mpi_write_file( "  D = E^-1 mod (P-1)*(Q-1) = ",
                    &D, 10, NULL );
#else
    mbedtls_printf("\nTest skipped (MBEDTLS_GENPRIME not defined).\n\n");
#endif
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &X, 10, "55555" ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &Y, &X, &E, &N, NULL ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &Z, &Y, &D, &N, NULL ) );

    mbedtls_printf( "\n  RSA operation:\n\n" );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_file( "  X (plaintext)  = ", &X, 10, NULL ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_file( "  Y (ciphertext) = X^E mod N = ", &Y, 10, NULL ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_file( "  Z (decrypted)  = Y^D mod N = ", &Z, 10, NULL ) );
    mbedtls_printf( "\n" );

    exit_code = MBEDTLS_EXIT_SUCCESS;

cleanup:
    mbedtls_mpi_free( &E ); mbedtls_mpi_free( &P ); mbedtls_mpi_free( &Q ); mbedtls_mpi_free( &N );
    mbedtls_mpi_free( &H ); mbedtls_mpi_free( &D ); mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y );
    mbedtls_mpi_free( &Z );

    if( exit_code != MBEDTLS_EXIT_SUCCESS )
    {
        mbedtls_printf( "\nAn error occurred.\n" );
    }

#if defined(_WIN32)
    mbedtls_printf( "  Press Enter to exit this program.\n" );
    fflush( stdout ); getchar();
#endif

    mbedtls_exit( exit_code );
}
#endif /* MBEDTLS_BIGNUM_C && MBEDTLS_FS_IO */
