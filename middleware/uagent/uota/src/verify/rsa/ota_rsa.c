#include "ota_pub.h"
#include "ota_verify.h"
#include "ota_rsa.h"
#include "ota_oid.h"
#include "ota_asn1.h"

#define OTA_OID_SIZE(x) (sizeof(x) - 1)
#define ADD_LEN(s)      s, OTA_OID_SIZE(s)

typedef struct {
  ota_oid_descriptor_t    descriptor;
  ota_md_type_t           md_alg;
} ota_oid_md_alg_t;

static const ota_oid_md_alg_t oid_md_alg[] =
{
  {
      { ADD_LEN( OTA_OID_DIGEST_ALG_MD5 ),       "id-md5",       "MD5" },
      OTA_MD_MD5,
  },
  {
      { ADD_LEN( OTA_OID_DIGEST_ALG_SHA256 ),    "id-sha256",    "SHA-256" },
      OTA_MD_SHA256,
  },
  {
      { NULL, 0, NULL, NULL },
      OTA_MD_NONE,
  },
};

static const ota_oid_md_alg_t * ota_oid_md_alg_from_asn1( const ota_asn1_buf *oid )
{
  const ota_oid_md_alg_t *p = oid_md_alg;
  const ota_oid_descriptor_t *cur = (const ota_oid_descriptor_t *) p;
  if( p == NULL || oid == NULL ) return( NULL );
  while( cur->asn1 != NULL ) {
      if( cur->asn1_len == oid->len &&
          memcmp( cur->asn1, oid->p, oid->len ) == 0 ) {
          return( p );
      }
      p++;
      cur = (const ota_oid_descriptor_t *) p;
  }
  return( NULL );
}

int ota_oid_get_md_alg( const ota_asn1_buf *oid, ota_md_type_t * md_alg )
{
  const ota_oid_md_alg_t *data = ota_oid_md_alg_from_asn1(oid);
  if( data == NULL ) {
      return -1;
  }           
  *md_alg = data->md_alg;
  return( 0 );
}

//asn1parse.c
int ota_asn1_get_len( unsigned char **p,
                const unsigned char *end,
                unsigned int *len )
{
  if( ( end - *p ) < 1 )
      return( OTA_ERR_ASN1_OUT_OF_DATA );

  if( ( **p & 0x80 ) == 0 )
      *len = *(*p)++;
  else
  {
      switch( **p & 0x7F )
      {
      case 1:
          if( ( end - *p ) < 2 )
              return( OTA_ERR_ASN1_OUT_OF_DATA );
          *len = (*p)[1];
          (*p) += 2;
          break;
      case 2:
          if( ( end - *p ) < 3 )
              return( OTA_ERR_ASN1_OUT_OF_DATA );

          *len = ( (size_t)(*p)[1] << 8 ) | (*p)[2];
          (*p) += 3;
          break;
      case 3:
          if( ( end - *p ) < 4 )
              return( OTA_ERR_ASN1_OUT_OF_DATA );
          *len = ( (size_t)(*p)[1] << 16 ) |
                 ( (size_t)(*p)[2] << 8  ) | (*p)[3];
          (*p) += 4;
          break;
      case 4:
          if( ( end - *p ) < 5 )
              return( OTA_ERR_ASN1_OUT_OF_DATA );
          *len = ( (size_t)(*p)[1] << 24 ) | ( (size_t)(*p)[2] << 16 ) |
                 ( (size_t)(*p)[3] << 8  ) |           (*p)[4];
          (*p) += 5;
          break;
      default:
          return( OTA_ERR_ASN1_INVALID_LENGTH );
      }
  }

  if( *len > (size_t) ( end - *p ) )
      return( OTA_ERR_ASN1_OUT_OF_DATA );

  return( 0 );
}

int ota_asn1_get_tag( unsigned char **p,
                const unsigned char *end,
                unsigned int*len, int tag )
{
  if( ( end - *p ) < 1 )
      return( OTA_ERR_ASN1_OUT_OF_DATA );

  if( **p != tag )
      return( OTA_ERR_ASN1_UNEXPECTED_TAG );

  (*p)++;

  return( ota_asn1_get_len( p, end, len ) );
}

void ota_rsa_init( ota_rsa_context *ctx,
             int padding,
             int hash_id )
{
  memset( ctx, 0, sizeof( ota_rsa_context ) );

  ota_rsa_set_padding( ctx, padding, hash_id );
}

/*
* Set padding for an existing RSA context
*/
void ota_rsa_set_padding( ota_rsa_context *ctx, int padding, int hash_id )
{
  ctx->padding = padding;
  ctx->hash_id = hash_id;
}


