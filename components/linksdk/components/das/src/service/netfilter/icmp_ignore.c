#include <das/service.h>

#if (DAS_SERVICE_NETFILER_ENABLED)

static int _imcp_ignore_init (void) {
    return 0;
}

static pre_process_status_t _imcp_ignore_processor (netflow_info_t *info, 
                                                void *payload, 
                                                size_t payload_size, 
                                                char *buf, 
                                                size_t buf_size)
{   
    pre_process_status_t ret = PRE_PROCESS_CALL_NEXT;
    
    if (info == NULL) {
        return PRE_PROCESS_GENERAL_ERROR;
    }

    if (info->protocol == IPPROTO_ICMP || info->protocol == IPPROTO_ICMPV6) {
        ret = PRE_PROCESS_REPORT_IGNORE;
    }
    
    return ret;
}

netfilter_pre_processor_t imcp_ignore_pre_processor = {
    .name = "ICMP_IGNORE_PROCESSOR",
    .init = _imcp_ignore_init,
    .pre_process = _imcp_ignore_processor,
};

#endif