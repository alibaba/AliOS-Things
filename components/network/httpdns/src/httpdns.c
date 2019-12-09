#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/select.h>

#include "sds.h"
#include "dict.h"
#include "iplist.h"
#include "http_ctx.h"
#include "httpdns/httpdns.h"

#include "ulog/ulog.h"
#include "cJSON.h"

#include "httpdns_opts.h"
#include <posix/pthread.h>
#include <posix/timer.h>

#define HTTPDNS_TIMEOUT 10
#define HTTPDNS_CONNECT_TIMEOUT 5
#define MAX_DNSCACHE_AGE (15*60)

#define TAG "httpdns"
#define HTTPDNS_ERR(fmt,arg...)   LOGE(TAG, fmt,##arg)
#define HTTPDNS_INFO(fmt,arg...)  LOGI(TAG, fmt,##arg)
#define HTTPDNS_WARN(fmt,arg...)  LOGW(TAG, fmt,##arg)

static dict * resolv = NULL;

static pthread_mutex_t resolv_lock;

struct  dns_cache {
    struct list_head host_list;
    char * host_name;
    struct timespec ts;
};

typedef struct httpdns_connection {
    http_ctx_t ctx;
    dns_cache_t * dns;
} httpdns_connection_t;

static inline void httpdns_resolv_lock(void)
{
    pthread_mutex_lock(&resolv_lock);
}

static inline void httpdns_resolv_unlock(void)
{
    pthread_mutex_unlock(&resolv_lock);
}

static inline void httpnds_resolv_lock_init(void)
{
    pthread_mutex_init(&resolv_lock, NULL);
}

static double timespec_to_seconds(struct timespec* ts)
{
    return (double)ts->tv_sec + (double)ts->tv_nsec / 1000000000.0;
}

static void httpdns_age_out(void)
{
    struct timespec end;
    double elapsedSeconds = 0;

    clock_gettime(CLOCK_MONOTONIC, &end);

    if(NULL != resolv)  {
        dictIterator * di = dictGetSafeIterator(resolv);
        if(NULL != di) {
            dictEntry * de = NULL;
            while((de = dictNext(di)) != NULL) {
                dns_cache_t * cache = (dns_cache_t *)de->val;
                elapsedSeconds = timespec_to_seconds(&end) - timespec_to_seconds(&(cache->ts));
                if(elapsedSeconds >= MAX_DNSCACHE_AGE) {
                    HTTPDNS_INFO("<HTTPDNS> %s : age out,  free %s\n", __func__, cache->host_name);
                    dictDelete(resolv, cache->host_name);
                    httpdns_free_cache(cache);
                }
            }
            dictReleaseIterator(di);
        }
    }
}

int httpdns_rotate_cache(dns_cache_t * dns)
{
    if(NULL == dns)
        return 0;

    return iplist_rotate(&(dns->host_list));
}

int httpdns_update_cache(dns_cache_t * dns)
{
    int sz = -1;
    struct timespec ts;
    if(NULL == dns) {
        return -1;
    }
    sz = iplist_size(&(dns->host_list));

    clock_gettime(CLOCK_MONOTONIC, &ts);
    if(sz > 0) {
            httpdns_resolv_lock();
            if(NULL != resolv) {
                dns_cache_t *cache = (dns_cache_t *) dictFetchValue(resolv, dns->host_name);
                if(NULL != cache) {
                     dictDelete(resolv, dns->host_name);
                     httpdns_free_cache(cache);
                }
                if(dictAdd(resolv, dns->host_name, dns) != DICT_OK) {
                    HTTPDNS_INFO("<HTTPDNS> %s : failed to add cache for %s\n", __func__, dns->host_name);
                    httpdns_free_cache(dns);
                } else {
                    HTTPDNS_INFO("<HTTPDNS> %s : success to add cache for %s\n", __func__, dns->host_name);
                    memcpy(&dns->ts, &ts, sizeof(struct timespec));
                    httpdns_resolv_unlock();
                    return 0;
                }
            }
            httpdns_resolv_unlock();
    } else {
        httpdns_free_cache(dns);
    }
    return 0;
}

