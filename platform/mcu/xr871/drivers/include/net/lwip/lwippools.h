/**
 * Define pools used by mbuf.
 *
 * LWIP_MEMPOOL(pool_name, number_elements, element_size, pool_description)
 *     creates a pool name MEMP_pool_name. description is used in stats.c
 */

#define SIZEOF_MBUF    48    /* MUST be equal to MSIZE defined in mbuf.h */
#define MEMP_NUM_MBUF   64

//LWIP_MEMPOOL(MBUF,      MEMP_NUM_MBUF,    sizeof(struct mbuf),      "MBUF")
LWIP_MEMPOOL(MBUF,      MEMP_NUM_MBUF,    SIZEOF_MBUF,      "MBUF")
