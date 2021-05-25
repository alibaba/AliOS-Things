#include "rtw_opt_crypto_ssl.h"
#include "osdep_service.h"
/****************************************************************************************************


                        Function of Initialization


****************************************************************************************************/
int rtw_platform_set_calloc_free( void * (*calloc_func)( size_t, size_t ),
                              void (*free_func)( void * ) )
{
	//mbedtls_platform_set_calloc_free(calloc_func, free_func);
}
/****************************************************************************************************


                        Function of ECC Algorithm


****************************************************************************************************/
static const unsigned char secp224r1_a[]={ 
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFE};

//***************************************************************************************************
// \brief           initialization of ecp curve with group id
//
// \param ecc       pointer of ecc curve structure
// \param group_id  group_id defined by spec 12.4
//
// \return         0  if successful,
//                 -1 if group_id is not supported
//***************************************************************************************************
int rtw_crypto_ecc_init(sae_ecc_crypto *ecc,unsigned char group_id)
{
	int ret = 0;
	mbedtls_ecp_group_init(ecc);
	switch (group_id){
		case 19:
		      mbedtls_ecp_group_load( ecc, MBEDTLS_ECP_DP_SECP256R1);
			  mbedtls_mpi_init(&ecc->A);
			  mbedtls_mpi_copy(&ecc->A,&ecc->P);
			  mbedtls_mpi_set_bit(&ecc->A,0,0);
			  mbedtls_mpi_set_bit(&ecc->A,1,0);
			  break;
		case 20:
	 		  mbedtls_ecp_group_load( ecc, MBEDTLS_ECP_DP_SECP384R1);
			  mbedtls_mpi_init(&ecc->A);
			  mbedtls_mpi_copy(&ecc->A,&ecc->P);
			  mbedtls_mpi_set_bit(&ecc->A,0,0);
			  mbedtls_mpi_set_bit(&ecc->A,1,0);
			  break;
		case 21:
			  mbedtls_ecp_group_load( ecc, MBEDTLS_ECP_DP_SECP521R1);
			  mbedtls_mpi_init(&ecc->A);
			  mbedtls_mpi_copy(&ecc->A,&ecc->P);
			  mbedtls_mpi_set_bit(&ecc->A,0,0);
			  mbedtls_mpi_set_bit(&ecc->A,1,0);
			  break;
		case 25:
			  mbedtls_ecp_group_load( ecc, MBEDTLS_ECP_DP_SECP192R1);
			  mbedtls_mpi_init(&ecc->A);
			  mbedtls_mpi_copy(&ecc->A,&ecc->P);
			  mbedtls_mpi_set_bit(&ecc->A,0,0);
			  mbedtls_mpi_set_bit(&ecc->A,1,0);
			  break;
		case 26:
			  mbedtls_ecp_group_load( ecc, MBEDTLS_ECP_DP_SECP224R1);
			  mbedtls_mpi_init(&ecc->A);
			  mbedtls_mpi_read_binary(&ecc->A,secp224r1_a,28);
			  break;
		default:
			  printf("\r\nmbedtls_ecc_init: no available ecc type: %d \n",group_id);
			  ret = -1;
			  break;
	}
	return ret;
}

//***************************************************************************************************
// \brief           free of ecp curve
//
// \param ecc       pointer of ecc curve structure
//
// \return          void
//
//***************************************************************************************************
void rtw_crypto_ecc_free(sae_ecc_crypto *ecc)
{
	mbedtls_mpi_free(&ecc->A);
	mbedtls_ecp_group_free(ecc);
}

//***************************************************************************************************
// \brief            get Parameter A of ECC
//
// \param ecc        pointer of ecc curve structure
// \param a          pointer of big number of a 
//
// \return  		 0 if successful
//                   -1 if fail
//
//***************************************************************************************************
int rtw_crypto_ecc_get_param_a(sae_ecc_crypto *ecc, sae_crypto_bignum *a)
{
	int ret = 0;
	if((ecc == NULL) || (a == NULL)){
		ret = -1;
		return ret;
	}

	MBEDTLS_MPI_CHK(mbedtls_mpi_copy(a,&ecc->A));

cleanup:	
	return ret;
}