int httpdns_replace_cache(dns_cache_t * dns)
{
    int sz = -1;
    if(NULL == dns) {
        return -1;
    }
    sz = iplist_size(&(dns->host_list));
    if(sz > 0) {
        httpdns_resolv_lock();
        if(NULL != resolv) {
            dns_cache_t *cache = (dns_cache_t *) dictFetchValue(resolv, dns->host_name);
            if(NULL != cache) {
                 dictDelete(resolv, dns->host_name);
                 memcpy(&dns->ts, &cache->ts, sizeof(struct timespec));
                 httpdns_free_cache(cache);
            }
            if(dictAdd(resolv, dns->host_name, dns) != DICT_OK) {
                HTTPDNS_INFO("<HTTPDNS> %s : failed to replace cache for %s\n", __func__, dns->host_name);
                httpdns_free_cache(dns);
            } else {
                HTTPDNS_INFO("<HTTPDNS> %s : success to replace cache for %s\n", __func__, dns->host_name);
                httpdns_resolv_unlock();
                return 0;
            }
        }
        httpdns_resolv_unlock();
    } else {
        httpdns_free_cache(dns);
    }
    return 0;
}

static char * httpdns_select_server(void)
{
    int sz, index;
    srand(time(NULL));
    sz = sizeof(server_list)/sizeof(server_list[0]);
    index =  rand() % sz;
    return (char *)server_list[index];
}

void httpdns_get_ip_from_addrinfo(struct addrinfo* res, char * ip)
{
    struct addrinfo* ai = res;

    char str[INET_ADDRSTRLEN] = {0};

    if(ai->ai_family == AF_INET) {
        struct sockaddr_in *myaddr = (struct sockaddr_in *) ai->ai_addr;
        memset(str, 0, sizeof(str));
        inet_ntop(AF_INET, &(myaddr->sin_addr), str,  INET_ADDRSTRLEN);

    } else if(ai->ai_family == AF_INET6) {
#ifndef WITH_LWIP
        struct sockaddr_in6 *myaddr = (struct sockaddr_in6 *) ai->ai_addr;
        memset(str, 0, sizeof(str));
        inet_ntop(AF_INET6, &(myaddr->sin6_addr), str,  INET_ADDRSTRLEN);
#endif
    }
    memcpy(ip, str, INET_ADDRSTRLEN);
}

void httpdns_print_addrinfo(struct addrinfo* res)
{
    struct addrinfo* ai = res;

    char str[INET_ADDRSTRLEN];
    sds ip = sdsempty();
    while(ai != NULL) {
        if(ai->ai_family == AF_INET) {
            struct sockaddr_in *myaddr = (struct sockaddr_in *) ai->ai_addr;
            memset(str, 0, sizeof(str));
            inet_ntop(AF_INET, &(myaddr->sin_addr), str,  INET_ADDRSTRLEN);
            ip = sdscat(ip, str);

        } else if(ai->ai_family == AF_INET6) {
#ifndef WITH_LWIP
            struct sockaddr_in6 *myaddr = (struct sockaddr_in6 *) ai->ai_addr;
            memset(str, 0, sizeof(str));
            inet_ntop(AF_INET6, &(myaddr->sin6_addr), str,  INET_ADDRSTRLEN);
            ip = sdscat(ip, str);
#endif
        }
        ai = ai->ai_next;
        if(NULL != ai) {
            ip = sdscat(ip, (char*)", ");
        }
    }
    HTTPDNS_INFO("<HTTPDNS> %s : addr @ %p = %s\n", __func__, res, ip);
    sdsfree(ip);
}

struct addrinfo * httpdns_merge_addrinfo(struct addrinfo *ai1, struct addrinfo *ai2)
{
    struct addrinfo *p, *q, *s, *t;

    HTTPDNS_INFO("<HTTPDNS> %s : merge %p with %p\n", __func__, ai1, ai2);

    if(NULL == ai1) {
        if(ai2 != NULL) {
            return ai2;
        } else {
            return NULL;
        }
    } else  {

        if(NULL == ai2) {
            return ai1;
        } else {
            p = ai1;
            q = ai2;
            while(p && q) {
                s = p->ai_next;
                t = q->ai_next;
                if(s && t) {
                    p->ai_next = q;
                    q->ai_next = s;
                    p = s;
                    q = t;
                } else if(s && !t) {
                    p->ai_next = q;
                    q->ai_next = s;
                    p = NULL;
                    q= NULL;
                } else {
                    p->ai_next = q;
                    p = NULL;
                    q= NULL;
                }
            }
            httpdns_print_addrinfo(ai1);
            return ai1;
        }
    }
    return NULL;
}