int ota_rsa_check_pubkey( const ota_rsa_context *ctx )
{
  if( !ctx->N.p || !ctx->E.p )
      return( OTA_ERR_RSA_KEY_CHECK_FAILED );

  if( ( ctx->N.p[0] & 1 ) == 0 ||
      ( ctx->E.p[0] & 1 ) == 0 )
      return( OTA_ERR_RSA_KEY_CHECK_FAILED );

  if( ota_mpi_bitlen( &ctx->N ) < 128 ||
      ota_mpi_bitlen( &ctx->N ) > OTA_MPI_MAX_BITS )
      return( OTA_ERR_RSA_KEY_CHECK_FAILED );

  if( ota_mpi_bitlen( &ctx->E ) < 2 ||
      ota_mpi_cmp_mpi( &ctx->E, &ctx->N ) >= 0 )
      return( OTA_ERR_RSA_KEY_CHECK_FAILED );

  return( 0 );
}


int ota_rsa_public( ota_rsa_context *ctx,
              const unsigned char *input,
              unsigned char *output )
{
  int ret;
  unsigned int olen;
  ota_mpi T;

  ota_mpi_init( &T );

  OTA_MPI_CHK( ota_mpi_read_binary( &T, input, ctx->len ) );

  if( ota_mpi_cmp_mpi( &T, &ctx->N ) >= 0 ) {
      ret = OTA_ERR_MPI_BAD_INPUT_DATA;
      goto cleanup;
  }

  olen = ctx->len;
  OTA_MPI_CHK( ota_mpi_exp_mod( &T, &T, &ctx->E, &ctx->N, &ctx->RN ) );
  OTA_MPI_CHK( ota_mpi_write_binary( &T, output, olen ) );
  
cleanup:
  ota_mpi_free( &T );
  if( ret != 0 ) {
     return( OTA_ERR_RSA_PUBLIC_FAILED + ret );
  }

  return( 0 );
}


/*
* Implementation of the PKCS#1 v2.1 RSASSA-PKCS1-v1_5-VERIFY function
*/
int ota_rsa_rsassa_pkcs1_v15_verify( ota_rsa_context *ctx,
                               int (*f_rng)(void *, unsigned char *, unsigned int),
                               void *p_rng,
                               int mode,
                               ota_md_type_t md_alg,
                               unsigned int hashlen,
                               const unsigned char *hash,
                               const unsigned char *sig )
{
  int ret;
  unsigned int len, siglen, asn1_len;
  unsigned char *p, *end;
  ota_md_type_t msg_md_alg;
  const struct ota_md_info_t *md_info;
  ota_asn1_buf oid;
  unsigned char buf[OTA_MPI_MAX_SIZE];
  if(mode == OTA_RSA_PRIVATE && ctx->padding != OTA_RSA_PKCS_V15) {
      return( OTA_ERR_RSA_BAD_INPUT_DATA );
  }
  siglen = ctx->len;
  if(siglen < 16 || siglen > sizeof( buf )){
      return( OTA_ERR_RSA_BAD_INPUT_DATA );
  }
  ret = ota_rsa_public(ctx, sig, buf);
  if( ret != 0 ) {
      return( ret );
  }
  p = buf;
  if(*p++ != 0 || *p++ != OTA_RSA_SIGN) {
      return( OTA_ERR_RSA_INVALID_PADDING );
  }
  while(*p != 0)
  {
      if( p >= buf + siglen - 1 || *p != 0xFF ){
          return( OTA_ERR_RSA_INVALID_PADDING );
      }
      p++;
  }
  p++;
  len = siglen - ( p - buf );
  if(len == hashlen && md_alg == OTA_MD_NONE ) {
      if( memcmp(p, hash, hashlen) == 0 )
          return 0;
      else {
          return OTA_ERR_RSA_VERIFY_FAILED;
      }
  }
  md_info = ota_hash_info_from_type(md_alg);
  if(md_info == NULL) {
      return OTA_ERR_RSA_BAD_INPUT_DATA;
  }
  hashlen = ota_hash_get_size(md_info);

  end = p + len;
  /*
   * Parse the ASN.1 structure inside the PKCS#1 v1.5 structure
   */
  if((ret = ota_asn1_get_tag(&p, end, &asn1_len, OTA_ASN1_CONSTRUCTED | OTA_ASN1_SEQUENCE)) != 0){
      return OTA_ERR_RSA_VERIFY_FAILED;
  }

  if(asn1_len + 2 != len) {
      return OTA_ERR_RSA_VERIFY_FAILED;
  }
  if((ret = ota_asn1_get_tag( &p, end, &asn1_len, OTA_ASN1_CONSTRUCTED | OTA_ASN1_SEQUENCE)) != 0){
      return OTA_ERR_RSA_VERIFY_FAILED;
  }

  if(asn1_len + 6 + hashlen != len) {
      return OTA_ERR_RSA_VERIFY_FAILED;
  }

  if((ret = ota_asn1_get_tag(&p, end, (unsigned int*)(&oid.len), OTA_ASN1_OID)) != 0){
      return OTA_ERR_RSA_VERIFY_FAILED;
  }

  oid.p = p;
  p += oid.len;
  if(ota_oid_get_md_alg( &oid, &msg_md_alg) != 0){
      return OTA_ERR_RSA_VERIFY_FAILED;
  }

  if(md_alg != msg_md_alg){
      return OTA_ERR_RSA_VERIFY_FAILED;
  }

  /*
   * assume the algorithm parameters must be NULL
   */
  if((ret = ota_asn1_get_tag(&p, end, (unsigned int*)(&asn1_len), OTA_ASN1_NULL)) != 0){
      return OTA_ERR_RSA_VERIFY_FAILED;
  }

  if((ret = ota_asn1_get_tag( &p, end, &asn1_len, OTA_ASN1_OCTET_STRING)) != 0){
      return OTA_ERR_RSA_VERIFY_FAILED;
  }

  if(asn1_len != hashlen){
      return OTA_ERR_RSA_VERIFY_FAILED;
  }

  if(memcmp(p, hash, hashlen) != 0 ){
      return OTA_ERR_RSA_VERIFY_FAILED;
  }

  p += hashlen;
  if(p != end){
      return OTA_ERR_RSA_VERIFY_FAILED;
  }
  return 0;
}