//***************************************************************************************************
// \brief           get Parameter B of ECC
//
// \param ecc        pointer of ecc curve structure
// \param a          pointer of big number of a 
//
// \return  		0 if successful
//                  -1 if fail
//
//***************************************************************************************************
int rtw_crypto_ecc_get_param_b(sae_ecc_crypto *ecc, sae_crypto_bignum *b)
{
	int ret = 0;
	if((ecc == NULL) || (b == NULL)){
		ret = -1;
		return ret;
	}

	MBEDTLS_MPI_CHK(mbedtls_mpi_copy(b,&ecc->B));

cleanup:	
	return ret;
}

//***************************************************************************************************
// \brief           get Order of ECC
//
// \param ecc        pointer of ecc curve structure
// \param n          pointer of big number of a 
//
// \return  		0 if successful
//                  -1 if fail
//
//***************************************************************************************************
int rtw_crypto_ecc_get_param_order(sae_ecc_crypto *ecc, sae_crypto_bignum *n)
{
	int ret = 0;
	if((ecc == NULL) || (n == NULL)){
		ret = -1;
		return ret;
	}

	MBEDTLS_MPI_CHK(mbedtls_mpi_copy(n,&ecc->N));

cleanup:	
	return ret;
}


//***************************************************************************************************
// \brief           get Parameter B of ECC
//
// \param ecc        pointer of ecc curve structure
// \param a          pointer of big number of a 
//
// \return  		0 if successful
//                  -1 if fail
//
//***************************************************************************************************
int rtw_crypto_ecc_get_param_prime(sae_ecc_crypto *ecc, sae_crypto_bignum *prime)
{
	int ret = 0;
	if((ecc == NULL) || (prime == NULL)){
		ret = -1;
		return ret;
	}
	
	MBEDTLS_MPI_CHK(mbedtls_mpi_copy(prime,&ecc->P));

cleanup:	
	return ret;
}


/*********************************************************************************************
// \brief:     import point from bignum

// \param ecc: pointer of ecc
// \param x:   input point x
// \param y:   input point y 
// \param p:   import point
//          
return:    0 if successful
		   -1 if failed
*********************************************************************************************/
int rtw_crypto_ecc_point_read_bignum(sae_ecc_crypto *ecc,sae_crypto_bignum *x,sae_crypto_bignum *y,sae_ecc_point *p)
{
	int ret = 0;
	unsigned char *buf = NULL;
	unsigned int buf_len;
	unsigned int prime_len;

	if((ecc == NULL) || (x == NULL) || (y == NULL) || (p == NULL)){
		ret = -1;
		goto cleanup;
	}

	prime_len = mbedtls_mpi_size(&ecc->P);
	
	buf_len = 2*prime_len + 1;

	buf = rtw_zmalloc(buf_len);

	if(buf == NULL){
		ret = -1;
		goto cleanup;
	}

	buf[0] = 0x04;
	
	if(rtw_crypto_bignum_write_binary(x,buf + 1,prime_len) < 0){
		ret = -1;
		goto cleanup;
	}

	if(rtw_crypto_bignum_write_binary(y,buf + 1 + prime_len,prime_len) < 0){
		ret = -1;
		goto cleanup;
	}

	ret = mbedtls_ecp_point_read_binary(ecc,p,buf,buf_len);

		
cleanup:
	if(buf)
		rtw_mfree(buf,buf_len);
	return ret;
	
}