struct addrinfo * httpdns_unique_addrinfo(struct addrinfo *ai)
{
    struct addrinfo * p, *q, *r;
    struct sockaddr_in * myaddr1, * myaddr2;
    if(NULL == ai) {
        return NULL;
    }
    HTTPDNS_INFO("<HTTPDNS> %s : unique %p\n", __func__, ai);
    p = ai;
    while(p) {
        q = p;
        myaddr1 = (struct sockaddr_in *) p->ai_addr;
        while(q->ai_next) {
            myaddr2 = (struct sockaddr_in *) q->ai_next->ai_addr;
            if(memcmp(myaddr1, myaddr2, sizeof(struct sockaddr_in)) == 0) {
                r = q->ai_next;
                q->ai_next = r->ai_next;
                r->ai_next = NULL;
                freeaddrinfo(r);
            } else {
                q = q->ai_next;
            }
        }
        p = p->ai_next;
    }
    httpdns_print_addrinfo(ai);
    return ai;
}

struct addrinfo * httpdns_crop_addrinfo(struct addrinfo *ai, int size)
{
    /* crop size */
    int  i = 0;
    struct addrinfo *t = NULL;
    struct addrinfo *s =  ai;
    HTTPDNS_INFO("<HTTPDNS> %s : crop %p, size = %d\n", __func__, ai, size);

    if(ai == NULL && size <= 0) {
        return NULL;
    }

    while(s != NULL) {
        i++;
        s = s->ai_next;

        if(i >= size) {
            t = s->ai_next;
            s->ai_next = NULL;
        }
    }
    if(NULL != t) {
       freeaddrinfo(t);
    }
    //httpdns_print_addrinfo(ai);
    return ai;
}

static dns_cache_t * httpdns_build_default_cache(char *host_name)
{
    dns_cache_t * cache = (dns_cache_t *) malloc(sizeof(dns_cache_t));
    if(NULL == cache ) return NULL;
    memset(cache, 0, sizeof(dns_cache_t));
    cache->host_name = sdsnew(host_name);
    iplist_init(&(cache->host_list));
    iplist_add(&(cache->host_list), "0.0.0.0");
    return cache;
}

dns_cache_t * httpdns_build_cache(struct addrinfo * res, char * host_name)
{
    struct addrinfo* ai;
    dns_cache_t * cache;
    if(NULL == res || NULL == host_name) {
        return NULL;
    }
    ai = res;
    cache = (dns_cache_t *) malloc(sizeof(dns_cache_t));
    if(NULL == cache) {
        return NULL;
    }
    cache->host_name=sdsnew(host_name);
    iplist_init(&(cache->host_list));

    while(ai != NULL) {
        char str[INET_ADDRSTRLEN];
        struct sockaddr_in *myaddr = (struct sockaddr_in *) ai->ai_addr;
        inet_ntop(AF_INET, &(myaddr->sin_addr.s_addr), str,  INET_ADDRSTRLEN);
        iplist_add(&(cache->host_list), str);
        ai = ai->ai_next;
    }
    return cache;
}

struct addrinfo* httpdns_build_addrinfo(dns_cache_t *cache, int port)
{
    ipv4_list_t *s;
    struct addrinfo * head = NULL;
    struct sockaddr_in * myaddr = NULL;
    struct addrinfo *res = NULL;

    HTTPDNS_INFO("<HTTPDNS> %s : build addrinfo from cache  %p, port = %d\n", __func__, cache, port);

    if((NULL == cache) || (iplist_size(&(cache->host_list)) == 0)) {
        return NULL;
    }