/*
* Do an RSA operation and check the message digest
*/
int ota_rsa_pkcs1_verify(ota_rsa_context *ctx,
                    int (*f_rng)(void *, unsigned char *, unsigned int),
                    void *p_rng,
                    int mode,
                    ota_md_type_t md_alg,
                    unsigned int hashlen,
                    const unsigned char *hash,
                    const unsigned char *sig)
{
  return ota_rsa_rsassa_pkcs1_v15_verify(ctx, f_rng, p_rng, mode, md_alg,
                                              hashlen, hash, sig);

}

void ota_rsa_free( ota_rsa_context *ctx )
{
  ota_mpi_free( &ctx->Vi ); ota_mpi_free( &ctx->Vf );
  ota_mpi_free( &ctx->RQ ); ota_mpi_free( &ctx->RP ); ota_mpi_free( &ctx->RN );
  ota_mpi_free( &ctx->QP ); ota_mpi_free( &ctx->DQ ); ota_mpi_free( &ctx->DP );
  ota_mpi_free( &ctx->Q  ); ota_mpi_free( &ctx->P  ); ota_mpi_free( &ctx->D );
  ota_mpi_free( &ctx->E  ); ota_mpi_free( &ctx->N  );
}


/* tee rsa_key -> mbed rsa_key */
static int ota_rsa_key_decode(
                             unsigned int type, void *in_key, void *out_key)
{
    int ret;
    ota_rsa_context *key;
    if (in_key == NULL || out_key == NULL) {
        return OTA_CRYPTO_INVALID_ARG;
    }

    key = (ota_rsa_context *)out_key;
    if (type == PK_PUBLIC) {
        ota_rsa_pubkey_t *rsa_key = (ota_rsa_pubkey_t *)in_key;

        ret = ota_mpi_read_binary(&key->N, rsa_key->n, rsa_key->n_size);
        if (0 != ret) {
            return OTA_CRYPTO_INVALID_KEY;
        }
        ret = ota_mpi_read_binary(&key->E, rsa_key->e, rsa_key->e_size);
        if (0 != ret) {
            return OTA_CRYPTO_INVALID_KEY;
        }
    } else {
        return OTA_CRYPTO_INVALID_TYPE;
    }

    return OTA_CRYPTO_SUCCESS;
}

static unsigned int _ota_get_hash_size(OTA_HASH_E type)
{
    unsigned int hash_size = 32;
    switch(type) {
        case SHA256:
            hash_size = 32;
            break; 
        case MD5:
            hash_size = 16;
            break;
        default:
            OTA_LOG_E("invalid %d", type);
    }

    return hash_size;
}

static ota_md_type_t _ota_get_hash_type(OTA_HASH_E type)
{
    ota_md_type_t hash_type = OTA_MD_SHA256;
    switch(type) {
        case SHA256:
            hash_type = OTA_MD_SHA256;
            break;
        case MD5:
            hash_type = OTA_MD_MD5;
            break;
        default:
            OTA_LOG_E("err type:%d", type);
    }
    return hash_type;
}

int ota_rsa_get_pubkey_size(unsigned int keybits, unsigned int *size)
{
    if (size == NULL) {
        return OTA_CRYPTO_INVALID_ARG;
    }

    if (keybits < TEE_MIN_RSA_KEY_SIZE || keybits > TEE_MAX_RSA_KEY_SIZE) {
        OTA_LOG_E("invalid keybits:%d", (int)keybits);
    }

    *size = sizeof(ota_rsa_pubkey_t);
    return OTA_CRYPTO_SUCCESS;
}

int ota_rsa_init_pubkey(unsigned int keybits,
                      const unsigned char *n, unsigned int n_size,
                      const unsigned char *e, unsigned int e_size,
                      ota_rsa_pubkey_t *pubkey)
{
    ota_rsa_pubkey_t *pub_key = NULL;
    if (pubkey == NULL ||
        n == NULL || n_size == 0 ||
        e == NULL || e_size == 0) {
        return OTA_CRYPTO_ERROR;
    }

    if ((n_size << 3) > keybits ||
        (e_size << 3) > keybits) {
        return OTA_CRYPTO_ERROR;
    }

    pub_key = (ota_rsa_pubkey_t *)pubkey;
    pub_key->n_size = n_size;
    pub_key->e_size = e_size;
    memcpy(pub_key->n, n, n_size);
    memcpy(pub_key->e, e, e_size);

    INIT_CTX_MAGIC(pub_key->magic);
    return OTA_CRYPTO_SUCCESS;
}

int ota_rsa_verify(const ota_rsa_pubkey_t *pub_key,
                      const uint8_t *dig, size_t dig_size,
                      const uint8_t *sig, size_t sig_size,
                      ota_rsa_padding_t padding, bool *p_result)
{
    int ret;
    unsigned int hash_size;
    unsigned int key_size;
    OTA_HASH_E ali_hash_type;
    ota_md_type_t hash_type = OTA_MD_SHA256;
    ota_rsa_context ctx;
    int result = OTA_CRYPTO_SUCCESS;

    if (pub_key == NULL || NULL == p_result ||
        dig == NULL || dig_size == 0 ||
        sig == NULL || sig_size == 0) {
        if(p_result != NULL)
            *p_result = 0;
        return OTA_CRYPTO_ERROR;
    }

    if (!IS_VALID_CTX_MAGIC(((ota_rsa_pubkey_t *)pub_key)->magic)) {
        *p_result = 0;
        return OTA_CRYPTO_ERROR;
    }

    if (padding.type == RSASSA_PKCS1_V1_5) {
        ali_hash_type = padding.pad.rsassa_v1_5.type;
    }
    else {
        *p_result = 0;
        return OTA_CRYPTO_ERROR;
    }
    hash_size = _ota_get_hash_size(ali_hash_type);
    if (dig_size != hash_size) {
        *p_result = 0;
        return OTA_CRYPTO_ERROR;
    }
    hash_type = _ota_get_hash_type(ali_hash_type);
    if (0 == hash_type) {
        *p_result = 0;
        return OTA_CRYPTO_ERROR;
    }
    ota_rsa_init(&ctx, OTA_RSA_PKCS_V15, hash_type);
    result = ota_rsa_key_decode(PK_PUBLIC, (void *)pub_key, &ctx);
    if (OTA_CRYPTO_SUCCESS != result) {
        OTA_GO_RET(OTA_CRYPTO_INVALID_KEY, "decode err:%08x", result);
    }
    ret = ota_rsa_check_pubkey(&ctx);
    if (0 != ret) {
        OTA_GO_RET(OTA_CRYPTO_INVALID_KEY, "key invalid:%08x", ret);
    }
    key_size = ota_mpi_size(&ctx.N);
    if (sig_size != key_size) {
        OTA_GO_RET(OTA_CRYPTO_LENGTH_ERR, "invalid size:%d", (int)sig_size);
    }
    ctx.len = key_size;
    if (dig_size + 11 > key_size) {
        OTA_GO_RET(OTA_CRYPTO_LENGTH_ERR, "invalid dig_size:%d", (int)dig_size);
    }
    ret = ota_rsa_pkcs1_verify(&ctx, NULL, NULL,
                          OTA_RSA_PUBLIC, (ota_md_type_t)hash_type,
                          (unsigned int )0,
                          (const unsigned char *)dig,
                          (const unsigned char *)sig);
    if(0 != ret) {
        OTA_GO_RET(OTA_CRYPTO_ERROR, "pkcs1 err:%d", ret);
    }

_OUT:
    if (0 == result) {
        *p_result = 1;
    } else {
        *p_result = 0;
    }
    ota_rsa_free(&ctx);
    return result;
}