/*********************************************************************************************
brief:     export point to bignum 

Parameter: ecc: pointer of ecc
           x:  output point x
           y:  output point y
           p:  export point of ecc
           
return:    0 if successful
		   -1 if failed
*********************************************************************************************/
int rtw_crypto_ecc_point_write_bignum(sae_ecc_crypto *ecc,sae_crypto_bignum *x,sae_crypto_bignum *y,sae_ecc_point *p)
{
	int ret = 0;
	unsigned char *buf = NULL;
	unsigned int buf_len;
	unsigned int prime_len;
	unsigned int out_len = 0;


	if((ecc == NULL) || (x == NULL) || (y == NULL) || (p == NULL)){
		ret = -1;
		goto cleanup;
	}

	prime_len = mbedtls_mpi_size(&ecc->P);

	buf_len = 2*prime_len + 1;

	buf = rtw_zmalloc(buf_len);

	if(buf == NULL){
		ret = -1;
		goto cleanup;
	}

	if(mbedtls_ecp_point_write_binary(ecc,p,MBEDTLS_ECP_PF_UNCOMPRESSED,&out_len,buf,buf_len) <0 ){
		ret = -1;
		goto cleanup;
	}

	if(rtw_crypto_bignum_read_binary(x,buf + 1, prime_len) < 0){
		ret = -1;
		goto cleanup;
	}
	
	
	if(rtw_crypto_bignum_read_binary(y,buf + 1 + prime_len, prime_len) < 0){
		ret = -1;
		goto cleanup;
	}
	
		
cleanup:
	if(buf)
		rtw_mfree(buf,buf_len);
	return ret;
	
}


//***************************************************************************************************
// \brief			Initialize one ecc point
//
// \param point		pointer to ecc point
//
// \return			void
//					
//***************************************************************************************************
void rtw_crypto_ecc_point_init(sae_ecc_point *point)
{
	mbedtls_ecp_point_init(point);
}

//***************************************************************************************************
// \brief			Free one ecc point
//
// \param point		pointer to ecc point
//
// \return			void
//					
//***************************************************************************************************
void rtw_crypto_ecc_point_free(sae_ecc_point *point)
{
	mbedtls_ecp_point_free(point);
}

//***************************************************************************************************
// \brief			check the ecc point is the infinity point
//
// \param point		pointer to ecc point
//
// \return			0  none-infinity 
//					1  infinity				
//***************************************************************************************************
int rtw_crypto_ecc_point_is_at_infinity(sae_ecc_point *point)
{
	return mbedtls_ecp_is_zero(point);
}

//***************************************************************************************************
// \brief			R = m * P 
//
// \param ecc		pointer of ecc curve structure
// \param R			pointer to the result ecc point
// \param m			pointer to a bignum
// \param P         pointer to the source 
// \return			0  success
//					-1 fail
//***************************************************************************************************
int rtw_crypto_ecc_point_mul_bignum(sae_ecc_crypto *ecc,sae_ecc_point *R,sae_crypto_bignum *m,sae_ecc_point *P)
{
	int ret = 0;	
	MBEDTLS_MPI_CHK( mbedtls_ecp_mul( ecc, R, m, P, NULL, NULL ));
cleanup:
	if(ret < 0)
		ret = -1;
	return ret;
}

//***************************************************************************************************
// \brief			justify if the ecc point is on the curve
//
// \param ecc		pointer of ecc curve structure
// \param P         pointer to the ecc point 

// \return			0  yes
//					-1 no 
//***************************************************************************************************
int rtw_crypto_ecc_point_is_on_curve(sae_ecc_crypto *ecc,sae_ecc_point *P)
{
	int ret = 0;	

	MBEDTLS_MPI_CHK(mbedtls_ecp_check_pubkey(ecc,P));

cleanup:	
	if(ret < 0)
		ret = -1;
	return ret;
}


//***************************************************************************************************
// \brief			X = A + B 
//
// \param ecc		pointer of ecc curve structure
// \param X			pointer of destination point
// \param A			pointer of left hand point
// \param B         pointer of right hand point
//
// \return			0  success
//					-1 fail
//***************************************************************************************************
int rtw_crypto_ecc_point_add_point(sae_ecc_crypto *ecc,sae_ecc_point *X,sae_ecc_point *A,sae_ecc_point *B)
{
	int ret = 0;
	mbedtls_mpi one;

	mbedtls_mpi_init(&one);
	
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&one,1));
    MBEDTLS_MPI_CHK(mbedtls_ecp_muladd(ecc,X,&one,A,&one,B));

cleanup:
	if(ret < 0)
		ret = -1;
	mbedtls_mpi_free(&one);
	return ret;
}

//***************************************************************************************************
// \brief			compare two ecc points
//
// \param ecc		pointer of ecc curve structure
// \param P1		pointer of left hand point
// \param P2        pointer of right hand point
//
// \return			0  P1 = P2
//					<0 P1 != P2
//***************************************************************************************************
int rtw_crypto_ecc_point_cmp(sae_ecc_point *P1,sae_ecc_point *P2)
{
	return mbedtls_ecp_point_cmp(P1,P2);
}


/****************************************************************************************************


                        Function of Big number Operation and computation
                        

****************************************************************************************************/


//***************************************************************************************************
// \brief			Initialize one Big number
//                  This just makes it ready to be set or freed, but does not define a value for the MPI
//
// \param X		    pointer of one big number to be initialized
//
// \return			void					
//
//***************************************************************************************************
void rtw_crypto_bignum_init(sae_crypto_bignum *X)
{
	mbedtls_mpi_init(X);
}

//***************************************************************************************************
// \brief			unallocate one big number
//
// \param X		    pointer of big number to be unallocated
//
// \return			void					
//
//***************************************************************************************************
void rtw_crypto_bignum_free(sae_crypto_bignum *X)
{
	mbedtls_mpi_free(X);
}

//***************************************************************************************************
// \brief			Copy the contents of Y into X
//
// \param X		    pointer of destination big number
// \param Y			pointer of source big number
//
// \return			0 if successful
//					-1 if failed
//
//***************************************************************************************************
int rtw_crypto_bignum_copy(sae_crypto_bignum *X,sae_crypto_bignum *Y)
{
	int ret = 0;
	MBEDTLS_MPI_CHK(mbedtls_mpi_copy(X,Y));
	
cleanup:
	if((ret) < 0)
		ret = -1;
	return ret;
}


//***************************************************************************************************
// \brief			Return the number of bits up to and including the most significant '1' bit'
//
// \param X		    pointer of big number to use
//
// \return			length of bit					
//
//***************************************************************************************************
size_t rtw_crypto_bignum_bitlen(sae_crypto_bignum *X)
{
	return mbedtls_mpi_bitlen(X);
}


//***************************************************************************************************
// \brief			Get a specific bit from X
//
// \param X		    pointer of big number to use
// \param pos		Zero-based index of the bit in X
//
// \return			either 0 or 1					
//
//***************************************************************************************************
size_t rtw_crypto_bignum_get_bit(sae_crypto_bignum *X,size_t pos)
{
	return mbedtls_mpi_get_bit(X,pos);
}


//***************************************************************************************************
// \brief			return the total size in bytes
//
// \param X		    pointer of big number to use
//
// \return			total size of big number in bytes				
//
//***************************************************************************************************
size_t rtw_crypto_bignum_size(sae_crypto_bignum *X)
{
	return mbedtls_mpi_size(X);
}


//***************************************************************************************************
// \brief			Import X from unsigned binary data, big endian
//
// \param X		    pointer of destination big number
// \param buf		pointer of input buffer
// \param buf_len   input buffer size
//
// \return			0 if successful				
//					-1 if failed
//
//***************************************************************************************************
int rtw_crypto_bignum_read_binary(sae_crypto_bignum *X,const unsigned char *buf, size_t buf_len)
{
	int ret = 0;
	MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(X,buf,buf_len));
		
cleanup:		
	if((ret)<0)
		ret = -1;

	return ret;
}


//***************************************************************************************************
// \brief			 Export X into unsigned binary data, big endian.
//					 Always fills the whole buffer, which will start with zeros if the number is smaller.
//
// \param X		    pointer of source big number
// \param buf		pointer of output buffer
// \param buf_len   output buffer size
//
// \return			0 if successful				
//					-1 if failed
//
//***************************************************************************************************
int rtw_crypto_bignum_write_binary(sae_crypto_bignum *X,unsigned char *buf, size_t buf_len)
{
	int ret = 0;
	MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(X,buf,buf_len));
		
cleanup:		
	if((ret)<0)
		ret = -1;

	return ret;
}