    list_for_each_entry(s, &(cache->host_list), list) {

        if(iplist_size(&(cache->host_list)) == 1) {
            if(strcmp(s->ipv4_addr, "0.0.0.0") == 0)  {
                HTTPDNS_INFO("<HTTPDNS> %s : addr = %s\n", __func__, s->ipv4_addr);
                return NULL;
            }
        }
        res = (struct addrinfo *) malloc(sizeof(struct addrinfo) + sizeof(struct sockaddr));
        if(NULL == res) {
            continue;
        }
        res->ai_family = AF_INET;
        res->ai_socktype = SOCK_STREAM;
        res->ai_protocol = IPPROTO_TCP;
        res->ai_addrlen = sizeof(struct sockaddr);
        res->ai_addr = (struct sockaddr  *) ((char *)res + sizeof(struct addrinfo));

        myaddr = (struct sockaddr_in *) res->ai_addr;
        memset(myaddr, 0, sizeof(struct sockaddr_in));

        myaddr->sin_family = AF_INET;
        if(port == 80) {
            myaddr->sin_port = 0x5000;
        } else if(port == 443) {
            myaddr->sin_port = 0xbb01;
        } else {
            myaddr->sin_port = 0;
        }
        inet_pton(AF_INET, s->ipv4_addr, &(myaddr->sin_addr.s_addr));

        res->ai_canonname = NULL;
        res->ai_next = NULL;

        if(NULL == head) {
            head = res;
        } else {
            struct addrinfo * tmp = head;
            while(tmp->ai_next != NULL) {
                tmp = tmp->ai_next;
            }
            tmp->ai_next = res;
        }
    }

    if(NULL != head) {
        httpdns_print_addrinfo(head);
        return head;
    }

    return NULL;
}

int httpdns_prefetch(char * host_name, dns_cache_t ** cache)
{
    if((NULL == cache) || (NULL == host_name)) {
        return 0;
    }
    httpdns_resolv_lock();

    httpdns_age_out();

    if(NULL != resolv) {
        dns_cache_t *dns_cache = (dns_cache_t *) dictFetchValue(resolv, host_name);
        if(NULL != dns_cache) {
            *cache = (dns_cache_t *) malloc(sizeof(dns_cache_t));
            if(NULL == *cache) {
                HTTPDNS_INFO("<HTTPDNS> %s : malloc cache failed\n", __func__);
                httpdns_resolv_unlock();
                return 0;
            }
            (*cache)->host_name = sdsdup(dns_cache->host_name);
            iplist_dup(&((*cache)->host_list),  &(dns_cache->host_list));
        }
        httpdns_resolv_unlock();
        return 1;
    }

    httpdns_resolv_unlock();
    return 0;
}

int httpdns_get_ipaddr_from_cache(dns_cache_t * cache, char * ipaddr)
{
    int sz = 0, index = 0;
    char zeroipaddr[16] = {0};
    ipv4_list_t *s = NULL;

    memset(zeroipaddr, 0, sizeof(zeroipaddr));
    snprintf(zeroipaddr, sizeof(zeroipaddr), "%s", "0.0.0.0");

    if(cache == NULL) return 0;

    sz = iplist_size(&(cache->host_list));
    if(sz <= 0) {
        return 0;
    }

    srand(time(NULL));
    index = rand() % sz;

    s = iplist_get(&(cache->host_list), index);
    memcpy(ipaddr, s->ipv4_addr, 16);
    if(strncmp(ipaddr, zeroipaddr, 16) != 0) {
        return 1;
    }
    return 0;
}

int httpdns_prefetch_timeout(char * host_name, dns_cache_t ** cache, int ms)
{
    int result = 0;
    int sleep_ms = 50;
    double timeout = 0;
    struct timespec start = { 0, 0 }, end = { 0, 0 };

    if((NULL == cache) || (NULL == host_name) || (ms < 0)) {
        return 0;
    }

    int max_retry = ms / sleep_ms + 1;

    HTTPDNS_INFO("<HTTPDNS> %s : prefetch cache, max retry = %d\n", __func__, max_retry);

    if(clock_gettime(CLOCK_MONOTONIC, &start) < 0 ) {
       HTTPDNS_INFO("<HTTPDNS> %s : get start time failed %s\n", __func__, strerror(errno));
    }

    do {
        result = httpdns_prefetch(host_name, cache);
        max_retry --;
        if(result == 1) {
            char ipaddr[16] = {0};
            if(httpdns_get_ipaddr_from_cache(*cache, ipaddr)) {
                break;
            } else {
                httpdns_free_cache(*cache);
                *cache = NULL;
                result = 0;
            }
        }
        usleep(sleep_ms*1000);
    } while(max_retry > 0);

    if(clock_gettime(CLOCK_MONOTONIC, &end) < 0 ) {
        HTTPDNS_INFO("<HTTPDNS> %s : get end time failed %s\n", __func__, strerror(errno));
    }

    timeout = timespec_to_seconds(&end) - timespec_to_seconds(&start);
    HTTPDNS_INFO("<HTTPDNS> %s : prefetch end,  time = %lf s, max_retry = %d, result  = %d\n", __func__, timeout, max_retry, result);

   return result;
}

