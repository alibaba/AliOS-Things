/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <das/service.h>

extern das_service_t service_sys;

#if (DAS_SERVICE_ROM_ENABLED)
extern das_service_t service_rom;
#endif

#if (DAS_SERVICE_LWIP_NFI_ENABLED)
extern das_service_t service_lwip_nfi;
#endif

#if (DAS_SERVICE_STACK_PROTECTION_ENABLED)
extern das_service_t service_stack_protection;
#endif

#if (DAS_SERVICE_FSCAN_ENABLED)
extern das_service_t service_fscan;
#endif

#if (DAS_SERVICE_NETFILER_ENABLED)
extern das_service_t service_netfilter;
#endif 

das_service_t * das_service_table[] = {
	&service_sys,
#if (DAS_SERVICE_ROM_ENABLED)
    &service_rom,
#endif
#if (DAS_SERVICE_LWIP_NFI_ENABLED)
	&service_lwip_nfi,
#endif
#if (DAS_SERVICE_STACK_PROTECTION_ENABLED)
	&service_stack_protection,
#endif
#if (DAS_SERVICE_FSCAN_ENABLED)
	&service_fscan,
#endif
#if (DAS_SERVICE_NETFILER_ENABLED)
	&service_netfilter,
#endif
    NULL,
};

#if (DAS_SERVICE_LWIP_NFI_ENABLED)
extern das_attest_handler_t lwip_nfi_input_handler;
extern das_attest_handler_t lwip_nfi_output_handler;
extern das_attest_handler_t lwip_nfi_tcp_listen_handler;
#endif

das_attest_handler_t* das_attest_handler_table[] = {
#if (DAS_SERVICE_LWIP_NFI_ENABLED)
	&lwip_nfi_input_handler,
	&lwip_nfi_output_handler,
	&lwip_nfi_tcp_listen_handler,
#endif
	NULL,
};

#if (DAS_SERVICE_NETFILER_ENABLED)
extern netfilter_pre_processor_t imcp_ignore_pre_processor;
extern netfilter_pre_processor_t dns_parser_pre_processor;

netfilter_pre_processor_t *netfilter_pre_processor_table[] = {
	// &imcp_ignore_pre_processor,
	// &dns_parser_pre_processor,
	NULL,
};
#endif // DAS_SERVICE_NETFILER_ENABLED