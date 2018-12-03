/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/
#include "config.h"
#include "osa.h"



#if defined(ALI_ALGO_ASN1_PARSE_C)
#include "asn1.h"

#if defined(ALI_ALGO_BIGNUM_C)
#include "bignum.h"
#endif


/* Implementation that should never be optimized out by the compiler */
static void ali_algo_zeroize(void *v, size_t n)
{
    volatile unsigned char *p = (unsigned char *)v;
    while (n--)
        *p++ = 0;
}

/*
 * ASN.1 DER decoding routines
 */
int ali_algo_asn1_get_len(unsigned char **p, const unsigned char *end,
                         size_t *len)
{
    if ((end - *p) < 1)
        return (ALI_ALGO_ERR_ASN1_OUT_OF_DATA);

    if ((**p & 0x80) == 0)
        *len = *(*p)++;
    else {
        switch (**p & 0x7F) {
            case 1:
                if ((end - *p) < 2)
                    return (ALI_ALGO_ERR_ASN1_OUT_OF_DATA);

                *len = (*p)[1];
                (*p) += 2;
                break;

            case 2:
                if ((end - *p) < 3)
                    return (ALI_ALGO_ERR_ASN1_OUT_OF_DATA);

                *len = ((size_t)(*p)[1] << 8) | (*p)[2];
                (*p) += 3;
                break;

            case 3:
                if ((end - *p) < 4)
                    return (ALI_ALGO_ERR_ASN1_OUT_OF_DATA);

                *len =
                  ((size_t)(*p)[1] << 16) | ((size_t)(*p)[2] << 8) | (*p)[3];
                (*p) += 4;
                break;

            case 4:
                if ((end - *p) < 5)
                    return (ALI_ALGO_ERR_ASN1_OUT_OF_DATA);

                *len = ((size_t)(*p)[1] << 24) | ((size_t)(*p)[2] << 16) |
                       ((size_t)(*p)[3] << 8) | (*p)[4];
                (*p) += 5;
                break;

            default:
                return (ALI_ALGO_ERR_ASN1_INVALID_LENGTH);
        }
    }

    if (*len > (size_t)(end - *p))
        return (ALI_ALGO_ERR_ASN1_OUT_OF_DATA);

    return (0);
}

int ali_algo_asn1_get_tag(unsigned char **p, const unsigned char *end,
                         size_t *len, int tag)
{
    if ((end - *p) < 1)
        return (ALI_ALGO_ERR_ASN1_OUT_OF_DATA);

    if (**p != tag)
        return (ALI_ALGO_ERR_ASN1_UNEXPECTED_TAG);

    (*p)++;

    return (ali_algo_asn1_get_len(p, end, len));
}

int ali_algo_asn1_get_bool(unsigned char **p, const unsigned char *end, int *val)
{
    int    ret;
    size_t len;

    if ((ret = ali_algo_asn1_get_tag(p, end, &len, ALI_ALGO_ASN1_BOOLEAN)) != 0)
        return (ret);

    if (len != 1)
        return (ALI_ALGO_ERR_ASN1_INVALID_LENGTH);

    *val = (**p != 0) ? 1 : 0;
    (*p)++;

    return (0);
}

int ali_algo_asn1_get_int(unsigned char **p, const unsigned char *end, int *val)
{
    int    ret;
    size_t len;

    if ((ret = ali_algo_asn1_get_tag(p, end, &len, ALI_ALGO_ASN1_INTEGER)) != 0)
        return (ret);

    if (len == 0 || len > sizeof(int) || (**p & 0x80) != 0)
        return (ALI_ALGO_ERR_ASN1_INVALID_LENGTH);

    *val = 0;

    while (len-- > 0) {
        *val = (*val << 8) | **p;
        (*p)++;
    }

    return (0);
}

#if defined(ALI_ALGO_BIGNUM_C)
int ali_algo_asn1_get_mpi(unsigned char **p, const unsigned char *end,
                         ali_algo_mpi *X)
{
    int    ret;
    size_t len;

    if ((ret = ali_algo_asn1_get_tag(p, end, &len, ALI_ALGO_ASN1_INTEGER)) != 0)
        return (ret);

    ret = ali_algo_mpi_read_binary(X, *p, len);

    *p += len;

    return (ret);
}
#endif /* ALI_ALGO_BIGNUM_C */

int ali_algo_asn1_get_bitstring(unsigned char **p, const unsigned char *end,
                               ali_algo_asn1_bitstring *bs)
{
    int ret;

    /* Certificate type is a single byte bitstring */
    if ((ret = ali_algo_asn1_get_tag(p, end, &bs->len,
                                    ALI_ALGO_ASN1_BIT_STRING)) != 0)
        return (ret);

    /* Check length, subtract one for actual bit string length */
    if (bs->len < 1)
        return (ALI_ALGO_ERR_ASN1_OUT_OF_DATA);
    bs->len -= 1;

    /* Get number of unused bits, ensure unused bits <= 7 */
    bs->unused_bits = **p;
    if (bs->unused_bits > 7)
        return (ALI_ALGO_ERR_ASN1_INVALID_LENGTH);
    (*p)++;

    /* Get actual bitstring */
    bs->p = *p;
    *p += bs->len;

    if (*p != end)
        return (ALI_ALGO_ERR_ASN1_LENGTH_MISMATCH);

    return (0);
}