int httpdns_is_valid_ip(char * host_name)
{
    char * str = host_name;
    if (str == NULL || *str == '\0')
        return 0;

    struct sockaddr_in addr4;
#ifndef WITH_LWIP
    struct sockaddr_in6 addr6;
#endif

    if (1 == inet_pton(AF_INET, str, &addr4.sin_addr))
        return 1;
#ifndef WITH_LWIP
    else if (1 == inet_pton(AF_INET6, str, &addr6.sin6_addr))
        return 1;
#endif

    return 0;
}

int httpdns_is_disabled(void)
{
    return (HTTP_DNS_DISABLED == 1);
}

/*
 *  http://203.107.1.33/100000/d?host=www.aliyun.com
*/
static int httpdns_init(httpdns_connection_t *conn, char *server_ip, char * user_id,
                 char * host_name, int connect_timeout, int timeout)
{
    sds url = NULL;
    dns_cache_t * cache = NULL;
    httpnds_resolv_lock_init();

    httpdns_resolv_lock();
    if(NULL == resolv)  {
        resolv = dictCreate(&dictTypeHeapStrings, NULL);
        if(NULL == resolv) {
            free(conn);
            httpdns_resolv_unlock();
            HTTPDNS_INFO("<HTTPDNS> %s : Failed to alloc resolv cache\n", __func__);
            return -1;
        }
    }
    httpdns_resolv_unlock();

    if(NULL == server_ip) {
         server_ip = httpdns_select_server();
    }
    url = sdsnew((char *)"http://");
    url = sdscat(url, (char *)server_ip);
    url = sdscat(url, (char *)"/");
    url = sdscat(url, user_id);
    url = sdscat(url, (char *)"/resolve?host=");
    url = sdscat(url, host_name);

    http_ctx_init(&(conn->ctx), url, connect_timeout, timeout);
    conn->dns = (dns_cache_t *) malloc(sizeof(dns_cache_t));
    if(NULL == conn->dns) {
        sdsfree(url);
        free(conn);
        HTTPDNS_INFO("<HTTPDNS> %s : Failed to alloc dns cache\n", __func__);
        return -1;
    }
    conn->dns->host_name = sdsnew(host_name);
    iplist_init(&(conn->dns->host_list));

    cache = httpdns_build_default_cache(host_name);
    if(NULL != cache) {
        httpdns_update_cache(cache);
    }
    return 0;
}

/*
 * { "dns": [{"host":"www.aliyun.com", "client_ip": "106.11.31.222", "ips":["140.205.32.8"],"ttl":38,"origin_ttl":120}] }
 */
static void httpdns_parse_dns(httpdns_connection_t *conn, cJSON* j)
{
    struct list_head *ips = NULL;
    cJSON* obj = NULL;

    if((NULL == conn) || (NULL == j)) {
        return;
    }
    ips = &(conn->dns->host_list);
    obj = cJSON_GetObjectItem(j, "host");
    if(NULL != obj) {
        const char * str = obj->valuestring;
        if((NULL == str) || (strcmp(conn->dns->host_name, str) != 0)) {
            HTTPDNS_INFO("<HTTPDNS> %s : Host Does Not Match\n", __func__);
            iplist_add(ips, "0.0.0.0");
            return;
        }
    }

    obj = cJSON_GetObjectItem(j, "client_ip");
    if(obj != NULL) {
        const char * str = obj->valuestring;
        if(NULL != str) {
            HTTPDNS_INFO("<HTTPDNS> %s: client ip =%s\n", __func__, str);
        }
    }

    obj = cJSON_GetObjectItem(j, "ips");
    if(obj != NULL) {
            int l = cJSON_GetArraySize(obj);
            if(0 == l) {
                iplist_add(ips, "0.0.0.0");
            } else {
                int i = 0 ;
                cJSON* v = NULL;
                for( ; i < l; i++) {
                    const char * str = NULL;
                    v = cJSON_GetArrayItem(obj, i);
                    str = v->valuestring;
                    if(httpdns_is_valid_ip(str)) {
                        iplist_add(ips, str);
                    }
                }
             }
        }
}

static void httpdns_parse_json(httpdns_connection_t *conn, cJSON* j)
{
    struct list_head *ips = NULL;
    cJSON* obj = NULL;

    if((NULL == conn) || (NULL == j)) {
        return;
    }

    ips = &(conn->dns->host_list);

    obj =  cJSON_GetObjectItem(j, "dns");
    if(NULL != obj) {
        int l = cJSON_GetArraySize(obj);
        if(1 == l) {
            cJSON* v = cJSON_GetArrayItem(obj, 0);
            httpdns_parse_dns(conn, v);
        } else {
            iplist_add(ips, "0.0.0.0");
        }
    }
}

void httpdns_free_cache(dns_cache_t * cache)
{
    if(NULL == cache) return;
    iplist_free(&(cache->host_list));
    sdsfree(cache->host_name);
    free(cache);
}

static int httpdns_process(httpdns_connection_t *conn)
{
    double elapsedSeconds;
    memory_t *mem = NULL;
    struct timespec start, end;

    if(NULL == conn) {
        return -1;
    }
    clock_gettime(CLOCK_MONOTONIC, &start);
    mem = http_ctx_get(&(conn->ctx));
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsedSeconds = timespec_to_seconds(&end) - timespec_to_seconds(&start);

    HTTPDNS_INFO("<HTTPDNS> %s : elapse time = %lf seconds\n", __func__, elapsedSeconds);
    if(NULL != mem) {
        HTTPDNS_INFO("<HTTPDNS> %s : memory = %s\n", __func__,  mem->memory);

        printf("input==> %s\n", mem->memory);
        cJSON *j = cJSON_Parse(mem->memory);
        if(NULL != j) {
            httpdns_parse_json(conn, j);
            cJSON_Delete(j);
        } else {
            HTTPDNS_ERR("parser failed \n");
        }

        return httpdns_update_cache(conn->dns);
    } else {
       char * host = conn->dns->host_name;
       httpdns_resolv_lock();
       if(NULL != resolv)  {
            HTTPDNS_INFO("<HTTPDNS> %s : delete cache for %s\n", __func__, host);
            dictDelete(resolv, host);
       }
       httpdns_resolv_unlock();
    }
    return -1;
}

static void httpdns_destroy(httpdns_connection_t *conn)
{
    if(NULL == conn) {
        return;
    }
    http_ctx_destroy(&(conn->ctx));
    /* don't free cache, for it should
     * be added to resolv cache */
    free(conn);
}

/* resolv dns and update cache */
static void * httpdns_routine(void * arg)
{
    httpdns_connection_t * conn = (httpdns_connection_t *) arg;
    if(NULL != conn) {
        if(0 == httpdns_process(conn)) {
            HTTPDNS_INFO("<HTTPDNS> %s : httpdns process ok\n", __func__);
        }
        httpdns_destroy(conn);
    }
    pthread_exit(NULL);
    return NULL;
}

static void httpdns_resolv_internal(httpdns_connection_t *conn, int async)
{
    int ret;
    pthread_t tid;
    pthread_attr_t attr;

    HTTPDNS_INFO("<HTTPDNS> %s: pthread_create, async = %d\n", __func__, async);

    if (pthread_attr_init(&attr) == 0) {
        pthread_attr_setstacksize(&attr, 8 * 1024);
        struct sched_param sched;
        sched.sched_priority = 33;
        pthread_attr_setschedparam(&attr, &sched);
        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) == 0) {
            ret = pthread_create(&tid, &attr, &httpdns_routine, conn);
            pthread_setname_np(tid, "httpdns");
        }
        pthread_attr_destroy(&attr);
    }

    if(0 != ret)
    {
        HTTPDNS_INFO("<HTTPDNS> %s : pthread_create error!\n", __func__);
        return;
    }
    if(async) {
        pthread_detach(tid);
    } else {
        pthread_join(tid, NULL);
    }
}

void httpdns_resolv_host(char *host_name, int async)
{
    httpdns_connection_t *conn = (httpdns_connection_t *) malloc(sizeof(httpdns_connection_t));
    if(NULL != conn) {
        if(0 == httpdns_init(conn, NULL, (char *)HTTP_DNS_USER_ID, host_name, HTTPDNS_CONNECT_TIMEOUT, HTTPDNS_TIMEOUT)) {
            httpdns_resolv_internal(conn, async);
        }
    } else {
        HTTPDNS_INFO("<HTTPDNS> Alloc httpdns_connection_t Failed!\n");
    }
}
