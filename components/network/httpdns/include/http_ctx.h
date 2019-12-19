#ifndef HTTP_CTX_H
#define HTTP_CTX_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mem_struct {
    size_t size;
    char *memory;
} memory_t;

typedef struct {
   char * url;
   int conn_timeout;
   int timeout;
   memory_t response;
} http_ctx_t;

int http_ctx_init(http_ctx_t *ctx, char *url, int conn_timeout, int timeout);

memory_t* http_ctx_get(http_ctx_t *ctx);

void http_ctx_destroy(http_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif
