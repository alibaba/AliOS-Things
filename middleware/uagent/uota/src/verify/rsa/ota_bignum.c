#include "ota_pub.h"
#include "ota_bn_mul.h"

#define OTA_MPI_CHK(f) do { if( ( ret = f ) != 0 ) goto cleanup; } while( 0 )

static void ota_mpi_zeroize( ota_mpi_uint *v, size_t n ) {
    volatile ota_mpi_uint *p = v; while( n-- ) *p++ = 0;
}

#define ciL    (sizeof(ota_mpi_uint))         /* chars in limb  */
#define biL    (ciL << 3)               /* bits  in limb  */
#define biH    (ciL << 2)               /* half limb size */

#define MPI_SIZE_T_MAX  ( (size_t) -1 ) /* SIZE_T_MAX is not standard */

#define BITS_TO_LIMBS(i)  ( (i) / biL + ( (i) % biL != 0 ) )
#define CHARS_TO_LIMBS(i) ( (i) / ciL + ( (i) % ciL != 0 ) )

void ota_mpi_init( ota_mpi *X )
{
    if( X == NULL ) {
        OTA_LOG_E("ota mpi init is null \n");
	return;
    }
    X->s = 1;
    X->n = 0;
    X->p = NULL;
}

void ota_mpi_free( ota_mpi *X )
{
    if( X == NULL )
        return;

    if( X->p != NULL )
    {
        ota_mpi_zeroize( X->p, X->n );
        ota_free( X->p );
    }

    X->s = 1;
    X->n = 0;
    X->p = NULL;
}

int ota_mpi_grow( ota_mpi *X, size_t nblimbs )
{
    ota_mpi_uint *p;

    if( nblimbs > OTA_MPI_MAX_LIMBS )
        return( OTA_ERR_MPI_ALLOC_FAILED );

    if( X->n < nblimbs )
    {
        if( ( p = (ota_mpi_uint*)ota_calloc( nblimbs, ciL ) ) == NULL )
            return( OTA_ERR_MPI_ALLOC_FAILED );

        if( X->p != NULL )
        {
            memcpy( p, X->p, X->n * ciL );
            ota_mpi_zeroize( X->p, X->n );
            ota_free( X->p );
        }

        X->n = nblimbs;
        X->p = p;
    }

    return( 0 );
}

int ota_mpi_copy( ota_mpi *X, const ota_mpi *Y )
{
    int ret;
    unsigned int i;

    if( X == Y )
        return( 0 );

    if( Y->p == NULL )
    {
        ota_mpi_free( X );
        return( 0 );
    }

    for( i = Y->n - 1; i > 0; i-- )
        if( Y->p[i] != 0 )
            break;
    i++;

    X->s = Y->s;

    OTA_MPI_CHK( ota_mpi_grow( X, i ) );

    memset( X->p, 0, X->n * ciL );
    memcpy( X->p, Y->p, i * ciL );

cleanup:

    return( ret );
}

int ota_mpi_lset( ota_mpi *X, ota_mpi_sint z )
{
    int ret;

    OTA_MPI_CHK( ota_mpi_grow( X, 1 ) );
    memset( X->p, 0, X->n * ciL );

    X->p[0] = ( z < 0 ) ? -z : z;
    X->s    = ( z < 0 ) ? -1 : 1;

cleanup:

    return( ret );
}


static unsigned int ota_clz( const ota_mpi_uint x )
{
    unsigned int j;
    ota_mpi_uint mask = (ota_mpi_uint) 1 << (biL - 1);

    for( j = 0; j < biL; j++ )
    {
        if( x & mask ) break;

        mask >>= 1;
    }

    return j;
}

size_t ota_mpi_bitlen( const ota_mpi *X )
{
    unsigned int i, j;

    if( X->n == 0 )
        return( 0 );

    for( i = X->n - 1; i > 0; i-- )
        if( X->p[i] != 0 )
            break;

    j = biL - ota_clz( X->p[i] );
    return( ( i * biL ) + j );
}

size_t ota_mpi_size( const ota_mpi *X )
{
    return( ( ota_mpi_bitlen( X ) + 7 ) >> 3 );
}

int ota_mpi_read_binary( ota_mpi *X, const unsigned char *buf, size_t buflen )
{
    int ret;
    unsigned int i, j, n;

    for( n = 0; n < buflen; n++ )
        if( buf[n] != 0 )
            break;

    OTA_MPI_CHK( ota_mpi_grow( X, CHARS_TO_LIMBS( buflen - n ) ) );
    OTA_MPI_CHK( ota_mpi_lset( X, 0 ) );

    for( i = buflen, j = 0; i > n; i--, j++ )
        X->p[j / ciL] |= ((ota_mpi_uint) buf[i - 1]) << ((j % ciL) << 3);

cleanup:

    return( ret );
}

int ota_mpi_write_binary( const ota_mpi *X, unsigned char *buf, size_t buflen )
{
    unsigned int i, j, n;

    n = ota_mpi_size( X );
    if( buflen < n )
        return( OTA_ERR_MPI_BUFFER_TOO_SMALL );

    memset( buf, 0, buflen );

    for( i = buflen - 1, j = 0; n > 0; i--, j++, n-- )
        buf[i] = (unsigned char)( X->p[j / ciL] >> ((j % ciL) << 3) );

    return( 0 );
}

int ota_mpi_shift_l( ota_mpi *X, size_t count )
{
    int ret;
    size_t i, v0, t1;
    ota_mpi_uint r0 = 0, r1;

    v0 = count / (biL    );
    t1 = count & (biL - 1);

    i = ota_mpi_bitlen( X ) + count;
    if( X->n * biL < i )
        OTA_MPI_CHK( ota_mpi_grow( X, BITS_TO_LIMBS( i ) ) );

    ret = 0;
    if( v0 > 0 )
    {
        for( i = X->n; i > v0; i-- )
            X->p[i - 1] = X->p[i - v0 - 1];

        for( ; i > 0; i-- )
            X->p[i - 1] = 0;
    }

    if( t1 > 0 )
    {
        for( i = v0; i < X->n; i++ )
        {
            r1 = X->p[i] >> (biL - t1);
            X->p[i] <<= t1;
            X->p[i] |= r0;
            r0 = r1;
        }
    }

cleanup:

    return( ret );
}

int ota_mpi_shift_r( ota_mpi *X, size_t count )
{
    size_t i, v0, v1;
    ota_mpi_uint r0 = 0, r1;

    v0 = count /  biL;
    v1 = count & (biL - 1);

    if( v0 > X->n || ( v0 == X->n && v1 > 0 ) )
        return ota_mpi_lset( X, 0 );

    if( v0 > 0 )
    {
        for( i = 0; i < X->n - v0; i++ )
            X->p[i] = X->p[i + v0];

        for( ; i < X->n; i++ )
            X->p[i] = 0;
    }

    /*
     * shift by count % limb_size
     */
    if( v1 > 0 )
    {
        for( i = X->n; i > 0; i-- )
        {
            r1 = X->p[i - 1] << (biL - v1);
            X->p[i - 1] >>= v1;
            X->p[i - 1] |= r0;
            r0 = r1;
        }
    }

    return( 0 );
}

/*
 * Compare unsigned values
 */
int ota_mpi_cmp_abs( const ota_mpi *X, const ota_mpi *Y )
{
    unsigned int i, j;

    for( i = X->n; i > 0; i-- )
        if( X->p[i - 1] != 0 )
            break;

    for( j = Y->n; j > 0; j-- )
        if( Y->p[j - 1] != 0 )
            break;

    if( i == 0 && j == 0 )
        return( 0 );

    if( i > j ) return(  1 );
    if( j > i ) return( -1 );

    for( ; i > 0; i-- )
    {
        if( X->p[i - 1] > Y->p[i - 1] ) return(  1 );
        if( X->p[i - 1] < Y->p[i - 1] ) return( -1 );
    }

    return( 0 );
}

int ota_mpi_cmp_mpi( const ota_mpi *X, const ota_mpi *Y )
{
    unsigned int i, j;

    for( i = X->n; i > 0; i-- )
        if( X->p[i - 1] != 0 )
            break;

    for( j = Y->n; j > 0; j-- )
        if( Y->p[j - 1] != 0 )
            break;

    if( i == 0 && j == 0 )
        return( 0 );

    if( i > j ) return(  X->s );
    if( j > i ) return( -Y->s );

    if( X->s > 0 && Y->s < 0 ) return(  1 );
    if( Y->s > 0 && X->s < 0 ) return( -1 );

    for( ; i > 0; i-- )
    {
        if( X->p[i - 1] > Y->p[i - 1] ) return(  X->s );
        if( X->p[i - 1] < Y->p[i - 1] ) return( -X->s );
    }

    return( 0 );
}

int ota_mpi_cmp_int( const ota_mpi *X, ota_mpi_sint z )
{
    ota_mpi Y;
    ota_mpi_uint p[1];

    *p  = ( z < 0 ) ? -z : z;
    Y.s = ( z < 0 ) ? -1 : 1;
    Y.n = 1;
    Y.p = p;

    return( ota_mpi_cmp_mpi( X, &Y ) );
}

int ota_mpi_add_abs( ota_mpi *X, const ota_mpi *A, const ota_mpi *B )
{
    int ret;
    unsigned int i, j;
    ota_mpi_uint *o, *p, c, tmp;

    if( X == B )
    {
        const ota_mpi *T = A; A = X; B = T;
    }

    if( X != A )
        OTA_MPI_CHK( ota_mpi_copy( X, A ) );

    X->s = 1;

    for( j = B->n; j > 0; j-- )
        if( B->p[j - 1] != 0 )
            break;

    OTA_MPI_CHK( ota_mpi_grow( X, j ) );

    o = B->p; p = X->p; c = 0;

    /*
     * tmp is used because it might happen that p == o
     */
    for( i = 0; i < j; i++, o++, p++ )
    {
        tmp= *o;
        *p +=  c; c  = ( *p <  c );
        *p += tmp; c += ( *p < tmp );
    }

    while( c != 0 )
    {
        if( i >= X->n )
        {
            OTA_MPI_CHK( ota_mpi_grow( X, i + 1 ) );
            p = X->p + i;
        }

        *p += c; c = ( *p < c ); i++; p++;
    }

cleanup:

    return( ret );
}

static void ota_mpi_sub_hlp( unsigned int n, ota_mpi_uint *s, ota_mpi_uint *d )
{
    unsigned int i;
    ota_mpi_uint c, z;

    for( i = c = 0; i < n; i++, s++, d++ )
    {
        z = ( *d <  c );     *d -=  c;
        c = ( *d < *s ) + z; *d -= *s;
    }

    while( c != 0 )
    {
        z = ( *d < c ); *d -= c;
        c = z; i++; d++;
    }
}

int ota_mpi_sub_abs( ota_mpi *X, const ota_mpi *A, const ota_mpi *B )
{
    ota_mpi TB;
    int ret;
    unsigned int n;

    if( ota_mpi_cmp_abs( A, B ) < 0 )
        return( OTA_ERR_MPI_NEGATIVE_VALUE );

    ota_mpi_init( &TB );

    if( X == B )
    {
        OTA_MPI_CHK( ota_mpi_copy( &TB, B ) );
        B = &TB;
    }

    if( X != A )
        OTA_MPI_CHK( ota_mpi_copy( X, A ) );

    /*
     * X should always be positive as a result of unsigned subtractions.
     */
    X->s = 1;

    ret = 0;

    for( n = B->n; n > 0; n-- )
        if( B->p[n - 1] != 0 )
            break;

    ota_mpi_sub_hlp( n, B->p, X->p );

cleanup:
    ota_mpi_free( &TB );
    return( ret );
}

int ota_mpi_add_mpi( ota_mpi *X, const ota_mpi *A, const ota_mpi *B )
{
    int ret, s = A->s;

    if( A->s * B->s < 0 )
    {
        if( ota_mpi_cmp_abs( A, B ) >= 0 )
        {
            OTA_MPI_CHK( ota_mpi_sub_abs( X, A, B ) );
            X->s =  s;
        }
        else
        {
            OTA_MPI_CHK( ota_mpi_sub_abs( X, B, A ) );
            X->s = -s;
        }
    }
    else
    {
        OTA_MPI_CHK( ota_mpi_add_abs( X, A, B ) );
        X->s = s;
    }

cleanup:
    return( ret );
}

int ota_mpi_sub_mpi( ota_mpi *X, const ota_mpi *A, const ota_mpi *B )
{
    int ret, s = A->s;

    if( A->s * B->s > 0 )
    {
        if( ota_mpi_cmp_abs( A, B ) >= 0 )
        {
            OTA_MPI_CHK( ota_mpi_sub_abs( X, A, B ) );
            X->s =  s;
        }
        else
        {
            OTA_MPI_CHK( ota_mpi_sub_abs( X, B, A ) );
            X->s = -s;
        }
    }
    else
    {
        OTA_MPI_CHK( ota_mpi_add_abs( X, A, B ) );
        X->s = s;
    }

cleanup:
    return( ret );
}

static
#if defined(__APPLE__) && defined(__arm__)

__attribute__ ((noinline))
#endif
void ota_mpi_mul_hlp( size_t i, ota_mpi_uint *s, ota_mpi_uint *d, ota_mpi_uint b )
{
    ota_mpi_uint c = 0, t = 0;

#if defined(MULADDC_HUIT)
    for( ; i >= 8; i -= 8 )
    {
        MULADDC_INIT
        MULADDC_HUIT
        MULADDC_STOP
    }

    for( ; i > 0; i-- )
    {
        MULADDC_INIT
        MULADDC_CORE
        MULADDC_STOP
    }
#else /* MULADDC_HUIT */
    for( ; i >= 16; i -= 16 )
    {
        MULADDC_INIT
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE

        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_STOP
    }

    for( ; i >= 8; i -= 8 )
    {
        MULADDC_INIT
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE

        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_STOP
    }

    for( ; i > 0; i-- )
    {
        MULADDC_INIT
        MULADDC_CORE
        MULADDC_STOP
    }
#endif /* MULADDC_HUIT */

    t++;

    do {
        *d += c; c = ( *d < c ); d++;
    }
    while( c != 0 );
}

int ota_mpi_mul_mpi( ota_mpi *X, const ota_mpi *A, const ota_mpi *B )
{
    int ret;
    unsigned int i, j;
    ota_mpi TA, TB;

    ota_mpi_init( &TA ); ota_mpi_init( &TB );

    if( X == A ) { OTA_MPI_CHK( ota_mpi_copy( &TA, A ) ); A = &TA; }
    if( X == B ) { OTA_MPI_CHK( ota_mpi_copy( &TB, B ) ); B = &TB; }

    for( i = A->n; i > 0; i-- )
        if( A->p[i - 1] != 0 )
            break;

    for( j = B->n; j > 0; j-- )
        if( B->p[j - 1] != 0 )
            break;

    OTA_MPI_CHK( ota_mpi_grow( X, i + j ) );
    OTA_MPI_CHK( ota_mpi_lset( X, 0 ) );

    for( i++; j > 0; j-- )
        ota_mpi_mul_hlp( i - 1, A->p, X->p + j - 1, B->p[j - 1] );

    X->s = A->s * B->s;

cleanup:

    ota_mpi_free( &TB ); ota_mpi_free( &TA );

    return( ret );
}

int ota_mpi_mul_int( ota_mpi *X, const ota_mpi *A, ota_mpi_uint b )
{
    ota_mpi _B;
    ota_mpi_uint p[1];

    _B.s = 1;
    _B.n = 1;
    _B.p = p;
    p[0] = b;

    return( ota_mpi_mul_mpi( X, A, &_B ) );
}


static ota_mpi_uint ota_int_div_int( ota_mpi_uint u1,
            ota_mpi_uint u0, ota_mpi_uint d, ota_mpi_uint *r )
{
#if defined(MBEDTLS_HAVE_UDBL)
    ota_t_udbl dividend, quotient;
#else
    const ota_mpi_uint radix = (ota_mpi_uint) 1 << biH;
    const ota_mpi_uint uint_halfword_mask = ( (ota_mpi_uint) 1 << biH ) - 1;
    ota_mpi_uint d0, d1, q0, q1, rAX, r0, quotient;
    ota_mpi_uint u0_msw, u0_lsw;
    size_t s;
#endif

    if( 0 == d || u1 >= d )
    {
        if (r != NULL) *r = ~0;

        return ( ~0 );
    }

#if defined(MBEDTLS_HAVE_UDBL)
    dividend  = (ota_t_udbl) u1 << biL;
    dividend |= (ota_t_udbl) u0;
    quotient = dividend / d;
    if( quotient > ( (ota_t_udbl) 1 << biL ) - 1 )
        quotient = ( (ota_t_udbl) 1 << biL ) - 1;

    if( r != NULL )
        *r = (ota_mpi_uint)( dividend - (quotient * d ) );

    return (ota_mpi_uint) quotient;
#else

    s = ota_clz( d );
    d = d << s;

    u1 = u1 << s;
    u1 |= ( u0 >> ( biL - s ) ) & ( -(ota_mpi_sint)s >> ( biL - 1 ) );
    u0 =  u0 << s;

    d1 = d >> biH;
    d0 = d & uint_halfword_mask;

    u0_msw = u0 >> biH;
    u0_lsw = u0 & uint_halfword_mask;

    q1 = u1 / d1;
    r0 = u1 - d1 * q1;

    while( q1 >= radix || ( q1 * d0 > radix * r0 + u0_msw ) )
    {
        q1 -= 1;
        r0 += d1;

        if ( r0 >= radix ) break;
    }

    rAX = ( u1 * radix ) + ( u0_msw - q1 * d );
    q0 = rAX / d1;
    r0 = rAX - q0 * d1;

    while( q0 >= radix || ( q0 * d0 > radix * r0 + u0_lsw ) )
    {
        q0 -= 1;
        r0 += d1;

        if ( r0 >= radix ) break;
    }

    if (r != NULL)
        *r = ( rAX * radix + u0_lsw - q0 * d ) >> s;

    quotient = q1 * radix + q0;

    return quotient;
#endif
}

int ota_mpi_div_mpi( ota_mpi *Q, ota_mpi *R, const ota_mpi *A, const ota_mpi *B )
{
    int ret;
    size_t i, n, t, k;
    ota_mpi X, Y, Z, T1, T2;

    if( ota_mpi_cmp_int( B, 0 ) == 0 )
        return( OTA_ERR_MPI_DIVISION_BY_ZERO );

    ota_mpi_init( &X ); ota_mpi_init( &Y ); ota_mpi_init( &Z );
    ota_mpi_init( &T1 ); ota_mpi_init( &T2 );

    if( ota_mpi_cmp_abs( A, B ) < 0 )
    {
        if( Q != NULL ) OTA_MPI_CHK( ota_mpi_lset( Q, 0 ) );
        if( R != NULL ) OTA_MPI_CHK( ota_mpi_copy( R, A ) );
        return( 0 );
    }

    OTA_MPI_CHK( ota_mpi_copy( &X, A ) );
    OTA_MPI_CHK( ota_mpi_copy( &Y, B ) );
    X.s = Y.s = 1;

    OTA_MPI_CHK( ota_mpi_grow( &Z, A->n + 2 ) );
    OTA_MPI_CHK( ota_mpi_lset( &Z,  0 ) );
    OTA_MPI_CHK( ota_mpi_grow( &T1, 2 ) );
    OTA_MPI_CHK( ota_mpi_grow( &T2, 3 ) );

    k = ota_mpi_bitlen( &Y ) % biL;
    if( k < biL - 1 )
    {
        k = biL - 1 - k;
        OTA_MPI_CHK( ota_mpi_shift_l( &X, k ) );
        OTA_MPI_CHK( ota_mpi_shift_l( &Y, k ) );
    }
    else k = 0;

    n = X.n - 1;
    t = Y.n - 1;
    OTA_MPI_CHK( ota_mpi_shift_l( &Y, biL * ( n - t ) ) );

    while( ota_mpi_cmp_mpi( &X, &Y ) >= 0 )
    {
        Z.p[n - t]++;
        OTA_MPI_CHK( ota_mpi_sub_mpi( &X, &X, &Y ) );
    }
    OTA_MPI_CHK( ota_mpi_shift_r( &Y, biL * ( n - t ) ) );

    for( i = n; i > t ; i-- )
    {
        if( X.p[i] >= Y.p[t] )
            Z.p[i - t - 1] = ~0;
        else
        {
            Z.p[i - t - 1] = ota_int_div_int( X.p[i], X.p[i - 1],
                                                            Y.p[t], NULL);
        }

        Z.p[i - t - 1]++;
        do
        {
            Z.p[i - t - 1]--;

            OTA_MPI_CHK( ota_mpi_lset( &T1, 0 ) );
            T1.p[0] = ( t < 1 ) ? 0 : Y.p[t - 1];
            T1.p[1] = Y.p[t];
            OTA_MPI_CHK( ota_mpi_mul_int( &T1, &T1, Z.p[i - t - 1] ) );

            OTA_MPI_CHK( ota_mpi_lset( &T2, 0 ) );
            T2.p[0] = ( i < 2 ) ? 0 : X.p[i - 2];
            T2.p[1] = ( i < 1 ) ? 0 : X.p[i - 1];
            T2.p[2] = X.p[i];
        }
        while( ota_mpi_cmp_mpi( &T1, &T2 ) > 0 );

        OTA_MPI_CHK( ota_mpi_mul_int( &T1, &Y, Z.p[i - t - 1] ) );
        OTA_MPI_CHK( ota_mpi_shift_l( &T1,  biL * ( i - t - 1 ) ) );
        OTA_MPI_CHK( ota_mpi_sub_mpi( &X, &X, &T1 ) );

        if( ota_mpi_cmp_int( &X, 0 ) < 0 )
        {
            OTA_MPI_CHK( ota_mpi_copy( &T1, &Y ) );
            OTA_MPI_CHK( ota_mpi_shift_l( &T1, biL * ( i - t - 1 ) ) );
            OTA_MPI_CHK( ota_mpi_add_mpi( &X, &X, &T1 ) );
            Z.p[i - t - 1]--;
        }
    }

    if( Q != NULL )
    {
        OTA_MPI_CHK( ota_mpi_copy( Q, &Z ) );
        Q->s = A->s * B->s;
    }

    if( R != NULL )
    {
        OTA_MPI_CHK( ota_mpi_shift_r( &X, k ) );
        X.s = A->s;
        OTA_MPI_CHK( ota_mpi_copy( R, &X ) );

        if( ota_mpi_cmp_int( R, 0 ) == 0 )
            R->s = 1;
    }

cleanup:

    ota_mpi_free( &X ); ota_mpi_free( &Y ); ota_mpi_free( &Z );
    ota_mpi_free( &T1 ); ota_mpi_free( &T2 );

    return( ret );
}

int ota_mpi_mod_mpi( ota_mpi *R, const ota_mpi *A, const ota_mpi *B )
{
    int ret;

    if( ota_mpi_cmp_int( B, 0 ) < 0 )
        return( OTA_ERR_MPI_NEGATIVE_VALUE );

    OTA_MPI_CHK( ota_mpi_div_mpi( NULL, R, A, B ) );

    while( ota_mpi_cmp_int( R, 0 ) < 0 )
      OTA_MPI_CHK( ota_mpi_add_mpi( R, R, B ) );

    while( ota_mpi_cmp_mpi( R, B ) >= 0 )
      OTA_MPI_CHK( ota_mpi_sub_mpi( R, R, B ) );

cleanup:

    return( ret );
}