/*
 * Get a bit string without unused bits
 */
int ali_algo_asn1_get_bitstring_null(unsigned char **p, const unsigned char *end,
                                    size_t *len)
{
    int ret;

    if ((ret = ali_algo_asn1_get_tag(p, end, len, ALI_ALGO_ASN1_BIT_STRING)) != 0)
        return (ret);

    if ((*len)-- < 2 || *(*p)++ != 0)
        return (ALI_ALGO_ERR_ASN1_INVALID_DATA);

    return (0);
}


/*
 *  Parses and splits an ASN.1 "SEQUENCE OF <tag>"
 */
int ali_algo_asn1_get_sequence_of(unsigned char **p, const unsigned char *end,
                                 ali_algo_asn1_sequence *cur, int tag)
{
    int               ret;
    size_t            len;
    ali_algo_asn1_buf *buf;

    /* Get main sequence tag */
    if ((ret = ali_algo_asn1_get_tag(
           p, end, &len, ALI_ALGO_ASN1_CONSTRUCTED | ALI_ALGO_ASN1_SEQUENCE)) !=
        0)
        return (ret);

    if (*p + len != end)
        return (ALI_ALGO_ERR_ASN1_LENGTH_MISMATCH);

    while (*p < end) {
        buf      = &(cur->buf);
        buf->tag = **p;

        if ((ret = ali_algo_asn1_get_tag(p, end, &buf->len, tag)) != 0)
            return (ret);

        buf->p = *p;
        *p += buf->len;

        /* Allocate and assign next pointer */
        if (*p < end) {
            cur->next = (ali_algo_asn1_sequence *)osa_calloc(
              1, sizeof(ali_algo_asn1_sequence));

            if (cur->next == NULL)
                return (ALI_ALGO_ERR_ASN1_ALLOC_FAILED);

            cur = cur->next;
        }
    }

    /* Set final sequence entry's next pointer to NULL */
    cur->next = NULL;

    if (*p != end)
        return (ALI_ALGO_ERR_ASN1_LENGTH_MISMATCH);

    return (0);
}

int ali_algo_asn1_get_alg(unsigned char **p, const unsigned char *end,
                         ali_algo_asn1_buf *alg, ali_algo_asn1_buf *params)
{
    int    ret;
    size_t len;

    if ((ret = ali_algo_asn1_get_tag(
           p, end, &len, ALI_ALGO_ASN1_CONSTRUCTED | ALI_ALGO_ASN1_SEQUENCE)) !=
        0)
        return (ret);

    if ((end - *p) < 1)
        return (ALI_ALGO_ERR_ASN1_OUT_OF_DATA);

    alg->tag = **p;
    end      = *p + len;

    if ((ret = ali_algo_asn1_get_tag(p, end, &alg->len, ALI_ALGO_ASN1_OID)) != 0)
        return (ret);

    alg->p = *p;
    *p += alg->len;

    if (*p == end) {
        ali_algo_zeroize(params, sizeof(ali_algo_asn1_buf));
        return (0);
    }

    params->tag = **p;
    (*p)++;

    if ((ret = ali_algo_asn1_get_len(p, end, &params->len)) != 0)
        return (ret);

    params->p = *p;
    *p += params->len;

    if (*p != end)
        return (ALI_ALGO_ERR_ASN1_LENGTH_MISMATCH);

    return (0);
}

int ali_algo_asn1_get_alg_null(unsigned char **p, const unsigned char *end,
                              ali_algo_asn1_buf *alg)
{
    int              ret;
    ali_algo_asn1_buf params;

    osa_memset(&params, 0, sizeof(ali_algo_asn1_buf));

    if ((ret = ali_algo_asn1_get_alg(p, end, alg, &params)) != 0)
        return (ret);

    if ((params.tag != ALI_ALGO_ASN1_NULL && params.tag != 0) || params.len != 0)
        return (ALI_ALGO_ERR_ASN1_INVALID_DATA);

    return (0);
}

void ali_algo_asn1_free_named_data(ali_algo_asn1_named_data *cur)
{
    if (cur == NULL)
        return;

    osa_free(cur->oid.p);
    osa_free(cur->val.p);

    ali_algo_zeroize(cur, sizeof(ali_algo_asn1_named_data));
}

void ali_algo_asn1_free_named_data_list(ali_algo_asn1_named_data **head)
{
    ali_algo_asn1_named_data *cur;

    while ((cur = *head) != NULL) {
        *head = cur->next;
        ali_algo_asn1_free_named_data(cur);
        osa_free(cur);
    }
}

ali_algo_asn1_named_data *ali_algo_asn1_find_named_data(
  ali_algo_asn1_named_data *list, const char *oid, size_t len)
{
    while (list != NULL) {
        if (list->oid.len == len && osa_memcmp(list->oid.p, oid, len) == 0) {
            break;
        }

        list = list->next;
    }

    return (list);
}

#endif /* ALI_ALGO_ASN1_PARSE_C */
