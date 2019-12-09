#ifndef HTTPDNS_H
#define HTTPDNS_H

#include <network/network.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SYNC (0)
#define ASYNC (1)

/**
 *
 * type of httpdns result cache
 *
 */
struct dns_cache;
typedef struct dns_cache dns_cache_t;

/******************* httpdns dns operation **********************/

/**
 *
 * @brief check httpdns is disabled
 *
 * @return 0 available
 * @return 1 disabled
 */
int httpdns_is_disabled(void);

/**
 *
 * @brief Invoke host resolving action
 * @param[in] host_name host domain
 * @param[in] asyn 1: asynchronized 0: synchronized
 *
 */
void httpdns_resolv_host(char *host_name, int asyn);

/**
 *
 * @brief Fetch DNS result from cache
 * @param[in]  host_name host domain
 * @param[out] cache DNS result
 *
 * @return 1  pretch success
 * @return 0  pretch failed
 */
int httpdns_prefetch(char * host_name, dns_cache_t ** cache);

/**
 *
 * @brief Fetch DNS result from cache with timeout
 * @param[in]  host_name host domain
 * @param[out] cache DNS result
 * @param[in]  timeout fetch timeout in millisecond
 *
 * @return 1  pretch success
 * @return 0  pretch failed
 */
int httpdns_prefetch_timeout(char * host_name, dns_cache_t ** cache, int ms);


/***** httpdns dns operation *****/

/**
 *
 * @brief Get the first ip address from cache
 * @param[in]  cache DNS result
 * @param[out] ipaddr ip address
 *
 * @return 1  success
 * @return 0  failed
 */
int httpdns_get_ipaddr_from_cache(dns_cache_t * cache, char * ipaddr);

/**
 *
 * @brief Free cache
 * @param[in] cache DNS result
 *
 */
void httpdns_free_cache(dns_cache_t * cache);

/**
 *
 * @brief Rotate ip list in cache
 * @param[in] cache DNS result
 *
 * @return 1  success
 * @return 0  failed
 */
int httpdns_rotate_cache(dns_cache_t * dns);

#ifdef __cplusplus
}
#endif

#endif