static void ota_mpi_montg_init( ota_mpi_uint *mm, const ota_mpi *N )
{
    ota_mpi_uint x, m0 = N->p[0];
    unsigned int i;

    x  = m0;
    x += ( ( m0 + 2 ) & 4 ) << 1;

    for( i = biL; i >= 8; i /= 2 )
        x *= ( 2 - ( m0 * x ) );

    *mm = ~x + 1;
}

static int ota_mpi_montmul( ota_mpi *A, const ota_mpi *B, const ota_mpi *N, ota_mpi_uint mm,
                         const ota_mpi *T )
{
    size_t i, n, m;
    ota_mpi_uint u0, u1, *d;

    if( T->n < N->n + 1 || T->p == NULL )
        return( OTA_ERR_MPI_BAD_INPUT_DATA );

    memset( T->p, 0, T->n * ciL );

    d = T->p;
    n = N->n;
    m = ( B->n < n ) ? B->n : n;

    for( i = 0; i < n; i++ )
    {
        /*
         * T = (T + u0*B + u1*N) / 2^biL
         */
        u0 = A->p[i];
        u1 = ( d[0] + u0 * B->p[0] ) * mm;

        ota_mpi_mul_hlp( m, B->p, d, u0 );
        ota_mpi_mul_hlp( n, N->p, d, u1 );

        *d++ = u0; d[n + 1] = 0;
    }

    memcpy( A->p, d, ( n + 1 ) * ciL );

    if( ota_mpi_cmp_abs( A, N ) >= 0 )
        ota_mpi_sub_hlp( n, N->p, A->p );
    else
        /* prevent timing attacks */
        ota_mpi_sub_hlp( n, A->p, T->p );

    return( 0 );
}

static int ota_mpi_montred( ota_mpi *A, const ota_mpi *N, ota_mpi_uint mm, const ota_mpi *T )
{
    ota_mpi_uint z = 1;
    ota_mpi U;

    U.n = U.s = (int) z;
    U.p = &z;

    return( ota_mpi_montmul( A, &U, N, mm, T ) );
}

int ota_mpi_exp_mod( ota_mpi *X, const ota_mpi *A, const ota_mpi *E, const ota_mpi *N, ota_mpi *_RR )
{
    int ret;
    unsigned int wbits, wsize, one = 1;
    unsigned int i, j, nblimbs;
    unsigned int bufsize, nbits;
    ota_mpi_uint ei, mm, state;
    ota_mpi RR, T, W[ 2 << OTA_MPI_WINDOW_SIZE ], Apos;
    int neg;

    if( ota_mpi_cmp_int( N, 0 ) < 0 || ( N->p[0] & 1 ) == 0 )
        return( OTA_ERR_MPI_BAD_INPUT_DATA );

    if( ota_mpi_cmp_int( E, 0 ) < 0 )
        return( OTA_ERR_MPI_BAD_INPUT_DATA );

    /*
     * Init temps and window size
     */
    ota_mpi_montg_init( &mm, N );
    ota_mpi_init( &RR ); 
    ota_mpi_init( &T );
    ota_mpi_init( &Apos );
    memset( W, 0, sizeof( W ) );

    i = ota_mpi_bitlen( E );

    wsize = ( i > 671 ) ? 6 : ( i > 239 ) ? 5 :
            ( i >  79 ) ? 4 : ( i >  23 ) ? 3 : 1;

    if( wsize > OTA_MPI_WINDOW_SIZE )
        wsize = OTA_MPI_WINDOW_SIZE;

    j = N->n + 1;
    OTA_MPI_CHK( ota_mpi_grow( X, j ) );
    OTA_MPI_CHK( ota_mpi_grow( &W[1],  j ) );
    OTA_MPI_CHK( ota_mpi_grow( &T, j * 2 ) );

    neg = ( A->s == -1 );
    if( neg )
    {
        OTA_MPI_CHK( ota_mpi_copy( &Apos, A ) );
        Apos.s = 1;
        A = &Apos;
    }

    if( _RR == NULL || _RR->p == NULL )
    {
        OTA_MPI_CHK( ota_mpi_lset( &RR, 1 ) );
        OTA_MPI_CHK( ota_mpi_shift_l( &RR, N->n * 2 * biL ) );
        OTA_MPI_CHK( ota_mpi_mod_mpi( &RR, &RR, N ) );

        if( _RR != NULL )
            memcpy( _RR, &RR, sizeof( ota_mpi ) );
    }
    else
        memcpy( &RR, _RR, sizeof( ota_mpi ) );

    if( ota_mpi_cmp_mpi( A, N ) >= 0 )
        OTA_MPI_CHK( ota_mpi_mod_mpi( &W[1], A, N ) );
    else
        OTA_MPI_CHK( ota_mpi_copy( &W[1], A ) );

    OTA_MPI_CHK( ota_mpi_montmul( &W[1], &RR, N, mm, &T ) );

    OTA_MPI_CHK( ota_mpi_copy( X, &RR ) );
    OTA_MPI_CHK( ota_mpi_montred( X, N, mm, &T ) );

    if( wsize > 1 )
    {
        j =  one << ( wsize - 1 );

        OTA_MPI_CHK( ota_mpi_grow( &W[j], N->n + 1 ) );
        OTA_MPI_CHK( ota_mpi_copy( &W[j], &W[1]    ) );

        for( i = 0; i < wsize - 1; i++ )
            OTA_MPI_CHK( ota_mpi_montmul( &W[j], &W[j], N, mm, &T ) );

        for( i = j + 1; i < ( one << wsize ); i++ )
        {
            OTA_MPI_CHK( ota_mpi_grow( &W[i], N->n + 1 ) );
            OTA_MPI_CHK( ota_mpi_copy( &W[i], &W[i - 1] ) );

            OTA_MPI_CHK( ota_mpi_montmul( &W[i], &W[1], N, mm, &T ) );
        }
    }

    nblimbs = E->n;
    bufsize = 0;
    nbits   = 0;
    wbits   = 0;
    state   = 0;

    while( 1 )
    {
        if( bufsize == 0 )
        {
            if( nblimbs == 0 )
                break;

            nblimbs--;

            bufsize = sizeof( ota_mpi_uint ) << 3;
        }

        bufsize--;

        ei = (E->p[nblimbs] >> bufsize) & 1;

        if( ei == 0 && state == 0 )
            continue;

        if( ei == 0 && state == 1 )
        {
            /*
             * out of window, square X
             */
            OTA_MPI_CHK( ota_mpi_montmul( X, X, N, mm, &T ) );
            continue;
        }

        state = 2;

        nbits++;
        wbits |= ( ei << ( wsize - nbits ) );

        if( nbits == wsize )
        {
            /*
             * X = X^wsize R^-1 mod N
             */
            for( i = 0; i < wsize; i++ )
                OTA_MPI_CHK( ota_mpi_montmul( X, X, N, mm, &T ) );

            OTA_MPI_CHK( ota_mpi_montmul( X, &W[wbits], N, mm, &T ) );

            state--;
            nbits = 0;
            wbits = 0;
        }
    }

    /*
     * process the remaining bits
     */
    for( i = 0; i < nbits; i++ )
    {
        OTA_MPI_CHK( ota_mpi_montmul( X, X, N, mm, &T ) );

        wbits <<= 1;

        if( ( wbits & ( one << wsize ) ) != 0 )
            OTA_MPI_CHK( ota_mpi_montmul( X, &W[1], N, mm, &T ) );
    }

    OTA_MPI_CHK( ota_mpi_montred( X, N, mm, &T ) );

    if( neg )
    {
        X->s = -1;
        OTA_MPI_CHK( ota_mpi_add_mpi( X, N, X ) );
    }

cleanup:

    for( i = ( one << ( wsize - 1 ) ); i < ( one << wsize ); i++ )
        ota_mpi_free( &W[i] );

    ota_mpi_free( &W[1] ); ota_mpi_free( &T ); ota_mpi_free( &Apos );

    if( _RR == NULL || _RR->p == NULL )
        ota_mpi_free( &RR );

    return( ret );
}


