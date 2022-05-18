#include <coap3/coap.h>

int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    coap_uri_t uri;
    coap_split_uri(data, size, &uri);
    return 0;
}
