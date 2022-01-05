#include "coap_config.h"
#include <coap.h>

void server_coap_init(void);
/* call this when you think that resources could be dirty */
void server_coap_poll(void);