//***************************************************************************************************
// \brief			 Right-shift: X >>= count
//
// \param X		    pointer of big number to shift
// \param count		amount to shift
//
// \return			0 if successful				
//					-1 if failed
//
//***************************************************************************************************
int rtw_crypto_bignum_shift_r(sae_crypto_bignum *X,size_t count)
{
	int ret = 0;
	MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(X,count));
		
cleanup:		
	if((ret)<0)
		ret = -1;

	return ret;
}

//***************************************************************************************************
// \brief			Compare signed values
//
// \param X		    pointer of left hand big number
// \param Y		    pointer of right hand big number
//
// \return			1 if X is greater than Y				
//					-1 if X is lesser than Y
//					0 if X is equal to Y
//
//***************************************************************************************************
int rtw_crypto_bignum_cmp_bignum(sae_crypto_bignum *X,sae_crypto_bignum *Y)
{
	return mbedtls_mpi_cmp_mpi(X,Y);
}

//***************************************************************************************************
// \brief			Compare signed values
//
// \param X		    pointer of left hand big number
// \param z		    The integer value to compare to
//
// \return			1 if X is greater than z				
//					-1 if X is lesser than z
//					0 if X is equal to z
//
//***************************************************************************************************
int rtw_crypto_bignum_cmp_int(sae_crypto_bignum *X,int z)
{
	return mbedtls_mpi_cmp_int(X,z);
}

//***************************************************************************************************
// \brief			Signed addition:X = A + B
//
// \param X		    pointer of destination big number
// \param A		    pointer of left hand big number
// \param B			pointer of right hand big number
//
// \return			0 if successful					
//					-1 if failed
//
//***************************************************************************************************
int rtw_crypto_bignum_add_bignum(sae_crypto_bignum *X,sae_crypto_bignum *A,sae_crypto_bignum *B)
{
	int ret = 0;
	MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(X,A,B));
		
cleanup:		
	if((ret)<0)
		ret = -1;

	return ret;
}


//***************************************************************************************************
// \brief			Signed addition: X = A + b
//
// \param X		    pointer of destination big number
// \param A		    pointer of left hand big number
// \param b			The integer value to add
//
// \return			0 if successful					
//					-1 if failed
//
//***************************************************************************************************
int rtw_crypto_bignum_add_int(sae_crypto_bignum *X,sae_crypto_bignum *A,int b)
{
	int ret = 0;
	MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(X,A,b));
		
cleanup:		
	if((ret)<0)
		ret = -1;

	return ret;
}

//***************************************************************************************************
// \brief			Signed addition: X = A - B
//
// \param X		    pointer of destination big number
// \param A		    pointer of left hand big number
// \param B			pointer of right hand big number
//
// \return			0 if successful					
//					-1 if failed
//
//***************************************************************************************************
int rtw_crypto_bignum_sub_bignum(sae_crypto_bignum *X,sae_crypto_bignum *A,sae_crypto_bignum *B)
{
	int ret = 0;
	MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(X,A,B));
		
cleanup:		
	if((ret)<0)
		ret = -1;

	return ret;
}


//***************************************************************************************************
// \brief			Signed addition: X = A - b
//
// \param X		    point of destination big number
// \param A		    point of left hand big number
// \param b			The integer value to subtract
//
// \return			0 if successful					
//					-1 if failed
//
//***************************************************************************************************
int rtw_crypto_bignum_sub_int(sae_crypto_bignum *X,sae_crypto_bignum *A,int b)
{
	int ret = 0;
	MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(X,A,b));
		
cleanup:		
	if((ret)<0)
		ret = -1;

	return ret;
}

//***************************************************************************************************
// \brief			Division by big number: A = Q * B + R
//
// \param Q		    pointer of destination big number for the quotient
// \param R		    pointer of destination big number for the rest value
// \param A			pointer of left hand big number
// \param B			pointer of right hand big number
//
// \return			0 if successful					
//					-1 if failed
// 
// \note 			Either Q or R can be NULL.
//***************************************************************************************************
int rtw_crypto_bignum_div_bignum(sae_crypto_bignum *Q,sae_crypto_bignum *R,sae_crypto_bignum *A,sae_crypto_bignum *B)
{
	int ret = 0;
	MBEDTLS_MPI_CHK(mbedtls_mpi_div_mpi(Q,R,A,B));
		
cleanup:		
	if((ret)<0)
		ret = -1;

	return ret;
}


//***************************************************************************************************
// \brief			X = A mod N
//
// \param X		    pointer of destination big number
// \param A		    pointer of left hand big number
// \param N			pointer of modular big number
//
// \return			0 if successful					
//					-1 if failed
//
//***************************************************************************************************
int rtw_crypto_bignum_mod_bignum(sae_crypto_bignum *X,sae_crypto_bignum *A,sae_crypto_bignum *N)
{
	int ret = 0;
	MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(X,A,N));
		
cleanup:		
	if((ret)<0)
		ret = -1;

	return ret;
}

//***************************************************************************************************
// \brief			X = (A + B) mod N
//
// \param X		    pointer of destination big number
// \param A		    pointer of left hand big number
// \param B 		pointer of right hand big number
// \param N			pointer of modular big number
//
// \return			0 if successful					
//					-1 if failed
//
//***************************************************************************************************
int rtw_crypto_bignum_add_mod(sae_crypto_bignum *X,sae_crypto_bignum *A,sae_crypto_bignum *B,sae_crypto_bignum *N)
{
	int ret = 0;
	MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(X,A,B));
	MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(X,X,N));
		
cleanup:		
	if((ret)<0)
		ret = -1;

	return ret;
}

//***************************************************************************************************
// \brief			X = (A * B) mod N
//
// \param X		    pointer of destination big number
// \param A		    pointer of left hand big number
// \param B 		pointer of right hand big number
// \param N			pointer of modular big number
//
// \return			0 if successful					
//					-1 if failed
//
//***************************************************************************************************
int rtw_crypto_bignum_mul_mod(sae_crypto_bignum *X,sae_crypto_bignum *A,sae_crypto_bignum *B,sae_crypto_bignum *N)
{
	int ret = 0;
	MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(X,A,B));
	MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(X,X,N));
		
cleanup:		
	if((ret)<0)
		ret = -1;

	return ret;
}

//***************************************************************************************************
// \brief			X = (A ^ B) mod N
//
// \param X		    pointer of destination big number
// \param A		    pointer of left hand big number
// \param B 		pointer of right hand big number
// \param N			pointer of modular big number
//
// \return			0 if successful					
//					-1 if failed
//
//***************************************************************************************************
int rtw_crypto_bignum_exp_mod(sae_crypto_bignum *X,sae_crypto_bignum *A,sae_crypto_bignum *B,sae_crypto_bignum *N)
{
	int ret = 0;
	MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(X,A,B,N,NULL));
		
cleanup:		
	if((ret)<0)
		ret = -1;

	return ret;
}


//***************************************************************************************************
// \brief		assign Y ==> X
//
// \param X     pointer to bignum X
// \param Y		pointer to bignum Y
// \param inv   assign or not
//
// \return		0 if successful
//				-1 if failed				
//***************************************************************************************************
int rtw_crypto_bignum_assign(sae_crypto_bignum *X, sae_crypto_bignum *Y,unsigned char inv)
{
	int ret = 0;

	MBEDTLS_MPI_CHK(mbedtls_mpi_safe_cond_assign(X,Y,inv));
		
cleanup:
	if(ret < 0)
		ret = -1;
	return ret;
}

//***************************************************************************************************
// \brief		X = A^-1 mod N
//
// \param X     pointer to bignum X
// \param Y		pointer to bignum Y
// \param inv   assign or not
//
// \return		0 if successful
//				-1 if failed				
//***************************************************************************************************
int rtw_crypto_bignum_inverse(sae_crypto_bignum *X, sae_crypto_bignum *A,sae_crypto_bignum *N)
{
	int ret = 0;

	MBEDTLS_MPI_CHK(mbedtls_mpi_inv_mod(X,A,N));
		
cleanup:
	if(ret < 0)
		ret = -1;
	return ret;
}

