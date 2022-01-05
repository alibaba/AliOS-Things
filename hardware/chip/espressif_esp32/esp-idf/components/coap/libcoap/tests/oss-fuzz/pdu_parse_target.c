#include <coap2/coap.h>

int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    coap_pdu_t *pdu = coap_pdu_init(0, 0, 0, size);
    if (pdu) {
        coap_pdu_parse(COAP_PROTO_UDP, data, size, pdu);
        coap_delete_pdu(pdu);
    }
    return 0;
}
